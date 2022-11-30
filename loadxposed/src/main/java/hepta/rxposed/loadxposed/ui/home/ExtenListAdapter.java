package hepta.rxposed.loadxposed.ui.home;

import android.view.View;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.module.LoadMoreModule;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.loadxposed.R;
import hepta.rxposed.loadxposed.Util;


public class ExtenListAdapter extends BaseQuickAdapter<ExtenDataProvider.ExtendInfo, BaseViewHolder> implements LoadMoreModule {


    public ExtenListAdapter(int layoutResId) {
        super(layoutResId);
    }

//    @Override
//    protected void onItemViewHolderCreated(@NotNull BaseViewHolder viewHolder, int viewType) {
//        // 绑定 view
//        DataBindingUtil.bind(viewHolder.itemView);
//    }


    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, ExtenDataProvider.ExtendInfo item) {
        if (item == null) {
            return;
        }
        baseViewHolder.setText(R.id.app_name, item.getAppName());
        baseViewHolder.setText(R.id.description, item.getPackageName());
        baseViewHolder.setImageDrawable(R.id.app_icon,item.getIcon());
        baseViewHolder.findView(R.id.btn_open_app).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Util.StartRxmoduleApplication(item.getPackageName());
            }
        });
//        ListItemRxmoduleBinding binding = DataBindingUtil.getBinding(baseViewHolder.itemView);
//        if (binding != null) {
//            binding.setModuleInfo(moduleInfo);   //setAppInfo 这个函数就是在xml 文件中设置的data variable 里的name,后面的类名和这里直接对应
//            //这里还可以绑定别的，比如item的处理类，然后再xml文件中配置
//        }
        //这样绑定的好处是，不用手工写代码设置数据了，直接在xml文件中设置数据就可以了
    }
}