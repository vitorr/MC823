//Structures used for the database of books.
#ifndef defs
#define defs

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

#endif
