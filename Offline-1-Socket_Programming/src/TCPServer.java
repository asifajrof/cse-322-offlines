import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

public class TCPServer{
    private static int serverPort = 6666;
    private static TCPServer uniqueServerInstance = new TCPServer(serverPort);

    private ServerSocket serverSocket;
    private Socket clientSocket;

    private HashMap<String, User> allUserHashMap = new HashMap<>();
    public HashMap<String, User> getAllUserHashMap() {
        return allUserHashMap;
    }

    private HashMap<String, Files> allFilesHashMap = new HashMap<>();
    public HashMap<String, Files> getAllFilesHashMap() {
        return allFilesHashMap;
    }

    private ArrayList<Requests> fileRequests = new ArrayList<>();
    public ArrayList<Requests> getFileRequests(){
        return fileRequests;
    }

    private Scanner input = new Scanner(System.in);
    private boolean serverShutDown = false;

    private Path serverDirectory = Paths.get(System.getProperty("user.dir"), "Server_Directory");
    public Path getServerDirectory() {
        return serverDirectory;
    }

    public static int MAX_BUFFER_SIZE = 10*1024*1024;
    public static int USED_BUFFER_SIZE = 0;
    public static int MIN_CHUNK_SIZE = 1*1024;
    public static int MAX_CHUNK_SIZE = 4*1024;

    private TCPServer(int serverPort) {
        try {
            this.serverSocket = new ServerSocket(serverPort);
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("Server started...");

        Thread inputTrigger = new Thread(new Runnable()
        {
            @Override
            public void run() {
                while (true) {

                    // read the message to deliver.
                    String msg = input.next();

                    if(msg.equalsIgnoreCase("shutdown"))
                    {
                        System.out.println("Server ShutDown!!!");
                        serverShutDown = true;
                        break;
                    }
                }
            }
        });
        inputTrigger.start();
    }

    public void serverWhileLoop(){
        while(true){
            try {
                if(serverShutDown){
                    //shutdownServer();
                    break;
                }
                this.serverSocket.setSoTimeout(3000);
                try {
                    this.clientSocket = this.serverSocket.accept();
                }catch (SocketTimeoutException e){
                    //e.printStackTrace();
                    continue;
                }
                System.out.println("Connection established with " + this.clientSocket);
                Thread userThread = new UserThread(this.clientSocket);
                userThread.start();
                //runningThreadList.add(userThread);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        try {
            if (this.clientSocket != null) {
                this.clientSocket.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static TCPServer getUniqueServerInstance(){
        return uniqueServerInstance;
    }
    public static void main(String[] args) {
        TCPServer tcpServer = getUniqueServerInstance();
        //System.out.println("current directory : " + System.getProperty("user.dir"));
        tcpServer.serverWhileLoop();
    }
}