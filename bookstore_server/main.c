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
    //Scans the output of ifconfig to get the IP address, and switches 1's for 2's (for security reasons).
    system("sudo /sbin/ifconfig | awk -F':' '/inet addr/&&!/127.0.0.1/{split($2,_,\" \");print _[1]}' | sed s/1/#/g | sed s/2/1/g | sed s/#/2/g > ip.txt");
    //Copies the file to the public HTML location, in order for it to be available to all clients.
    system("scp ip.txt ra091187@ssh.students.ic.unicamp.br:~/public_html/mc823/ip.txt");

    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    int recv_status, op; //Flags for communication with the client: return of the "recv()" function and operation.

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

    while(1) {  // main accept() loop
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
	    recv_status = recv(new_fd, &op, sizeof (int), 0);
	    //Stops when there is an error (-1) or when the client closes the connection (0).
            while (recv_status != 0) {
		if (recv_status == -1) {
 	        	perror("receive");
		}
		//Defines the operation to be done and calls the corresponding function.
		switch (op) {
			case ALL_ISBNS_AND_TITLES:
				get_isbns_and_titles (new_fd);
			break;
			case ISBN_TO_DESCRIPTION:
				get_desc_by_isbn (new_fd);
			break;
			case ISBN_TO_INFO:

			break;
			case ALL_BOOKS_INFO:
			
			break;
			case CHANGE_STOCK:
			
			break;
			case ISBN_TO_STOCK:
			
			break;
			default:
				printf ("Operation not recognized.");
			break;
		}
		//Processes new request from the same client.
		recv_status = recv(new_fd, &op, sizeof (int), 0);
	    }
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}


int get_isbns_and_titles(int socket) {



	return 0;
}

//Obtains the description of the book with a certain ISBN.
int get_desc_by_isbn (int socket) {
	char isbn [ISBN_LENGTH];
	int recv_status;
	book b;
	FILE * db_file;
	char msg [MAX_TEXT]; //Message to be sent to client: book's description.
	int find = 0; //Flag to indicate if ISBN was found (1) or not (0).
	int len;

	//Receives ISBN number from client.
	recv_status = recv(socket, isbn, ISBN_LENGTH, 0);
	if (recv_status == -1) {
        	perror("receive");
	}
	if (recv_status == 0) {
        	perror("receive: connection closed unexpectedly");
	}

	//Gets the structs from the file and checks for the one with the required ISBN.
	db_file = fopen ("./bookstore_database.bin", "rb");
	while ((fread (&b, sizeof (book), 1, db_file)) == 1) {
		if (strcmp (isbn, b.isbn) == 0) {
			strcpy (msg, b.description);
			find = 1;
			break;
		}
	}

	//ISBN found: sends the description.
	if (find ==1) {
		len = ISBN_LENGTH;
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

	return 0;
}
