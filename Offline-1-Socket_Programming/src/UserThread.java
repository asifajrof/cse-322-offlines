import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Date;
import java.util.Map;
import java.util.concurrent.ThreadLocalRandom;

public class UserThread extends Thread{
    private Socket userSocket;
    private String userID;
    private User user;
    private boolean successfulConnection = false;
    private TCPServer uniqueServerInstance;
    private ObjectOutputStream objOutStream;
    private ObjectInputStream objInStream;

    public UserThread(Socket clientSocket){
        this.userSocket = clientSocket;
        uniqueServerInstance =  TCPServer.getUniqueServerInstance();
    }

    private void sendMessage(String msg){
        try {
            objOutStream.writeObject(msg);
            objOutStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run(){
        try {
            objOutStream = new ObjectOutputStream(this.userSocket.getOutputStream());
            objInStream = new ObjectInputStream(this.userSocket.getInputStream());

            //checking id
            String openingMsg;
            openingMsg = "Connection established with server";
            openingMsg += "\n" + "Enter your Student ID: ";
            sendMessage(openingMsg);
            this.userID = (String) objInStream.readObject();
            System.out.println(this.userID);
            if(isNewUser(this.userID)){
                //add a new user
                this.successfulConnection = true;
                user = new User(this.userID, this.userSocket.getInetAddress(), this.userSocket.getPort());
                user.setActive(true);
                user.setUploadPath(Paths.get(uniqueServerInstance.getServerDirectory().toString(), user.getUserID()).toString());
                uniqueServerInstance.getAllUserHashMap().put(user.getUserID(), user);
                System.out.println("new user added " + user.getUserID());
                sendMessage("Welcome to our Server, " + user.getUserID());
                objOutStream.writeBoolean(this.successfulConnection);
                objOutStream.flush();
            }
            else{
                //existing user
                user = uniqueServerInstance.getAllUserHashMap().get(this.userID);
                //check active now. no double instance allowed.
                if(user.isActive()){
                    sendMessage("One instance active for this id already");
                    objOutStream.writeBoolean(this.successfulConnection);
                    objOutStream.flush();
                }
                else{
                    //not active
                    //check previous connection ip
                    if(this.userSocket.getInetAddress().equals(user.getUserIPAddress())){
                        this.successfulConnection = true;
                        user.setActive(true);
                        sendMessage("Welcome back to our Server, " + user.getUserID());
                        objOutStream.writeBoolean(this.successfulConnection);
                        objOutStream.flush();
                    }
                    else{
                        sendMessage("corresponding ip address with this id does not match");
                        objOutStream.writeBoolean(this.successfulConnection);
                        objOutStream.flush();
                    }
                }
            }
            if(this.successfulConnection){
                String msg;
                String sendMsg;
                while(true){
                    userSocket.setSoTimeout(60000);
                    try {
                        msg = (String) objInStream.readObject();
                    } catch (SocketTimeoutException e){
                        e.printStackTrace();
                        System.out.println("user logging out " + user.getUserID());
                        user.setActive(false);
                        //objOutStream.writeObject("Idle for too long. ");
                        //objOutStream.writeObject("Closing connection!!!");
                        //objOutStream.flush();
                        break;
                    }

                    System.out.println(msg);
                    if(msg.equalsIgnoreCase("logout")){
                        System.out.println("user logging out " + user.getUserID());
                        sendMsg = "Closing connection!!!";
                        sendMessage(sendMsg);
                        user.setActive(false);
                        break;
                    }
                    else if(msg.equalsIgnoreCase("ls")){
                        //lookup students
                        sendMsg = "Existing Student list:";
                        User value;
                        for (Map.Entry mapElement : uniqueServerInstance.getAllUserHashMap().entrySet()) {
                            value = (User) mapElement.getValue();
                            sendMsg += "\n" + value.getUserID();
                            if(value.isActive()){
                                sendMsg += " (Active Now)";
                            }
                        }
                        sendMessage(sendMsg);
                    }
                    else if(msg.equalsIgnoreCase("ul")){
                        //upload file
                        sendMsg = "Enter filepath (and file size): ";
                        sendMessage(sendMsg);
                        String filepath = (String) objInStream.readObject();
                        //System.out.println("got filepath" + filepath);
                        long fileSize = objInStream.readLong();
                        //System.out.println("got file size" + fileSize);
                        sendMsg = "is a public file? (y/n)?: ";
                        sendMessage(sendMsg);
                        String filePrivacy = (String) objInStream.readObject();
                        //System.out.println("got file privacy" + filePrivacy);
                        boolean isPublic;
                        if(filePrivacy.equalsIgnoreCase("y")){
                            //is public
                            isPublic = true;
                        }
                        else{
                            isPublic = false;
                        }
                        receiveFile(filepath, fileSize, isPublic);
                    }
                    else if(msg.equalsIgnoreCase("dl")){
                        //download file
                        if(user.getDownloadPath().equalsIgnoreCase("")){
                            //sendMessage("set download path:");
                            //String dlPath = (String) objInStream.readObject();
                            //user.setDownloadPath(dlPath);
                            //setting default current dir
                            Path clientDirectory = Paths.get(System.getProperty("user.dir"), "Client_Directory_" + user.getUserID());
                            user.setDownloadPath(clientDirectory.toString());
                        }
                        sendMessage("enter File ID: ");
                        String fileID = (String) objInStream.readObject();
                        boolean fileValid = false;

                        //getting file instance
                        if(uniqueServerInstance.getAllFilesHashMap().containsKey(fileID)) {
                            Files fileSend = uniqueServerInstance.getAllFilesHashMap().get(fileID);
                            if(fileSend.getUploader().equals(user)){
                                //self file
                                fileValid = true;
                            }
                            else if(fileSend.isPublic()){
                                //public file
                                fileValid = true;
                            }
                            else{
                                fileValid = false;
                            }
                            if(fileValid){
                                objOutStream.writeBoolean(fileValid);
                                String fileDLPath = Paths.get(user.getDownloadPath(), fileSend.getFileName()).toString();
                                sendFile(fileSend.getFilepath(), fileDLPath);
                            }
                            else{
                                sendMessage("Access Denied!!!");
                            }
                        }
                        else{
                            fileValid = false;
                            sendMessage("No file found with that ID!!!");
                        }
                    }
                    else if(msg.equalsIgnoreCase("req")){
                        //request file
                    }
                    else if(msg.equalsIgnoreCase("reqf")){
                        //request fill
                    }
                    else if(msg.equalsIgnoreCase("lfs")){
                        //lookup files self
                        sendMsg = "Self Files List (Public):";
                        for (Files selfFile : user.getPublicFiles()) {
                            sendMsg += "\n" + selfFile.getFileID() + "\t" + selfFile.getFileName();
                        }
                        sendMsg += "\n" + "Self Files List (Private):";
                        for (Files selfFile : user.getPrivateFiles()) {
                            sendMsg += "\n" + selfFile.getFileID() + "\t" + selfFile.getFileName();
                        }
                        sendMessage(sendMsg);
                    }
                    else if(msg.equalsIgnoreCase("lfo")){
                        //lookup files others
                        sendMsg = "Enter Student ID: ";
                        sendMessage(sendMsg);
                        String otherUserID = (String) objInStream.readObject();
                        User otherUser;

                        if(uniqueServerInstance.getAllUserHashMap().containsKey(otherUserID)) {
                            otherUser = uniqueServerInstance.getAllUserHashMap().get(otherUserID);
                            sendMsg = otherUser.getUserID() + " Files List:";
                            for(Files otherFile : otherUser.getPublicFiles()){
                                sendMsg += "\n" + otherFile.getFileID() + "\t" + otherFile.getFileName();
                            }
                        }
                        else{
                            sendMsg = "No User found!!!";
                        }
                        sendMessage(sendMsg);
                    }
                    else if(msg.equalsIgnoreCase("inbox")){
                        //view unread inbox messages
                    }
                    else{
                        //do nothing
                    }
//                    Date date = new Date();
//                    objOutStream.writeObject(date.toString());

                }
            }
            // closing resources
            objInStream.close();
            objOutStream.close();
        } catch (SocketException  e){
            if(e.getMessage().equalsIgnoreCase("connection reset")){
                if(user != null){
                    user.setActive(false);
                    System.out.println("user logging out " + user.getUserID());
                }
            }
            System.out.println(e.toString());
        }
        catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            //System.out.println("connection kete dise");
        }
    }

    private boolean isNewUser(String checkUserID){
        //System.out.println("checking user list");
        if(uniqueServerInstance.getAllUserHashMap().containsKey(checkUserID)){
            //System.out.println("found user");
            return false;
        }
        return true;
    }

    private void receiveFile(String filepath, long fileSize, boolean isPublic){
        Path p = Paths.get(filepath);
        String fileName;
        if(isPublic) {
            fileName = Paths.get(user.getUploadPath(), "public", p.getFileName().toString()).toString();
        }
        else{
            fileName = Paths.get(user.getUploadPath(), "private", p.getFileName().toString()).toString();
        }

        int bytes = 0;
        try {
            FileOutputStream fileOutStream = new FileOutputStream(fileName);
            int bufferSize = ThreadLocalRandom.current().nextInt(TCPServer.MIN_CHUNK_SIZE, TCPServer.MAX_CHUNK_SIZE + 1);
            objOutStream.writeInt(bufferSize);
            objOutStream.flush();
            byte[] buffer = new byte[bufferSize];
            while(fileSize > 0 &&(bytes = objInStream.read(buffer, 0, (int) Math.min(buffer.length, fileSize))) != -1){
                fileOutStream.write(buffer,0,bytes);
                fileSize -= bytes;
            }
            fileOutStream.close();
            sendMessage("Upload Successful!!!");
            Files receivedFile = new Files(fileName, fileSize, user, isPublic);
            user.addFile(receivedFile);
            uniqueServerInstance.getAllFilesHashMap().put(receivedFile.getFileID(), receivedFile);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendFile(String sourcePath, String destinationPath){
        int bytes = 0;
        try {
            File file = new File(sourcePath);
            objOutStream.writeObject(destinationPath);  //filepath sending
            objOutStream.flush();
            objOutStream.writeLong(file.length());  //fileSize sending
            objOutStream.flush();
            FileInputStream fileInputStream = new FileInputStream(file);

            int bufferSize = TCPServer.MAX_CHUNK_SIZE;
            objOutStream.writeInt(bufferSize);
            objOutStream.flush();
            byte[] buffer = new byte[bufferSize];
            while((bytes = fileInputStream.read(buffer)) != -1){
                objOutStream.write(buffer,0,bytes);
                objOutStream.flush();
            }
            fileInputStream.close();
            objOutStream.writeObject("File Download Successful!!!");
            objOutStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
