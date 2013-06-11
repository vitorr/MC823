package unicamp.mc823;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Hello extends Remote {
    String getIsbnsAndTitles() throws RemoteException;
    String getDescByIsbn(String isbn) throws RemoteException;
    String getInfoByIsbn(String isbn) throws RemoteException;
    String getAllInfos() throws RemoteException;
    String changeStockByIsbn(String isbn, int stock) throws RemoteException;
    String getStockByIsbn(String isbn) throws RemoteException;
}
