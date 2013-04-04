//Structures used for the database of books.
#ifndef defs
#define defs

#define MAX_NAME 100
#define MAX_AUTHORS_PER_BOOK 3
#define MAX_TEXT 500
#define ISBN_LENGTH 14 //Standard is 13, but we must consider the '\0' character.

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

#endif
