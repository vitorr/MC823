/*
 Authors: Fernando Lucchesi Bastos Jurema RA 091187
	  Vitor Roberto de Almeida Castro RA 093241
 Online bookstore - Main execution file of the client application. Administers communication between the client and the server,
 implementing six operations to be done in the database.
 Uses the UDP protocol for comparison with its TCP counterpart.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include "../definitions.h"  //Definitions of the bookstore application.

#define PORT "8002" // The port the client will use.

#define QUIT 6 //Menu option to quit.

//Functions for the network operations.
//returns the time taken to receive the request
long long get_isbns_and_titles(int sockfd, char *op);
long long get_desc_by_isbn (int sockfd, char *op);
long long get_info_by_isbn (int sockfd, char *op);
long long get_all_infos (int sockfd, char *op);
long long change_stock_by_isbn (int sockfd, char *op);
long long get_stock_by_isbn (int sockfd, char *op);

//Communicate with the server.
struct addrinfo *server;

//Gets sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Receives and prints the server reponse up to the '|'
//(or, if the message was not completely received, prints the received part).
void recvfrom_print_all(int sockfd)
{
	char buf[MAX_MSG];
	int numbytes;	
	//"complete" = 1 if the full message was received.
	int complete=0, i;
	//Indicates from whom we received.
	struct sockaddr from_addr;
	socklen_t from_len;
	//For using "select()" (non-blocking "recvfrom()").
	fd_set read_socket_set;
	struct timeval timeout; 
	int ret;
	int count = 0;
	struct timeval start, end;

	numbytes = recvfrom(sockfd, buf, MAX_MSG-1, 0, &from_addr, &from_len);

	//Prints the received buffer up until the termination character ("|").
	if(numbytes!=-1){
		buf[numbytes] = '\0';
		for(i=0; i<numbytes; i++)
			if(buf[i]=='|') {
				buf[i]='\0';
				complete = 1;
			}
		printf("%s", buf);
	} else {
		perror("recv");
		exit(1);
	}
	
	if (complete == 0) {
		printf("Message only partially received\n");
	}
	return;
}



int main (int argc, char *argv[]) {
	int sockfd;  
	struct addrinfo hints, *servinfo;
	int rv;
	char s[INET6_ADDRSTRLEN];
	int bookstore=0;
	long long mtime;

	//Verifies if the user has "bookstore" permission.
	if( !strcmp(argv[argc-1], "bookstore") )
		bookstore=1;

    if (argc - bookstore > 2 
	    || ( argc - bookstore > 1 && 0 == strcmp(argv[1], "-h") )
	    || ( argc - bookstore > 1 && 0 == strcmp(argv[1], "--help") ) ) 
	{
	fprintf(stderr,"usage: client operation\n");
		fprintf(stderr,"\t0: List all ISBN with titles\n");
		fprintf(stderr,"\t1: Given an ISBN, show its description\n");
		fprintf(stderr,"\t2: Given an ISBN, show every information available\n");
		fprintf(stderr,"\t3: List everything from every book\n");
		fprintf(stderr,"\t4: Change the stock count of a book (by ISBN)\n");
		fprintf(stderr,"\t5: Given an ISBN, show the available stock\n");
	exit(1);
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	//VERSION THAT GETS IP FROM THE WEB
	//Gets the server ip from the web.
	char ip[100];
    system("curl -s http://www.students.ic.unicamp.br/~ra091187/mc823/ip.txt > ip.txt");
	// switches back the 1s and 2s (changed for security)
    system("cat ip.txt | sed s/1/#/g | sed s/2/1/g | sed s/#/2/g > ip2.txt");
    FILE *f = fopen("ip2.txt", "r");
    fscanf(f, "%s", ip);
    fclose(f);

	if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	//Loop through all the results and connect to the first we can.
	for(server = servinfo; server != NULL; server = server->ai_next) {
		if ((sockfd = socket(server->ai_family, server->ai_socktype,
				server->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		break;
	}

	//Set socket to non-blocking.
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	if (server == NULL) {
		fprintf(stderr, "Client: Failed to get server\n");
		return 2;
	}

	//Getting server information.
	inet_ntop(server->ai_family, get_in_addr((struct sockaddr *)server->ai_addr),
			s, sizeof s);
	printf("Client: communicating with server %s\n", s);

	int quit=0;
	while(!quit)
	{
		char op[OP_LENGTH];
		
		if( argc - bookstore != 2 )
		{
			// Menu
			printf("Please choose the operation:\n");
			printf("\t0: List all ISBN with titles\n");
			printf("\t1: Given an ISBN, show its description\n");
			printf("\t2: Given an ISBN, show every information available\n");
			printf("\t3: List everything from every book\n");
			printf("\t4: Change the stock count of a book (by ISBN)\n");
			printf("\t5: Given an ISBN, show the available stock\n");
			printf("\t6: Exit\n");
			scanf("%s", op);
		}
		else
			strcpy(op, argv[1]);
		
		// chooses the option and executes it
		switch(op[0]-'0')
		{
			case ALL_ISBNS_AND_TITLES:
				mtime = get_isbns_and_titles(sockfd, op);
				break;
			case ISBN_TO_DESCRIPTION:
				mtime = get_desc_by_isbn (sockfd, op);
				break;
			case ISBN_TO_INFO:
				mtime = get_info_by_isbn (sockfd, op);
				break;
			case ALL_BOOKS_INFO:
				mtime = get_all_infos (sockfd, op);
				break;
			case CHANGE_STOCK:
				if(!bookstore)
					printf("Only the \"bookstore\" user can do that!\n");
				else {
					mtime = change_stock_by_isbn (sockfd, op);
				}
				break;
			case ISBN_TO_STOCK:
				mtime = get_stock_by_isbn (sockfd, op);
				break;
			case QUIT:
				quit=1;
				break;
			default:
				printf("\nInvalid option\n");
				break;
		}
		
		if( (op[0]-'0'<6)  && (op[0]-'0'>=0) )
		{
			char fname[40];
			sprintf(fname, "../logs/client_op%s.log", op);
			FILE *f = fopen(fname, "a");
			fprintf(f, "%lld\n", mtime);
			fclose(f);
		}
		
		if( argc - bookstore == 2 ) quit=1;
	}

	close(sockfd);

	freeaddrinfo(servinfo); //Client done with this structure.	

	return 0;
}


long long get_isbns_and_titles(int sockfd, char *op) {
	struct timeval start, end;
	int len;

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);
	
	//Sends the operation code.
	len = OP_AND_PARAMETERS_LENGTH;
	if (sendto (sockfd, op, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}

	//Receives server response.
	recvfrom_print_all(sockfd);
	
	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

long long get_desc_by_isbn (int sockfd, char *op) {
	struct timeval start, end;
	int len;
	char ISBN[ISBN_LENGTH];
	char msg[ISBN_LENGTH + 1 + OP_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Sends the operation code and the ISBN in the same message.
	sprintf (msg, "%c|%s", op[0], ISBN);
	len = OP_AND_PARAMETERS_LENGTH;
	printf("Waiting1...\n");
	if (sendto (sockfd, msg, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}
	printf("Got out1...\n");	

	//Receives server response.
	recvfrom_print_all(sockfd);

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

long long get_info_by_isbn (int sockfd, char *op) {
	struct timeval start, end;
	int len;
	char ISBN[ISBN_LENGTH];
	char msg[OP_LENGTH + 1 + ISBN_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Sends the operation code and the ISBN in the same message.
	sprintf (msg, "%c|%s", op[0], ISBN);
	len = OP_AND_PARAMETERS_LENGTH;
	if (sendto (sockfd, msg, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}

	//Receives server response.
	recvfrom_print_all(sockfd);

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

long long get_all_infos (int sockfd, char *op) {
	struct timeval start, end;
	int len;

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Sends the operation code.
	len = OP_AND_PARAMETERS_LENGTH;
	if (sendto (sockfd, op, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}

	//Receives server response.
	recvfrom_print_all(sockfd);

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);

	return timelapse(start, end);
}

long long change_stock_by_isbn (int sockfd, char *op) {
	struct timeval start, end;
	int len;
	char ISBN[ISBN_LENGTH];
	char stock[INT_LENGTH];
	char msg[OP_LENGTH + 2 + ISBN_LENGTH + INT_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);
	//printf("\nCurrent stock: %d\n", 6666);
	printf("\nPlease type in the new value:\n");
	scanf("%s", stock);

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Sends the operation code, the ISBN and the new stock in the same message.
	sprintf (msg, "%c|%s|%s", op[0], ISBN, stock);
	len = OP_AND_PARAMETERS_LENGTH;
	if (sendto (sockfd, msg, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}

	//Receives server response.
	recvfrom_print_all(sockfd);

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

long long get_stock_by_isbn (int sockfd, char *op) {
	struct timeval start, end;
	int len;
	char ISBN[ISBN_LENGTH];
	char msg[OP_LENGTH + 1 + ISBN_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	//### Marks the start of execution ###//
	gettimeofday(&start, NULL);

	//Sends the operation code and the ISBN in the same message.
	sprintf (msg, "%c|%s", op[0], ISBN);
	len = OP_AND_PARAMETERS_LENGTH;
	if (sendto (sockfd, msg, len, 0, server->ai_addr, server->ai_addrlen) == -1) {
		perror ("sendto");
	}

	//Receives server response.
	recvfrom_print_all(sockfd);

	//### Marks the end of execution ###//
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

