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

	//Add a book.
	strcpy (new_book.title, "The Book Chief");
	strcpy (new_book.authors[0].name, "Markus Suzaku");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Death is the narrator in this story about life, family and love set in the era of World War II");
	strcpy (new_book.publisher, "Pick Up");
	new_book.publishing_year = 2003;
	strcpy (new_book.isbn, "1200000000003");
	new_book.stock = 90;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "He is The Messenger");
	strcpy (new_book.authors[0].name, "Markus Suzaku");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "A touching stories about overcoming obstacles and preconceptions");
	strcpy (new_book.publisher, "Pack Man");
	new_book.publishing_year = 2003;
	strcpy (new_book.isbn, "1200000000004");
	new_book.stock = 95;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "Angu, O Primeiro Guerreiro");
	strcpy (new_book.authors[0].name, "Orlando Pães Neto");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Guerras nórdicas e filosofia");
	strcpy (new_book.publisher, "Editora Universo");
	new_book.publishing_year = 1980;
	strcpy (new_book.isbn, "1200000000005");
	new_book.stock = 10;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "Os Oito");
	strcpy (new_book.authors[0].name, "André Virando");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Vampiros portugueses com poderes especiais atacando o Brasil");
	strcpy (new_book.publisher, "Velho Milênio");
	new_book.publishing_year = 1999;
	strcpy (new_book.isbn, "1200000000006");
	new_book.stock = 115;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "Faustoso");
	strcpy (new_book.authors[0].name, "Wolfgong von Goth");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "A doctor seduced by the devil");
	strcpy (new_book.publisher, "Deutschland Veroffentlichungen");
	new_book.publishing_year = 1820;
	strcpy (new_book.isbn, "1200000000007");
	new_book.stock = 175;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);


	//Add a book.
	strcpy (new_book.title, "Loveless");
	strcpy (new_book.authors[0].name, "Genesis");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Men cry not for themselves, but for their comrades");
	strcpy (new_book.publisher, "Square Enicks");
	new_book.publishing_year = 2008;
	strcpy (new_book.isbn, "1200000000008");
	new_book.stock = 77;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "Herlock Sholmes");
	strcpy (new_book.authors[0].name, "Arthas Klonoa Dole");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "Elementary, my dear Watt");
	strcpy (new_book.publisher, "Jorge New");
	new_book.publishing_year = 1900;
	strcpy (new_book.isbn, "1200000000009");
	new_book.stock = 200;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);

	//Add a book.
	strcpy (new_book.title, "The Miniscule Prince");
	strcpy (new_book.authors[0].name, "Anthony from Saint Exupery");
	strcpy (new_book.authors[1].name, "");
	strcpy (new_book.authors[2].name, "");
	strcpy (new_book.description, "You become eternally responsible for someone you captivate");
	strcpy (new_book.publisher, "Hitchie Cook");
	new_book.publishing_year = 1943;
	strcpy (new_book.isbn, "1200000000010");
	new_book.stock = 55;

	//Write struct to file.
	fwrite (&new_book, 1, sizeof (book), db_file);


	//Finished.
	fclose (db_file);

	return 0;
}
