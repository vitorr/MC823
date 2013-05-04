//Structures used for the database of books.
#ifndef defs
#define defs

#include <sys/time.h>

#define MAX_NAME 100
#define MAX_AUTHORS_PER_BOOK 3
#define MAX_TEXT 500
#define ISBN_LENGTH 14 //Standard is 13, but we must consider the '\0' character.
#define OP_LENGTH 3
#define MAX_MSG 1000
#define INT_LENGTH 10

//Database operations.
#define ALL_ISBNS_AND_TITLES 0
#define ISBN_TO_DESCRIPTION 1
#define ISBN_TO_INFO 2
#define ALL_BOOKS_INFO 3
#define CHANGE_STOCK 4
#define ISBN_TO_STOCK 5

typedef struct _author {
    char name[MAX_NAME];
} Author;

typedef struct _book {
    char title [MAX_NAME];
    Author authors [MAX_AUTHORS_PER_BOOK];
    char description [MAX_TEXT];
    char publisher [MAX_NAME];
    int publishing_year;
    char isbn [ISBN_LENGTH];    
    int stock;
} book;

//Sends data over the network.
//Manages the case when the "send()" function does not send all of the data it was asked to send.
int sendall(int s, char *buf, int *len) {
	int total = 0; // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;
	while(total < *len) {
		n = send(s, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n==-1?-1:0; // return -1 on failure, 0 on success
}

/*
//Receives data from the network and prints it, until it finds the termination char.
//Manages the case when the "recv()" function does not receive all of the data it was sent.
int recvall(int s, char *buf, int *len, char end) {
	int total = 0; // how many bytes we've received
	//int bytesleft = *len; // how many we have left to send
	int n;
	while(total < *len) {
		n = recv(s, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n==-1?-1:0; // return -1 on failure, 0 on success
}
*/

long long timelapse(struct timeval start, struct timeval end) {
	if (1)
	{
		long long useconds;
		useconds = end.tv_usec - start.tv_usec;
		return useconds;
	}
	else
	{
		long long useconds, mtime, seconds;
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		return mtime;
	}
}

#endif
