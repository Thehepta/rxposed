package hepta.rxposed.manager.fragment.PlugExtend;

import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.provider.BaseNodeProvider;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.PlugSupport.FrameData;

public class ExtendNodeProvider extends BaseNodeProvider {


    public CompoundButton CurrentCheckBox = null;
    public FrameData.Frames CurrentbaseNode = null;

    @Override
    public int getItemViewType() {
        return 3;
    }

    @Override
    public int getLayoutId() {
        return R.layout.item_radioframe ;
    }


    @Override
    public void convert(@NonNull BaseViewHolder baseViewHolder, BaseNode baseNode) {
        FrameData.Frames item = (FrameData.Frames) baseNode;
        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.description, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
        CheckBox checkBox = baseViewHolder.findView(R.id.radiobtn);

        checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
//                Log.e("rzx","before status:"+buttonView.isChecked());
                item.setEnable(isChecked);
                if(isChecked){
                    if(CurrentCheckBox == null){
                        CurrentCheckBox = buttonView;
                        CurrentbaseNode = item;
                    }else if(CurrentCheckBox.equals(buttonView)){

                    }else {
                        CurrentCheckBox.setChecked(false);
                        CurrentbaseNode.setEnable(false);
                        CurrentCheckBox = buttonView;
                        CurrentbaseNode = item;
                    }
                }
//                buttonView.isChecked();
                Log.e("rzx","after status:"+buttonView.isChecked());

            }
        });
    }



}
