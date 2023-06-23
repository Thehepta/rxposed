package hepta.rxposed.manager.fragment.LoadModule;

import android.util.Log;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;

public class AppInfoAdapter extends BaseQuickAdapter<ItemInfo, BaseViewHolder > {

    public  AppInfoAdapter(int layoutResId) {
        super(layoutResId);

    }

    private onListener listener = null;
    public void setOnCheckedChangeListener( onListener listener){
        this .listener = listener;
    }



    protected void convert(@NonNull BaseViewHolder baseViewHolder, ItemInfo item) {
        if (item == null) {
            return;
        }
        String appname = item.getAppName();
        String app_packageName = item.getPackageName();
        baseViewHolder.setText(R.id.app_name, appname);
        baseViewHolder.setText(R.id.app_packageName, app_packageName);
        baseViewHolder.setImageDrawable(R.id.app_icon, item.getIcon());
        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
        switchCompat.setChecked(item.getEnable());
        switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

                    if(listener!=null){
                        listener.OnListener(isChecked,app_packageName);
                    }
            }
        });
    }


    public interface  onListener{
        void OnListener(boolean status,String pKgName);
    }
}