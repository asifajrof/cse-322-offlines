import java.io.File;

public class Requests {
    private String requestID;
    private String shortDesc;
    private User requester;
    private Files filledFile;
    private User requestFiller;

    public Requests(User requester, int requestCount, String shortDesc){
        this.requester = requester;
        this.shortDesc = shortDesc;
        this.requestID = requestCount + "_" + this.requester.getUserID();
        this.filledFile = null;
        this.requestFiller = null;
    }

    public void requestFill(Files filledFile, User requestFiller){
        this.filledFile = filledFile;
        this.requestFiller = requestFiller;

        this.notifyRequester();
    }

    private void notifyRequester(){
        String msg;
        if(this.filledFile != null){
            msg = "Request with id " + this.requestID + " filled." +
                    "\n" + "File id : " + this.filledFile.getFileID() +
                    "\n" + "Filled by : " + this.requestFiller.getUserID();
            requester.getInbox().add(msg);
        }
    }

    public String getRequestID() {
        return requestID;
    }

    public String getShortDesc() {
        return shortDesc;
    }

    public User getRequester() {
        return requester;
    }
}
