package hepta.rxposed.manager.fragment.LoadModule;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

public class ModuleListAdapter extends BaseQuickAdapter<AppListAdapter.AppInfo, BaseViewHolder>  {


    private onListener listener;


    public ModuleListAdapter(int layoutResId) {
        super(layoutResId);
    }


    public void setOnAppSelectListener( onListener listener){
        this .listener = listener;
    }
    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, AppInfo item) {
        if (item == null) {
            return;
        }

        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.description, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
        switchCompat.setChecked(item.getEnable());



        if (switchCompat != null) {
            switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    item.setEnable(isChecked);
                    if(listener!= null){
                        listener.OnListener(isChecked,item.getPackageName());
                    }
                }
            });
        }
//        ListItemRxmoduleBinding binding = DataBindingUtil.getBinding(baseViewHolder.itemView);
//        if (binding != null) {
//            binding.setModuleInfo(moduleInfo);   //setAppInfo 这个函数就是在xml 文件中设置的data variable 里的name,后面的类名和这里直接对应
//            //这里还可以绑定别的，比如item的处理类，然后再xml文件中配置
//        }
        //这样绑定的好处是，不用手工写代码设置数据了，直接在xml文件中设置数据就可以了
    }


    public static class AppInfo {

        private  Drawable icon;
        private  ApplicationInfo app;
        private  String packageName;
        private  String versionName;
        private  int versionCode;
        private  String appName;
        public boolean enable;

        public  AppInfo(ApplicationInfo applicationInfo, PackageManager mPm){

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

    public interface  onListener{
        void OnListener(boolean status,String pKgName);
    }
}