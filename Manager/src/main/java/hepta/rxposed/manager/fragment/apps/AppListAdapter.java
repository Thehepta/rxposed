package hepta.rxposed.manager.fragment.apps;

import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.module.LoadMoreModule;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;


public class AppListAdapter extends BaseQuickAdapter<AppInfo, BaseViewHolder> implements LoadMoreModule {

    public AppListAdapter(int layoutResId) {
        super(layoutResId);
    }

//    @Override
//    protected void onItemViewHolderCreated(@NotNull BaseViewHolder viewHolder, int viewType) {
//        // 绑定 view
//        DataBindingUtil.bind(viewHolder.itemView);
//    }

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
        switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                item.setEnable(isChecked);
            }
        });

//        ListItemAppBinding binding = DataBindingUtil.getBinding(baseViewHolder.itemView);
//        if (binding != null) {
//            binding.setAppInfo(item);   //setAppInfo 这个函数就是在xml 文件中设置的data variable 里的name,后面的类名和这里直接对应
//                                        //这里还可以绑定别的，比如item的处理类，然后再xml文件中配置
//        }
        //这样绑定的好处是，不用手工写代码设置数据了，直接在xml文件中设置数据就可以了

    }

}