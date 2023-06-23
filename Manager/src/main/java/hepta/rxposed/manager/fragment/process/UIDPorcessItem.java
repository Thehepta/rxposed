package hepta.rxposed.manager.fragment.process;

import android.graphics.drawable.Drawable;

import java.io.Serializable;
import java.util.List;

public class UIDPorcessItem implements Serializable {


    private List<subprocessItem> childNode;
    private int Uid;
    Drawable icon;
    private String UserName;
    private String AppName;
    private String pkgName;
    public UIDPorcessItem(List<subprocessItem> childNode, String UserName, int Uid, String AppName, Drawable icon) {
        // 默认不展开
        this.childNode = childNode;
        this.UserName = UserName;
        this.Uid = Uid;
        this.AppName = AppName;
        this.icon = icon;
    }

    public String getUserName() {
        return UserName;
    }

    public int getUid() {
        return Uid;
    }

    public void setUid(int uid) {
        Uid = uid;
    }

    public void setUserName(String userName) {
        UserName = userName;
    }


    public String getPkgName() {
        return pkgName;
    }

    public String getAppName() {
        return AppName;
    }

    public void setPkgName(String pkgName) {
        this.pkgName = pkgName;
    }


    public Drawable getIcon() {
        return icon;
    }

    public void setIcon(Drawable icon) {
        this.icon = icon;
    }

    public List<subprocessItem> getChildNode() {
        return childNode;
    }

    public void setChildNode(List<subprocessItem> childNode) {
        this.childNode = childNode;
    }



    public static class subprocessItem {
        String pid, processName;
        subprocessItem(String str, String str2){
            pid= str;
            processName=str2;
        }


        public String getPid() {
            return pid;
        }

        public String getProcessName() {
            return processName;
        }
    }
}