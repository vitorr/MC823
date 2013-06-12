package unicamp.mc823;

import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.io.*;

public class Server implements Hello {

    static Database database;

    public Server() {}

    public String getIsbnsAndTitles()
    {
	long end, init=System.nanoTime();
	String response = database.getIsbnsAndTitles();
	end = System.nanoTime();
	saveTime(0, end-init);

	return response;
    }

    public String getDescByIsbn(String isbn)
    {
	long end, init=System.nanoTime();
	String response = database.getDescByIsbn(isbn);
	end = System.nanoTime();
	saveTime(1, end-init);

	return response;
    }

    public String getInfoByIsbn(String isbn)
    {
	long end, init=System.nanoTime();
	String response = database.getInfoByIsbn(isbn);
	end = System.nanoTime();
	saveTime(2, end-init);

	return response;
    }

    public String getAllInfos()
    {
	long end, init=System.nanoTime();
	String response = database.getAllInfos();
	end = System.nanoTime();
	saveTime(3, end-init);

	return response;
    }

    public String changeStockByIsbn(String isbn, int stock)
    {
	long end, init=System.nanoTime();
	String response = database.changeStockByIsbn(isbn, stock);
	end = System.nanoTime();
	saveTime(4, end-init);

	return response;
    }

    public String getStockByIsbn(String isbn)
    {
	long end, init=System.nanoTime();
	String response = database.getStockByIsbn(isbn);
	end = System.nanoTime();
	saveTime(5, end-init);

	return response;
    }

    private void saveTime(int op, long mtime)
    {
	// Write to file
	PrintStream out = null;
	try {
	    out = new PrintStream(new FileOutputStream("../logs/server_op" + op + ".log", true));
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


    public static void main(String args[]) {
	database = new Database();

	try {
	    Server obj = new Server();
	    Hello stub = (Hello) UnicastRemoteObject.exportObject(obj, 0);

	    // Bind the remote object's stub in the registry
	    Registry registry = LocateRegistry.getRegistry();
	    registry.rebind("Hello", stub);

	    System.err.println("Server ready");
	} catch (Exception e) {
	    System.err.println("Server exception: " + e.toString());
	    e.printStackTrace();
	}
    }
}
