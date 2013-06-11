package unicamp.mc823;
	
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
	
public class Server implements Hello {
	
    public Server() {}

    public String getIsbnsAndTitles()
    {
	return "";
    }

    public String getDescByIsbn(String isbn)
    {
	return "it works! received: " + isbn;
    }

    public String getInfoByIsbn(String isbn)
    {
	return "";
    }

    public String getAllInfos()
    {
	return "";
    }

    public String changeStockByIsbn(String isbn, int stock)
    {
	return "";
    }

    public String getStockByIsbn(String isbn)
    {
	return "";
    }
    
    private void saveTime(int op, long start, long end)
    {
    }


    public static void main(String args[]) {
	
	try {
	    Server obj = new Server();
	    Hello stub = (Hello) UnicastRemoteObject.exportObject(obj, 0);

	    // Bind the remote object's stub in the registry
	    Registry registry = LocateRegistry.getRegistry();
	    registry.bind("Hello", stub);

	    System.err.println("Server ready");
	} catch (Exception e) {
	    System.err.println("Server exception: " + e.toString());
	    e.printStackTrace();
	}
    }
}
