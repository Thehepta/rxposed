package hepta.rxposed.manager.fragment.LoadModule;

import android.util.Log;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.base.AppItemInfo;

public class AppInfoAdapter extends BaseQuickAdapter<AppItemInfo, BaseViewHolder > {

    public  AppInfoAdapter(int layoutResId) {
        super(layoutResId);

    }

    private onListener listener = null;
    public void setOnCheckedChangeListener( onListener listener){
        this .listener = listener;
    }



    protected void convert(@NonNull BaseViewHolder baseViewHolder, AppItemInfo item) {
        if (item == null) {
            return;
        }
        String appname = item.getAppName();
        String app_packageName = item.getPackageName();
        baseViewHolder.setText(R.id.app_name, appname);
        baseViewHolder.setText(R.id.app_packageName, app_packageName);
        baseViewHolder.setImageDrawable(R.id.app_icon, item.getIcon());
        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
        if(item.getEnable() == true){
            Log.e("Rzxconvert",app_packageName);
        }
        switchCompat.setChecked(item.getEnable());
        switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                item.setEnable(isChecked);
                    if(listener!=null){
                        listener.OnListener(isChecked,app_packageName);
                    }
            }
        });
    }


    @Override
    protected int getDefItemViewType(int position) {   //不写这个容易导致item混乱
        return position;
    }

    public interface  onListener{
        void OnListener(boolean status,String pKgName);
    }
}