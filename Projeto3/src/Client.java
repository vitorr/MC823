package unicamp.mc823;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.io.*;

public class Client {

    private Client() {}
    
    public static void getIsbnsAndTitles(Hello stub)
    {
	try {
	    String response = stub.getIsbnsAndTitles();
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void getDescByIsbn(Hello stub)
    {
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    
	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();
	    
	    String response = stub.getDescByIsbn(isbn);
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void getInfoByIsbn(Hello stub)
    {
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    
	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();
	    
	    String response = stub.getInfoByIsbn(isbn);
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void getAllInfos(Hello stub)
    {
	try {
	    String response = stub.getAllInfos();
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void changeStockByIsbn(Hello stub)
    {
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    
	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();
	    
	    System.out.print("\nPlease type in the new value:\n");
	    int stock = Integer.parseInt(br.readLine());
	    
	    String response = stub.changeStockByIsbn(isbn, stock);
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void getStockByIsbn(Hello stub)
    {
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    
	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();
	    
	    String response = stub.getStockByIsbn(isbn);
	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }

    public static void main(String[] args) {
	
	//System.out.println("Starting client...");

	//String host = (args.length < 1) ? null : args[0];
	String host = null;
	try {
	    Registry registry = LocateRegistry.getRegistry(host);
	    Hello stub = (Hello) registry.lookup("Hello");
	    
	    if (args.length<1)
	    {
		System.err.println("usage: make client OP=operation B=bookstore");
		System.err.println("\t0: List all ISBN with titles");
		System.err.println("\t1: Given an ISBN, show its description");
		System.err.println("\t2: Given an ISBN, show every information available");
		System.err.println("\t3: List everything from every book");
		System.err.println("\t4: Change the stock count of a book (by ISBN)");
		System.err.println("\t5: Given an ISBN, show the available stock");
	    }
	    else
	    {
		int op = Integer.parseInt(args[0]);
		switch (op) {
		    case 0:
			getIsbnsAndTitles(stub);
			break;
		    case 1:
			getDescByIsbn(stub);
			break;
		    case 2:
			getInfoByIsbn(stub);
			break;
		    case 3:
			getAllInfos(stub);
			break;
		    case 4:
			changeStockByIsbn(stub);
			break;
		    case 5:
			getStockByIsbn(stub);
			break;
		    default:
			break;
		}
	    }
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }
}
