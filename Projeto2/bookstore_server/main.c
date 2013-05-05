/*
 Projeto 2 de MC823 - Laboratório de Teleprocessamento e Redes
 Grupo: Fernando Lucchesi Bastos Jurema RA 091187
	Vitor Roberto de Almeida Castro RA 093241
 Online bookstore - Main execution file of the server application. Administers communication between the client and the server,
 implementing six operations to be done in the database.
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
#include <sys/time.h>
#include <fcntl.h>

//Definitions specific to the bookstore application.
#include "../definitions.h"

#define PORT "8002"  // The port the server offers up to clients will be connecting to.

#define BACKLOG 10     // How many pending connections queue will hold.

//Functions for the database operations which return the time taken to process the request.
long get_isbns_and_titles(int sockfd, char* buf, struct sockaddr *client_addr);
long get_desc_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr);
long get_info_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr);
long get_all_infos (int sockfd, char* buf, struct sockaddr *client_addr);
long change_stock_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr);
long get_stock_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr);

//Auxiliary to the above functions.
int book_by_isbn (int sockfd, char isbn [ISBN_LENGTH], book * res);

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

int main(int argc, char *argv[])
{
	//only if the last argument is 'ip'
	if(!strcmp(argv[argc-1], "ip")) {
		//Scans the output of ifconfig to get the IP address, and switches 1's for 2's (for security reasons).
		system("sudo /sbin/ifconfig | awk -F':' '/inet addr/&&!/127.0.0.1/{split($2,_,\" \");print _[1]}' | sed s/1/#/g | sed s/2/1/g | sed s/#/2/g > ip.txt");
		//Copies the file to the public HTML location, in order for it to be available to all clients.
		system("scp ip.txt ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/ip.txt");
	}

    //Flags for communication with the client: operation string.
    int op;
	char ip_rcv[15];
	
	//variable to get the processing time
	long mtime;
	
	
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAX_MSG];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	
	//Set socket to non-blocking.
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

    //Main "accept()" loop.
    while(1) {  
		printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAX_MSG-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			//perror("recvfrom");
			//exit(1);
			continue;
		}

		// Gets the ip of the client
		strcpy(ip_rcv, inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s));
		printf("listener: got packet from %s\n", ip_rcv);
		printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
		printf("listener: packet contains \"%s\"\n", buf);
		
		// DEBUG
		if(!strcmp(buf, "end")) break;
		
		
		sscanf(buf, "%d", &op);
		printf("op: \"%d\"\n", op);
		
		struct sockaddr *client_addr = (struct sockaddr*)&their_addr;
		//Defines the operation to be done and calls the corresponding function.
		switch (op) {
			case ALL_ISBNS_AND_TITLES:
				mtime = get_isbns_and_titles (sockfd, buf, client_addr);
			break;
			case ISBN_TO_DESCRIPTION:
				mtime = get_desc_by_isbn (sockfd, buf, client_addr);
			break;
			case ISBN_TO_INFO:
				mtime = get_info_by_isbn(sockfd, buf, client_addr);
			break;
			case ALL_BOOKS_INFO:
				mtime = get_all_infos(sockfd, buf, client_addr);
			break;
			case CHANGE_STOCK:
				mtime = change_stock_by_isbn (sockfd, buf, client_addr);
			break;
			case ISBN_TO_STOCK:
				mtime = get_stock_by_isbn (sockfd, buf, client_addr);
			break;
			default:
				printf ("Operation not recognized.\n");
			break;
		}
		//stores the time in the log
		if( (op<6)  && (op>=0) )
		{
			char fname[40];
			sprintf(fname, "../logs/server_op%d.log", op);
			FILE *f = fopen(fname, "a");
			//fprintf(f, "Elapsed time: %ld milliseconds\n", mtime);
			fprintf(f, "%ld\n", mtime);
			fclose(f);
		}
    }
	
	close(sockfd);

    return 0;
}

//Obtains the ISBN and title of each book in the database.
long get_isbns_and_titles(int sockfd, char* buf, struct sockaddr *client_addr) {
	FILE * db_file;
	char msg [MAX_MSG]=""; //Message to be sent to client
	char tmp [MAX_MSG]=""; //Temporary variable
	book b;
	struct timeval start, end;
	int numbytes;

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Gets the structs from the file and sends the relevant information.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		//Sends reply to be printed on the client.
		snprintf (tmp, MAX_MSG, "ISBN: %s\nTitle: %s\n\n", b.isbn, b.title); 
		strncat(msg, tmp, MAX_MSG-2);
	}
	fclose (db_file);

	//Sends last information ("|" string) to indicate the end of the database.
	strncat(msg, "|", MAX_MSG-1);
	
	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

//Obtains the description of the book with a certain ISBN.
long get_desc_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr) {
	char msg [MAX_MSG]=""; //Message to be sent to client
	char tmp [MAX_MSG]=""; //Temporary variable
	book * req_book; //Book with the required ISBN.
	char isbn [ISBN_LENGTH];
	struct timeval start, end;
	int numbytes;


	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);
	
	//Gets the ISBN number from client.
	sscanf(buf, "%[^|]|%[^|]", tmp, isbn);
	printf("isbn: %s\n", isbn);
	
	
	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the description.
	if (book_by_isbn (sockfd, isbn, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		strcpy (tmp, req_book->description);
		strcat (tmp, "|");
		free (req_book);
		
		strncat(msg, tmp, MAX_MSG-2);
	//ISBN not found: replies accordingly.
	} else {
		strcpy (msg, "ISBN not in database\n");
		free (req_book);
		
		strncat(msg, tmp, MAX_MSG-2);
	}
	
	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

//Obtains all of the information of the book with a certain ISBN.
long get_info_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr) {
	char msg [MAX_MSG]=""; //Message to be sent to client
	char tmp [MAX_MSG]=""; //Temporary variable
	book * req_book; //Book with the required ISBN.
	char isbn [ISBN_LENGTH];
	struct timeval start, end;
	int numbytes;
	
	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);
	
	//Gets the ISBN number from client.
	sscanf(buf, "%[^|]|%[^|]", tmp, isbn);
	printf("isbn: %s\n", isbn);
	
	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the information.
	if (book_by_isbn (sockfd, isbn, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		//Builds the message string.
		snprintf (tmp, MAX_MSG, "Title: %s\nAuthors: %s\n%s\n%s\nDescription: %s\nPublisher: %s\nPublishing year: %d\nISBN: %s\nStock: %d\n\n|", req_book->title, req_book->authors[0].name, req_book->authors[1].name, req_book->authors[2].name, req_book->description, req_book->publisher, req_book->publishing_year, req_book->isbn, req_book->stock);
		
		free (req_book);

		strncat(msg, tmp, MAX_MSG-2);
	//ISBN not found: replies accordingly.
	} else {
		strcpy (tmp, "ISBN not in database\n");
		
		strncat(msg, tmp, MAX_MSG-2);
	}
	
	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

//Obtains all of the information of each book in the database. This information consists of
//title, authors, description, publisher, publishing year, ISBN and stock.
long get_all_infos (int sockfd, char* buf, struct sockaddr *client_addr) {
	FILE * db_file;
	char msg [MAX_MSG+2]=""; //Message to be sent to client
	char tmp [MAX_MSG+2]=""; //Temporary variable
	book b;
	struct timeval start, end;
	int numbytes;
	
	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Gets the structs from the file and sends the information.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		//Sends reply to be printed on the client.
		snprintf (tmp, MAX_MSG, "Title: %s\nAuthors: %s\n%s\n%s\nDescription: %s\nPublisher: %s\nPublishing year: %d\nISBN: %s\nStock: %d\n\n", b.title, b.authors[0].name, b.authors[1].name, b.authors[2].name, b.description, b.publisher, b.publishing_year, b.isbn, b.stock);
		
		if(strlen(tmp) + strlen(msg) < MAX_MSG)
			strncat(msg, tmp, MAX_MSG-2);
	}

	//Sends last information ("|" string) to indicate the end of the database.
	strncat(msg, "|", MAX_MSG-1);

	fclose (db_file);
	
	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	
	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

//Change the stock of a book with a certain ISBN.
//Receives an ISBN from client, sohows the correspondent book's stock, receives new stock value,
//changes it and shows change confirmation.
//Only for the user with special permissions ("Bookstore").
long change_stock_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr) {
	char msg [MAX_MSG]=""; //Message to be sent to client
	char tmp [MAX_MSG]=""; //Temporary variable
	book * req_book; //Book with the required ISBN.
	char isbn [ISBN_LENGTH];
	book b; //Auxiliary.
	FILE * db_file;
	int find = 0; //Flag which indicates if the required ISBN was found (1) or not (0).
	int new_stock;
	struct timeval start, end;
	int numbytes;
	
	
	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);
	
	//Gets the ISBN number and new stock from client.
	sscanf(buf, "%[^|]|%[^|]|%d", tmp, isbn, &new_stock);
	printf("isbn: %s\n", isbn);
	printf("stock: %d\n", new_stock);
	

	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

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
		strcpy (msg, "ISBN not in database\n:");
		// Sends everything
		if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
				 client_addr, sizeof(*client_addr))) == -1) {
			perror("talker: sendto");
			exit(1);
		}
		return 0;
	}

	//Changes stock according to client.
	req_book->stock = new_stock;

	//Writes changes to database.
	fseek (db_file, (-1) * (sizeof (book)), SEEK_CUR);  //Goes back one struct.
	fwrite (req_book, 1, sizeof (book), db_file);  //Writes modified struct.

	//Done with file.
	fclose (db_file);

	//Informs new stock.
	snprintf (msg, MAX_MSG, "Operation complete!\nNew stock: %d\n|", new_stock);
	
	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	free (req_book);
	
	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

//Obtains the number of copies (stock) of the book with a certain ISBN.
long get_stock_by_isbn (int sockfd, char* buf, struct sockaddr *client_addr) {
	char msg [MAX_MSG]=""; //Message to be sent to client
	char tmp [MAX_MSG]=""; //Temporary variable
	book * req_book; //Book with the required ISBN.
	char isbn [ISBN_LENGTH];
	struct timeval start, end;
	int numbytes;
	
	
	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);
	
	//Receives ISBN number from client.
	sscanf(buf, "%[^|]|%[^|]", tmp, isbn);
	printf("isbn: %s\n", isbn);


	//Initializes memory.
	req_book = (book *) malloc (sizeof (book));

	//ISBN found: sends the stock.
	if (book_by_isbn (sockfd, isbn, req_book) == 1) { //Gets the book with "isbn" in "*req_book".
		snprintf (msg, MAX_MSG, "%d\n|", req_book->stock);
	//ISBN not found: replies accordingly.
	} else {
		strcpy (msg, "ISBN not in database\n");
	}
	
	free (req_book);

	// Sends everything
	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 client_addr, sizeof(*client_addr))) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	
	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

//Gets "res" to point to a book struct with "isbn" (obtained from a client's request) as its ISBN.
//Assumes that "res" is pointing to a book struct beforehand.
//Returns 1 if the book was found, 0 if it was not.
int book_by_isbn (int sockfd, char isbn [ISBN_LENGTH], book * res) {
	book b;
	FILE * db_file;

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

