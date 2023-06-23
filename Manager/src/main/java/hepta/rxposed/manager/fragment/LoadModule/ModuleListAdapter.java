package hepta.rxposed.manager.fragment.LoadModule;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;

public class ModuleListAdapter extends BaseQuickAdapter<ItemInfo, BaseViewHolder>  {




    public ModuleListAdapter(int layoutResId) {
        super(layoutResId);
    }


    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, ItemInfo item) {
        if (item == null) {
            return;
        }

        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.app_packageName, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
//        SwitchCompat switchCompat = baseViewHolder.findView(R.id.switcher);
//        switchCompat.setChecked(item.getEnable());
//
//
//
//        if (switchCompat != null) {
//            switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
//                @Override
//                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
//                    item.setEnable(isChecked);
//                    if(listener!= null){
//                        listener.OnListener(isChecked,item.getPackageName());
//                    }
//                }
//            });
//        }
//        ListItemRxmoduleBinding binding = DataBindingUtil.getBinding(baseViewHolder.itemView);
//        if (binding != null) {
//            binding.setModuleInfo(moduleInfo);   //setAppInfo 这个函数就是在xml 文件中设置的data variable 里的name,后面的类名和这里直接对应
//            //这里还可以绑定别的，比如item的处理类，然后再xml文件中配置
//        }
        //这样绑定的好处是，不用手工写代码设置数据了，直接在xml文件中设置数据就可以了
    }

}