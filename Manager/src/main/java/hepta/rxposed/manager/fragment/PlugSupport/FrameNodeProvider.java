package hepta.rxposed.manager.fragment.depends;

import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.provider.BaseNodeProvider;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.base.AppModule;
import hepta.rxposed.manager.fragment.extend.ModuleData;


//在applistFragment 使用，用于自定义部分recycleview显示的内容
public class FrameNodeProvider extends BaseNodeProvider {


    @Override
    public int getItemViewType() {
        return 3;
    }

    @Override
    public int getLayoutId() {
        return R.layout.item_application;
    }

    @Override
    public void convert(@NonNull BaseViewHolder baseViewHolder, BaseNode baseNode) {
        FrameData.Frames item = (FrameData.Frames) baseNode;
        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.description, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
        switchCompat.setChecked(item.getEnable());
        switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                item.setEnable(isChecked);
            }
        });
    }
}
