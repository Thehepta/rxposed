package hepta.rxposed.manager.fragment.process;

import android.graphics.drawable.Drawable;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;

import java.io.Serializable;
import java.util.List;

public class UIDPorcessNode  implements Serializable {


    private List<subprocess> childNode;
    private int Uid;
    Drawable icon;
    private String UserName;
    private String AppName;
    private String pkgName;
    public UIDPorcessNode(List<subprocess> childNode, String UserName, int Uid,String AppName,Drawable icon) {
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

    public List<subprocess> getChildNode() {
        return childNode;
    }

    public void setChildNode(List<subprocess> childNode) {
        this.childNode = childNode;
    }



    public static class subprocess{
        String pid, processName;
        subprocess(String str,String str2){
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