package hepta.rxposed.manager.fragment.PlugSupport;

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
import hepta.rxposed.manager.fragment.PlugExtend.ModuleData;


//在applistFragment 使用，用于自定义部分recycleview显示的内容
public class FrameNodeProvider extends BaseNodeProvider {




    @Override
    public int getItemViewType() {
        return 2;
    }



    @Override
    public int getLayoutId() {
        return R.layout.item_application;
    }


    @Override
    public void convert(@NonNull BaseViewHolder baseViewHolder, BaseNode baseNode) {
        ModuleData.Modules item = (ModuleData.Modules) baseNode;
        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.description, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
        switchCompat.setVisibility(View.INVISIBLE);
    }



}
