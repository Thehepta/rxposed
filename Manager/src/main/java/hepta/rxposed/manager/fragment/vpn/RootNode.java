package hepta.rxposed.manager.fragment.vpn;

import android.content.pm.ApplicationInfo;
import android.graphics.drawable.Drawable;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

public class RootNode extends BaseExpandNode {


    private List<BaseNode> childNode;
    private int Uid;
    Drawable icon;
    private String UserName;
    private String pkgName;
    public RootNode(List<BaseNode> childNode, String UserName, int Uid) {
        // 默认不展开
        setExpanded(false);
        this.childNode = childNode;
        this.UserName = UserName;
        this.Uid = Uid;
    }

    public String getUserName() {
        return UserName;
    }

    public String getUid() {
        return String.valueOf(Uid);
    }

    public int getUID() {
        return Uid;
    }

    public String getPkgName() {
        return pkgName;
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

    @Nullable
    @Override
    public List<BaseNode> getChildNode() {
        return childNode;
    }


}