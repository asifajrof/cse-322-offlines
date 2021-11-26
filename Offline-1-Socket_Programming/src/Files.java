import java.nio.file.Paths;

public class Files {
    private String fileID;
    private String filepath;
    private String fileName;
    private User uploader;
    private long fileSize;
    private boolean isPublic;

    public Files(String filepath, long fileSize, User uploader, boolean isPublic){
        this.filepath = filepath;
        this.setFileName(this.filepath);
        this.fileSize = fileSize;
        this.uploader = uploader;
        this.isPublic = isPublic;
        this.setFileID();
    }

    public String getFileID() {
        return fileID;
    }

    public void setFileID() {
        this.fileID = this.uploader.getUserID() + "_" + this.uploader.getFileCount();
    }

    public String getFilepath() {
        return filepath;
    }

    public void setFilepath(String filepath) {
        this.filepath = filepath;
    }

    public String getFileName(){
        return fileName;
    }

    public void setFileName(String filepath){
        this.fileName = Paths.get(filepath).getFileName().toString();
    }

    public User getUploader() {
        return uploader;
    }

    public void setUploader(User uploader) {
        this.uploader = uploader;
    }

    public long getFileSize() {
        return fileSize;
    }

    public void setFileSize(long fileSize) {
        this.fileSize = fileSize;
    }

    public boolean isPublic() {
        return isPublic;
    }

    public void setPublic(boolean aPublic) {
        isPublic = aPublic;
    }
}
