package unicamp.mc823;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.io.*;

public class Client {

    private Client() {}

    public static long getIsbnsAndTitles(Hello stub)
    {
	long init=0, end=-1;
	try {
	    init = System.nanoTime();
	    String response = stub.getIsbnsAndTitles();
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    public static long getDescByIsbn(Hello stub)
    {
	long init=0, end=-1;
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();

	    init = System.nanoTime();
	    String response = stub.getDescByIsbn(isbn);
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    public static long getInfoByIsbn(Hello stub)
    {
	long init=0, end=-1;
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();

	    init = System.nanoTime();
	    String response = stub.getInfoByIsbn(isbn);
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    public static long getAllInfos(Hello stub)
    {
	long init=0, end=-1;
	try {
	    init = System.nanoTime();
	    String response = stub.getAllInfos();
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    public static long changeStockByIsbn(Hello stub)
    {
	long init=0, end=-1;
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();

	    System.out.print("\nPlease type in the new value:\n");
	    int stock = Integer.parseInt(br.readLine());

	    init = System.nanoTime();
	    String response = stub.changeStockByIsbn(isbn, stock);
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    public static long getStockByIsbn(Hello stub)
    {
	long init=0, end=-1;
	try {
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

	    System.out.print("\nPlease type in the ISBN:\n");
	    String isbn = br.readLine();

	    init = System.nanoTime();
	    String response = stub.getStockByIsbn(isbn);
	    end = System.nanoTime();

	    System.out.println(response);
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
	return end-init;
    }

    private static void usage()
    {
	System.err.println("usage: make client OP=operation U=user");
	System.err.println("\t0: List all ISBN with titles");
	System.err.println("\t1: Given an ISBN, show its description");
	System.err.println("\t2: Given an ISBN, show every information available");
	System.err.println("\t3: List everything from every book");
	System.err.println("\t4: Change the stock count of a book (by ISBN)");
	System.err.println("\t5: Given an ISBN, show the available stock");
    }

    public static void main(String[] args) {

	// Gets the operation
	int op;
	try {
	    op = Integer.parseInt(args[0]);
	}
	catch (Exception e) {
	    usage();
	    return;
	}

	// Verifies if the user has "bookstore" permission.
	boolean bookstore = false;
	if( args[args.length-1].equals("bookstore") )
	    bookstore = true;

	// Defines the host
	String host = null;
	try {
	    InputStream fis = new FileInputStream("ip.txt");
	    BufferedReader br = new BufferedReader(new InputStreamReader(fis));
	    host = br.readLine();
	    
	    // Done with the file
	    br.close();
	}
	catch(Exception e) {
	    System.err.println("No \"ip2.txt\" found, running locally");
	    System.err.println("Run \"make get_ip\" to get the host address");
	}
	
	try {
	    Registry registry = LocateRegistry.getRegistry(host);
	    Hello stub = (Hello) registry.lookup("Hello");

	    long mtime = 0;

	    switch (op) {
		case 0:
		    mtime = getIsbnsAndTitles(stub);
		    break;
		case 1:
		    mtime = getDescByIsbn(stub);
		    break;
		case 2:
		    mtime = getInfoByIsbn(stub);
		    break;
		case 3:
		    mtime = getAllInfos(stub);
		    break;
		case 4:
		    if(!bookstore) {
			System.err.println("Only the \"bookstore\" user can do that!");
			return;
		    }
		    else
			mtime = changeStockByIsbn(stub);
		    break;
		case 5:
		    mtime = getStockByIsbn(stub);
		    break;
		default:
		    usage();
		    break;
	    }

	    if( (op < 6) && (op >= 0) && (mtime != -1))
	    {
		// Write to file
		PrintStream out = null;
		try {
		    out = new PrintStream(new FileOutputStream("../logs/client_op" + op + ".log", true));
		    out.print(mtime + "\n");
		}
		catch(Exception e) {
		    System.err.println("Caught Exception: " + e);
		    e.printStackTrace();
		}
		finally {
		    if (out != null) out.close();
		}
	    }
	} catch (Exception e) {
	    System.err.println("Client exception: " + e.toString());
	    e.printStackTrace();
	}
    }
}
