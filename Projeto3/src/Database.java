package unicamp.mc823;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.io.*;
import java.util.*;


public class Database {

    ArrayList<Book> list;
    
    private Database() {
	InputStream    fis;
	BufferedReader br;
	String         line;
	
	list = new ArrayList<Book>();
	
	try{
	    fis = new FileInputStream("src/database.txt");
	    br = new BufferedReader(new InputStreamReader(fis));
	    
	    while ((line = br.readLine()) != null) {
		String tmp[] = line.split("\\|");
		Book book = new Book(tmp);
		
		list.add(book);
	    }
	    
	    // Done with the file
	    br.close();
	    br = null;
	    fis = null;
	}
	catch(Exception e) {
	    System.err.println("Caught Exception: " + e);
	    e.printStackTrace();
	}
    }
    
    public void print() {
	for(Book a: list)
	    a.print();
    }
    
    public void toFile() {
	String s = "";
	for(Book a: list) {
	    s += a.title + "|";
	    s += a.authors.get(0) + "|";
	    s += a.authors.get(1) + "|";
	    s += a.authors.get(2) + "|";
	    s += a.description + "|";
	    s += a.publisher + "|";
	    s += a.publishing_year + "|";
	    s += a.isbn + "|";
	    s += a.stock;
	    s += "\n";
	}
	
	// todo: write this to a file
	System.out.println(s);
    }

    public static void main(String[] args) {
	Database bla = new Database();
	bla.print();
	bla.toFile();
    }
}
