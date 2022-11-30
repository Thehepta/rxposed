package hepta.rxposed.loadxposed.ui.home;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class ModuleInfo extends BaseNode {



    private final Drawable icon;
    private final ApplicationInfo app;
    private final String packageName;
    private final String versionName;
    private final int versionCode;
    private final String appName;
    public boolean enable;
    private final Map<Integer, AppInfoNode> AppInfoMaps;
    public ModuleInfo(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfoNode> appInfoMap){
        this.app = pkg.applicationInfo;
        this.packageName = pkg.packageName;
        this.versionName = pkg.versionName;
        this.versionCode = pkg.versionCode;
        this.AppInfoMaps = appInfoMap;
        this.AppInfoMaps.remove(app.uid);
        this.icon = pkg.applicationInfo.loadIcon(mPm);
        this.appName= pkg.applicationInfo.loadLabel(mPm).toString();
        this.enable = false;
    }

    public ApplicationInfo getApp() {
        return app;
    }

    public Drawable getIcon() {
        return icon;
    }

    public String getAppName() {
        return appName;
    }

    public int getUID() {
        return app.uid;
    }


    public String getPackageName() {
        return packageName;
    }

    public boolean getEnable() {
        return enable;
    }

    public void setEnable(boolean enable) {
        this.enable = enable;
        return;
    }
    public List<AppInfoNode> getAppInfoList() {
        List<AppInfoNode> result = new ArrayList(AppInfoMaps.values());
        return result;
    }

    public Map<Integer, AppInfoNode> getAppInfoMaps() {
        return AppInfoMaps;
    }


    @Nullable
    @Override
    public List<BaseNode> getChildNode() {
        return null;
    }
}