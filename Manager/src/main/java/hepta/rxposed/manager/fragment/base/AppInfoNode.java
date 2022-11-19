package hepta.rxposed.manager.fragment.base;


import android.content.pm.ApplicationInfo;
import android.graphics.drawable.Drawable;
import android.util.Log;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;


public class AppInfoNode extends BaseNode {
    private String moduleName;
    private ApplicationInfo appInfo;
    private Drawable icon;
    private String appName;
    private String packageName;
    private boolean isSystemApp;
    public boolean enable;
    private long codesize;
    public long getCodesize() {
        return codesize;
    }
    public void setCodesize(long codesize) {
        this.codesize = codesize;
    }
    public Drawable getIcon() {
        return icon;
    }
    public void setIcon(Drawable icon) {
        this.icon = icon;
    }
    public String getAppName() {
        return appName;
    }
    public void setAppName(String appName) {
        this.appName = appName;
    }
    public String getPackageName() {
        return packageName;
    }
    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }
    public boolean isSystemApp() {
        return isSystemApp;
    }
    public void setSystemApp(boolean isSystemApp) {
        this.isSystemApp = isSystemApp;

    }
    public int getUID() {
        return appInfo.uid;
    }



    public void setEnable(boolean enable) {
        Log.e("Rzx","setHookStatus:"+enable+" appName:"+appName+" uid ="+getUID());
        this.enable = enable;
//        ModuleInfoProvider.getInstance().UpdateConfig();
    }

    public void setModuleName(String moduleName) {
        this.moduleName = moduleName;
    }

    public boolean getEnable() {
        return enable;
    }

    public ApplicationInfo getAppInfo() {
        return appInfo;
    }

    public void setAppInfo(ApplicationInfo appInfo) {
        this.appInfo = appInfo;
    }



    @Nullable
    @Override
    public List<BaseNode> getChildNode() {
        return null;
    }
}
