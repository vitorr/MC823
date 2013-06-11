package unicamp.mc823;
	
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
	
public class Server implements Hello {
    
    static Database database;
    
    public Server() {}

    public String getIsbnsAndTitles()
    {
	String response = database.getIsbnsAndTitles();
	return response;
    }

    public String getDescByIsbn(String isbn)
    {
	String response = database.getDescByIsbn(isbn);
	return response;
    }

    public String getInfoByIsbn(String isbn)
    {
	String response = database.getInfoByIsbn(isbn);
	return response;
    }

    public String getAllInfos()
    {
	String response = database.getAllInfos();
	return response;
    }

    public String changeStockByIsbn(String isbn, int stock)
    {
	String response = database.changeStockByIsbn(isbn, stock);
	return response;
    }

    public String getStockByIsbn(String isbn)
    {
	String response = database.getStockByIsbn(isbn);
	return response;
    }
    
    private void saveTime(int op, long start, long end)
    {
    }


    public static void main(String args[]) {
	database = new Database();
	
	try {
	    Server obj = new Server();
	    Hello stub = (Hello) UnicastRemoteObject.exportObject(obj, 9001);

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
