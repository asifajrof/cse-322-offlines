import java.io.*;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;

public class TCPClient{
    private Socket socket;
    private Scanner input;
    private ObjectOutputStream objOutStream;
    private ObjectInputStream objInStream;
    private boolean userLogout = false;

    public TCPClient(InetAddress serverIPAddress, int serverPort){
        try {
            this.socket = new Socket(serverIPAddress, serverPort);
            objOutStream = new ObjectOutputStream(this.socket.getOutputStream());
            objInStream = new ObjectInputStream(this.socket.getInputStream());
            input = new Scanner(System.in);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendMessage(String msg){
        try {
            objOutStream.writeObject(msg);
            objOutStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private String receiveMessage(){
        try {
            String msg = (String) objInStream.readObject();
            return msg;
        } catch (SocketException e){
            if(e.getMessage().equalsIgnoreCase("An established connection was aborted by the software in your host machine")){
                return "exit()";
            }
            else{
                e.printStackTrace();
                return "";
            }
        } catch (ClassNotFoundException | IOException e) {
            e.printStackTrace();
            return "";
        }
    }

    public boolean authenticate(){
        System.out.println(receiveMessage());
        sendMessage(input.nextLine());
        System.out.println(receiveMessage());

        try {
            return objInStream.readBoolean();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    public void startCommunication(){
        String msg;
        String rcvMsg = "";
        while (!userLogout) {
            // read the message to deliver.
            msg = input.nextLine();

            if(msg.equalsIgnoreCase("logout"))
            {
                sendMessage(msg);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
                userLogout = true;
                break;
            }
            else if(msg.equalsIgnoreCase("ls")){
                //lookup students
                sendMessage(msg);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
            }
            else if(msg.equalsIgnoreCase("ul"))
            {
                //upload file
                sendMessage(msg);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg); //enter filepath size
                String filepath = input.nextLine();
                sendFile(filepath);
            }
            else if(msg.equalsIgnoreCase("dl")){
                //download file
                sendMessage(msg);

                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
                if(rcvMsg.equalsIgnoreCase("set download path:")){
                    String dlPath = input.nextLine();
                    sendMessage(dlPath);
                    rcvMsg = receiveMessage();
                    System.out.println(rcvMsg);
                }
                String fileID = input.nextLine();
                receiveFile(fileID);
            }
            else if(msg.equalsIgnoreCase("lfs")){
                //lookup files self
                sendMessage(msg);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
            }
            else if(msg.equalsIgnoreCase("lfo")){
                //lookup files others
                sendMessage(msg);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
                String otherUserID = input.nextLine();
                sendMessage(otherUserID);
                rcvMsg = receiveMessage();
                System.out.println(rcvMsg);
            }
            else{
                //do nothing
            }

            if(rcvMsg.equalsIgnoreCase("exit()")){
                userLogout = true;
            }
        }
    }

    private void sendFile(String filepath){
        int bytes = 0;
        try {
            File file = new File(filepath);
            sendMessage(filepath);  //filepath sending
            objOutStream.writeLong(file.length());  //fileSize sending
            objOutStream.flush();
            System.out.println(receiveMessage());   //file privacy
            String filePrivacy = input.nextLine();
            sendMessage(filePrivacy);  //public? y/n
            FileInputStream fileInputStream = new FileInputStream(file);

            int bufferSize = objInStream.readInt();
            byte[] buffer = new byte[bufferSize];
            while((bytes = fileInputStream.read(buffer)) != -1){
                objOutStream.write(buffer,0,bytes);
                objOutStream.flush();
            }
            fileInputStream.close();
            System.out.println(receiveMessage());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void receiveFile(String fileID){
        int bytes = 0;
        try {
            sendMessage(fileID);
            boolean idValid = objInStream.readBoolean();
            if(idValid){
                String fileName = (String) objInStream.readObject();
                long fileSize = objInStream.readLong();
                FileOutputStream fileOutStream = new FileOutputStream(fileName);

                int bufferSize = objInStream.readInt();
                byte[] buffer = new byte[bufferSize];
                while(fileSize > 0 &&(bytes = objInStream.read(buffer, 0, (int) Math.min(buffer.length, fileSize))) != -1){
                    fileOutStream.write(buffer,0,bytes);
                    fileSize -= bytes;
                }
                fileOutStream.close();
                System.out.println(receiveMessage());
            }
            else{
                System.out.println(receiveMessage());
            }
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        InetAddress serverIPAddress = null;
        try {
            //serverIPAddress = InetAddress.getByName("192.168.1.2");
            serverIPAddress = InetAddress.getByName("localhost");
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        int serverPort = 6666;
        TCPClient tcpClient = new TCPClient(serverIPAddress, serverPort);
        if(tcpClient.authenticate()) {
            tcpClient.startCommunication();
        }
        else{
            System.out.println("Authentication failed. Closing connection!!!");
        }
    }
}