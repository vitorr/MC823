#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definitions specific to the bookstore application.
#include "../definitions.h"

int main () {
	FILE * db_file;
	book new_book;

	//Open for writing. Discards previous file with the same name.
	db_file = fopen ("./bookstore_database.bin", "w+b");

	//Add a book.
	strcpy (new_book.title, "HP and the Philosopher Stone");
	strcpy (new_book.authors[0].name, "J.K. Rolling");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "First HP");
	strcpy (new_book.publisher, "Bloomsblurred");
	new_book.publishing_year = 1990;
	strcpy (new_book.isbn, "1200000000001");
	new_book.stock = 100;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "The Picasso Code");
	strcpy (new_book.authors[0].name, "Dan Purple");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Conspiracies and action");
	strcpy (new_book.publisher, "Doublefine");
	new_book.publishing_year = 2000;
	strcpy (new_book.isbn, "1200000000002");
	new_book.stock = 125;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Finished.
	fclose (db_file);

	return 0;
}
