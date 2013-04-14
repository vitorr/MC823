/*
** client.c -- a stream socket client demo
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

#include <arpa/inet.h>

#include "../definitions.h"  //Definitions of the bookstore application.

#define PORT "8001" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define QUIT 6 // menu option to quit

//Functions for the network operations.
//returns the time taken to receive the request
long get_isbns_and_titles(int sockfd, char *op);
long get_desc_by_isbn (int sockfd, char *op);
long get_info_by_isbn (int sockfd, char *op);
long get_all_infos (int sockfd, char *op);
long change_stock_by_isbn (int sockfd, char *op);
long get_stock_by_isbn (int sockfd, char *op);

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
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	int bookstore=0;
	long mtime;

	//verify if the user has "bookstore" permission
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
		fprintf(stderr,"\t3: LIst everything from every book\n");
		fprintf(stderr,"\t4: Change the stock count of a book (by ISBN)\n");
		fprintf(stderr,"\t5: Given an ISBN, show the available stock\n");
        exit(1);
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/*VERSION THAT GETS IP FROM THE WEB
	// gets the server ip from the web
	char ip[100];
    system("curl -s http://www.students.ic.unicamp.br/~ra091187/mc823/ip.txt > ip.txt");
	// switches back the 1s and 2s (changed for security)
    system("cat ip.txt | sed s/1/#/g | sed s/2/1/g | sed s/#/2/g > ip.txt");
    FILE *f = fopen("ip.txt", "r");
    fscanf(f, "%s", ip);
    fclose(f);

	int i;
	for(i=0; i<5; i++)
		if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) == 0) break;
	if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	*/
	//Debug version: server and client in the same machine.
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {	
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	//End of changes in debug version.

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	// connecting to the server
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	
	int quit=0;
	while(!quit)
	{
		char op[3];
		
		if( argc - bookstore != 2 )
		{
			// Menu
			printf("Please choose the operation:\n");
			printf("\t0: List all ISBN with titles\n");
			printf("\t1: Given an ISBN, show its description\n");
			printf("\t2: Given an ISBN, show every information available\n");
			printf("\t3: LIst everything from every book\n");
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
			//fprintf(f, "Elapsed time: %ld milliseconds\n", mtime);
			fprintf(f, "%ld\n", mtime);
			fclose(f);
		}
		
		if( argc - bookstore == 2 ) quit=1;
	}

	close(sockfd);


	return 0;
}


long get_isbns_and_titles(int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;

	// marks the start of execution
	gettimeofday(&start, NULL);
	
	// sends the operation code
	len = 3;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// recvs and prints everything while there is no '|'
	while(1)
	{
		int end=0, i;
		numbytes = recv(sockfd, buf, MAX_MSG-1, 0);
		if(numbytes!=-1){
			buf[numbytes] = '\0';
			for(i=0; i<numbytes; i++)
				if(buf[i]=='|') {
					buf[i]='\0';
					end = 1;
				}
			printf("%s", buf);
			if(end) break;
		}
		else {
			perror("recv");
			exit(1);
		}
	}
	
	// marks the end of execution
	gettimeofday(&end, NULL);
	
	return timelapse(start, end);
}

long get_desc_by_isbn (int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;
	char ISBN[ISBN_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	// marks the start of execution
	gettimeofday(&start, NULL);

	// send the operation code
	len = 3;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// sends the ISBN
	len = strlen (ISBN);
	sendall(sockfd, ISBN, &len);

	// gets the answer
	if ((numbytes = recv(sockfd, buf, MAX_MSG-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	// marks the end of execution
	gettimeofday(&end, NULL);

	printf ("numbytes:%d\n", numbytes);
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	return timelapse(start, end);
}

long get_info_by_isbn (int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;
	char ISBN[ISBN_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	// marks the start of execution
	gettimeofday(&start, NULL);

	// send the operation code
	len = 3;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// sends the ISBN
	len = strlen (ISBN);
	sendall(sockfd, ISBN, &len);

	// gets the answer
	if ((numbytes = recv(sockfd, buf, MAX_MSG-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	// marks the end of execution
	gettimeofday(&end, NULL);

	printf ("numbytes:%d\n", numbytes);
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	return timelapse(start, end);
}

long get_all_infos (int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;

	// send the operation code
	len = 3;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// recvs and prints everything while there is no '|'
	while(1)
	{
		int end=0, i;
		numbytes = recv(sockfd, buf, MAX_MSG-1, 0);
		if(numbytes!=-1){
			buf[numbytes] = '\0';
			for(i=0; i<numbytes; i++)
				if(buf[i]=='|') {
					buf[i]='\0';
					end = 1;
				}
			printf("%s", buf);
			if(end) break;
		}
		else {
			perror("recv");
			exit(1);
		}
	}

	return timelapse(start, end);
}

long change_stock_by_isbn (int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;
	char ISBN[ISBN_LENGTH];
	char stock[20];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);
	//printf("\nCurrent stock: %d\n", 6666);
	printf("\nPlease type in the new value:\n");
	scanf("%s", stock);
	printf("recebi isso aqui: %s\n", stock);

	// sends the operation code
	len = OP_LENGTH;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}
	// sends the ISBN
	len = ISBN_LENGTH;
	if (sendall(sockfd, ISBN, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}
	// sends the new stock
	len = INT_LENGTH;
	if (sendall(sockfd, stock, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// gets the answer
	if ((numbytes = recv(sockfd, buf, MAX_MSG-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	// marks the end of execution
	gettimeofday(&end, NULL);

	printf ("numbytes:%d\n", numbytes);
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
	
	return timelapse(start, end);
}

long get_stock_by_isbn (int sockfd, char *op)
{
	int len;
	struct timeval start, end;
	char buf[MAX_MSG];
	int numbytes;
	char ISBN[ISBN_LENGTH];

	printf("\nPlease type in the ISBN:\n");
	scanf("%s", ISBN);

	// marks the start of execution
	gettimeofday(&start, NULL);

	// send the operation code
	len = 3;
	if (sendall(sockfd, op, &len) == -1) {
		perror ("sendall");
		printf ("Only %d bytes sent.\n", len);
	}

	// sends the ISBN
	len = strlen (ISBN);
	sendall(sockfd, ISBN, &len);

	// gets the answer
	if ((numbytes = recv(sockfd, buf, MAX_MSG-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	// marks the end of execution
	gettimeofday(&end, NULL);

	printf ("numbytes:%d\n", numbytes);
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
	
	return timelapse(start, end);
}

