/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

//Definitions specific to the bookstore application.
#include "../definitions.h"

#define PORT "8001"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

//Functions for the database operations.
int get_isbns_and_titles(int socket);
int get_desc_by_isbn (int socket);
int get_info_by_isbn (int socket);
int get_all_infos (int socket);
int change_stock_by_isbn (int socket);
int get_stock_by_isbn (int socket);

//Auxiliary to the above functions.
int book_by_isbn (int socket, book * res);

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    /*//Scans the output of ifconfig to get the IP address, and switches 1's for 2's (for security reasons).
    system("sudo /sbin/ifconfig | awk -F':' '/inet addr/&&!/127.0.0.1/{split($2,_,\" \");print _[1]}' | sed s/1/#/g | sed s/2/1/g | sed s/#/2/g > ip.txt");
    //Copies the file to the public HTML location, in order for it to be available to all clients.
    system("scp ip.txt ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/ip.txt");*/

    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    //Flags for communication with the client: return of the "recv()" function and operation string.
    char op[OP_LENGTH];
    int recv_status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    //Main "accept()" loop.
    while(1) {  
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener

            //Processing of the client request, specific to the database.
	    recv_status = recv(new_fd, op, OP_LENGTH, 0);
            op[recv_status] = '\0'; //Adds the "end of string" character to the received string.
            printf("op: \"%s\"\n", op);
            printf("recv_status: \"%d\"\n", recv_status);
	    //Stops when there is an error (-1) or when the client closes the connection (0).
            while (recv_status != 0) {
		if (recv_status == -1) {
 	        	perror("receive");
		}
		//Defines the operation to be done and calls the corresponding function.
		switch (op[0] - '0') {
			case ALL_ISBNS_AND_TITLES:
				get_isbns_and_titles (new_fd);
			break;
			case ISBN_TO_DESCRIPTION:
				get_desc_by_isbn (new_fd);
			break;
			case ISBN_TO_INFO:
				get_info_by_isbn(new_fd);
			break;
			case ALL_BOOKS_INFO:
				get_all_infos(new_fd);
			break;
			case CHANGE_STOCK:
				change_stock_by_isbn (new_fd);
			break;
			case ISBN_TO_STOCK:
				get_stock_by_isbn (new_fd);
			break;
			default:
				printf ("Operation not recognized.");
			break;
		}
		//Processes new request from the same client.
		recv_status = recv(new_fd, op, OP_LENGTH, 0);
            	op[recv_status] = '\0'; //Adds the "end of string" character to the received string.
	    }
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

//Obtains the ISBN and title of each book in the database.
int get_isbns_and_titles(int socket) {
	FILE * db_file;
	char msg [MAX_MSG]; //Message to be sent to client: ISBN and title.
	int len;
	book b;

	//Gets the structs from the file and sends the relevant information.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		//Sends reply to be printed on the client.
		snprintf (msg, MAX_MSG, "ISBN: %s\nTitle: %s\n\n", b.isbn, b.title); 
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}	
	}

	//Sends last information ("|" string) to indicate the end of the database.
	snprintf (msg, MAX_MSG, "|"); 
	len = strlen (msg);
	if (sendall (socket, msg, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}	

	fclose (db_file);

	return 0;
}

//Obtains the description of the book with a certain ISBN.
int get_desc_by_isbn (int socket) {
	char msg [MAX_MSG]; //Message to be sent to client: book's description.
	int len;
	book * req_book; //Book with the required ISBN.

	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the description.
	if (book_by_isbn (socket, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		strcpy (msg, req_book->description);
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	//ISBN not found: replies accordingly.
	} else {
		strcpy (msg, "ISBN not in database");
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	}

	free (req_book);

	return 0;
}

//Obtains all of the information of the book with a certain ISBN.
int get_info_by_isbn (int socket) {
	char msg [MAX_MSG]; //Message to be sent to client: full information.
	int len;
	book * req_book; //Book with the required ISBN.

	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the information.
	if (book_by_isbn (socket, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		//Builds the message string.
		snprintf (msg, MAX_MSG, "Title: %s\nAuthors: %s\n%s\n%s\nDescription: %s\nPublisher: %s\nPublishing year: %d\nISBN: %s\nStock: %d\n\n", req_book->title, req_book->authors[0].name, req_book->authors[1].name, req_book->authors[2].name, req_book->description, req_book->publisher, req_book->publishing_year, req_book->isbn, req_book->stock);
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	//ISBN not found: replies accordingly.
	} else {
		strcpy (msg, "ISBN not in database");
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	}

	free (req_book);

	return 0;
}

//Obtains all of the information of each book in the database. This information consists of
//title, authors, description, publisher, publishing year, ISBN and stock.
int get_all_infos (int socket) {
	FILE * db_file;
	char msg [MAX_MSG]; //Message to be sent to client: complete information.
	int len;
	book b;

	//Gets the structs from the file and sends the information.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		//Sends reply to be printed on the client.
		snprintf (msg, MAX_MSG, "Title: %s\nAuthors: %s\n%s\n%s\nDescription: %s\nPublisher: %s\nPublishing year: %d\nISBN: %s\nStock: %d\n\n", b.title, b.authors[0].name, b.authors[1].name, b.authors[2].name, b.description, b.publisher, b.publishing_year, b.isbn, b.stock);
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}	
	}

	//Sends last information ("|" string) to indicate the end of the database.
	snprintf (msg, MAX_MSG, "|"); 
	len = strlen (msg);
	if (sendall (socket, msg, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}	

	fclose (db_file);

	return 0;
}

//Change the stock of a book with a certain ISBN.
//Receives an ISBN from client, sohows the correspondent book's stock, receives new stock value,
//changes it and shows change confirmation.
//Only for the user with special permissions ("Bookstore").
int change_stock_by_isbn (int socket) {
	char msg [MAX_MSG]; //Message to be sent to client: Current stock and new stock.
	int len;
	book * req_book; //Book with the required ISBN.
	char isbn [ISBN_LENGTH];
	int recv_status;
	book b; //Auxiliary.
	FILE * db_file;
	int find = 0; //Flag which indicates if the required ISBN was found (1) or not (0).
	int new_stock;
	char buff [INT_LENGTH]; //Buffer for receiving new stock (as a string).

	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//Receives ISBN number from client.
	recv_status = recv(socket, isbn, ISBN_LENGTH, 0);
	if (recv_status == -1) {
        	perror("receive");
	}
	if (recv_status == 0) {
        	perror("receive - connection closed unexpectedly");
	}
        isbn[recv_status] = '\0';
        printf("isbn: %s\n", isbn);

	//Gets the structs from the file and checks for the one with the required ISBN.
	db_file = fopen ("./bookstore_database.bin", "rb+");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		if (strcmp (isbn, b.isbn) == 0) {
			(*req_book) = b;
			find = 1;
			break;
		}
	}

	//ISBN not found: replies accordingly and ends function.
	if (find == 0) {
		strcpy (msg, "ISBN not in database");
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
		return 0;
	}

	//Informs current stock.
	snprintf (msg, MAX_MSG, "Current stock: %d\n", req_book->stock);
	len = strlen (msg);
	if (sendall (socket, msg, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}
	
	//Changes stock according to client.
	recv_status = recv(socket, buff, INT_LENGTH, 0);
	if (recv_status == -1) {
        	perror("receive");
	}
	if (recv_status == 0) {
        	perror("receive - connection closed unexpectedly");
	}
        buff[recv_status] = '\0';
	new_stock = atoi (buff);
	req_book->stock = new_stock;

	//Writes changes to database.
	fseek (db_file, (-1) * (sizeof (book)), SEEK_CUR);  //Goes back one struct.
	fwrite (req_book, 1, sizeof (book), db_file);  //Writes modified struct.

	//Done with file.
	fclose (db_file);

	//Informs new stock.
	snprintf (msg, MAX_MSG, "Operation complete!\nNew stock: %d\n", new_stock);
	len = strlen (msg);
	if (sendall (socket, msg, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	free (req_book);

	return 0;
}

//Obtains the number of copies (stock) of the book with a certain ISBN.
int get_stock_by_isbn (int socket) {
	char msg [MAX_MSG]; //Message to be sent to client: stock.
	int len;
	book * req_book; //Book with the required ISBN.

	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the stock.
	if (book_by_isbn (socket, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		snprintf (msg, MAX_MSG, "%d\n", req_book->stock);
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	//ISBN not found: replies accordingly.
	} else {
		strcpy (msg, "ISBN not in database");
		len = strlen (msg);
		if (sendall (socket, msg, &len) == -1) {
			perror ("sendall");
			printf ("Only %d bytes sent.\n", len);
		}
	}

	free (req_book);

	return 0;
}

//Gets "res" to point to a book struct with "isbn" (obtained from a client's request) as its ISBN.
//Assumes that "res" is pointing to a book struct beforehand.
//Returns 1 if the book was found, 0 if it was not.
int book_by_isbn (int socket, book * res) {
	char isbn [ISBN_LENGTH];
	int recv_status;
	book b;
	FILE * db_file;

	//Receives ISBN number from client.
	recv_status = recv(socket, isbn, ISBN_LENGTH, 0);
	if (recv_status == -1) {
        	perror("receive");
	}
	if (recv_status == 0) {
        	perror("receive - connection closed unexpectedly");
	}
        isbn[recv_status] = '\0';
        printf("isbn: %s\n", isbn);

	//Gets the structs from the file and checks for the one with the required ISBN.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		if (strcmp (isbn, b.isbn) == 0) {
			(*res) = b;
			return 1;
		}
	}

	fclose (db_file);

	return 0;
}

