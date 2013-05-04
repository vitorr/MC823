#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definitions specific to the bookstore application.
#include "../definitions.h"

int main () {
	FILE * db_file;
	book current_book;

	//Open for reading only.
	db_file = fopen ("./bookstore_database.bin", "rb");

	//Read and print all of the books.
	while ((fread (&current_book, sizeof (book), 1, db_file)) == 1) {
		printf ("Title: %s\nAuthors: %s\nDescription: %s\nPublisher: %s\nPublishing year: %d\nISBN: %s\nStock: %d\n\n", current_book.title, current_book.authors[0].name, current_book.description, current_book.publisher, current_book.publishing_year, current_book.isbn, current_book.stock);
	}

	//Finished.
	fclose (db_file);

	return 0;
}
