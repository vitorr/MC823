package unicamp.mc823;

import java.util.*;

public class Book {

    String all[];
    
    String title;
    ArrayList<String> authors;
    String description;
    String publisher;
    int publishing_year;
    String isbn;    
    int stock;
    
    public Book(String arr[])
    {
		authors = new ArrayList<String>();
		
		all = arr;
		title = arr[0];
		authors.add(arr[1]);
		authors.add(arr[2]);
		authors.add(arr[3]);
		description = arr[4];
		publisher = arr[5];
		publishing_year = Integer.parseInt(arr[6]);
		isbn = arr[7];
		stock = Integer.parseInt(arr[8]);
	}

	public void print()
	{
		System.out.println(title);
	}
}
