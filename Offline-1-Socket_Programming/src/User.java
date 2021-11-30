import java.io.File;
import java.net.InetAddress;
import java.util.ArrayList;

public class User {
    private String userID;
    private InetAddress userIPAddress;
    private int userPort;
    private String uploadPath;
    private String downloadPath;
    private ArrayList<Files> publicFiles;
    private ArrayList<Files> privateFiles;
    private ArrayList<String> inbox;
    private boolean isActive;

    public User(String userID, InetAddress userIPAddress, int userPort){
        this.userID = userID;
        this.userIPAddress = userIPAddress;
        this.userPort = userPort;
        this.uploadPath = "";
        this.downloadPath = "";
        this.publicFiles = new ArrayList<>();
        this.privateFiles = new ArrayList<>();
        this.inbox = new ArrayList<>();
    }

    public String getUserID() {
        return userID;
    }
//    public void setUserID(String userID) {
//        this.userID = userID;
//    }
    public InetAddress getUserIPAddress() {
        return userIPAddress;
    }
//    public void setUserIPAddress(InetAddress userIPAddress) {
//        this.userIPAddress = userIPAddress;
//    }
//    public int getUserPort() {
//        return userPort;
//    }
//    public void setUserPort(int userPort) {
//        this.userPort = userPort;
//    }
    public String getUploadPath() {
        return uploadPath;
    }
    public void setUploadPath(String uploadPath) {
        this.uploadPath = uploadPath;
        new File(this.uploadPath).mkdirs();
        new File(this.uploadPath,"public").mkdirs();
        new File(this.uploadPath,"private").mkdirs();
    }
    public String getDownloadPath() {
        return downloadPath;
    }
    public void setDownloadPath(String downloadPath) {
        this.downloadPath = downloadPath;
        new File(this.downloadPath).mkdirs();
    }
    public ArrayList<Files> getPublicFiles() {
        return publicFiles;
    }
//    public void setPublicFiles(ArrayList<Files> publicFiles) {
//        this.publicFiles = publicFiles;
//    }
    public ArrayList<Files> getPrivateFiles() {
        return privateFiles;
    }
//    public void setPrivateFiles(ArrayList<Files> privateFiles) {
//        this.privateFiles = privateFiles;
//    }
    public void addFile(Files newFile){
        if(newFile.isPublic()){
            this.publicFiles.add(newFile);
        }
        else{
            this.privateFiles.add(newFile);
        }
    }

    public void addInInbox(String msgToAdd){
        this.inbox.add(msgToAdd);
    }

    public ArrayList<String> getInbox() {
        return inbox;
    }
//    public void setInbox(ArrayList<String> inbox) {
//        this.inbox = inbox;
//    }
    public boolean isActive() {
        return isActive;
    }
    public void setActive(boolean active) {
        isActive = active;
    }
    public int getFileCount(){
        return this.publicFiles.size() + this.privateFiles.size();
    }
}
