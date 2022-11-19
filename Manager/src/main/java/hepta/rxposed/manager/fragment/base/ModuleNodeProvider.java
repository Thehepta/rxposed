package hepta.rxposed.manager.fragment.base;

import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.provider.BaseNodeProvider;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;

public class ModuleNodeProvider extends BaseNodeProvider {


    @Override
    public int getItemViewType() {
        return 1;
    }

    @Override
    public int getLayoutId() {
        return R.layout.item_application;
    }

    @Override
    public void convert(@NonNull BaseViewHolder baseViewHolder, BaseNode baseNode) {
        ModuleInfo item = (ModuleInfo) baseNode;
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
