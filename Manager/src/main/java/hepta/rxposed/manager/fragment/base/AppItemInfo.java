package hepta.rxposed.manager.fragment.base;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;

public class AppItemInfo {

    private Drawable icon;
    private ApplicationInfo app;
    private  String packageName;
    private  String versionName;
    private  int versionCode;
    private  String appName;
    public boolean enable;
    public ApplicationInfo applicationInfo;
    public AppItemInfo(ApplicationInfo applicationInfo, PackageManager mPm){
        this.applicationInfo = applicationInfo;
        this.packageName = applicationInfo.packageName;
        this.icon = applicationInfo.loadIcon(mPm);
        this.appName= applicationInfo.loadLabel(mPm).toString();
        this.enable = false;
    }

    public String getAppName() {
        return appName;
    }
    public String getPackageName() {
        return packageName;
    }
    public Drawable getIcon() {
        return icon;
    }

    public boolean getEnable() {
        return enable;
    }

    public void setEnable(boolean enable) {
        this.enable = enable;
    }
}

