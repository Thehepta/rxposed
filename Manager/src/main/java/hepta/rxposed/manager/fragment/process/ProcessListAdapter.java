package hepta.rxposed.manager.fragment.process;

import android.graphics.drawable.Drawable;
import android.view.View;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.module.LoadMoreModule;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;


public class ProcessListAdapter extends BaseQuickAdapter<UIDPorcessNode, BaseViewHolder> implements LoadMoreModule {

    private OnClickListener onClickListener = null;

    public ProcessListAdapter(int layoutResId) {
        super(layoutResId);
    }


    public void setOnClickListener(OnClickListener onClickListener ){
        this.onClickListener = onClickListener;
    }

    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, UIDPorcessNode item) {
        if (item == null) {
            return;
        }
        baseViewHolder.setText(R.id.user_id, String.valueOf(item.getUid()));
        baseViewHolder.setText(R.id.app_packageName, item.getPkgName());
        baseViewHolder.setText(R.id.user_name, item.getUserName());
        Drawable icon = item.getIcon();
        baseViewHolder.setImageDrawable(R.id.app_icon,icon);

        baseViewHolder.findView(R.id.inject).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(onClickListener!= null){
                    onClickListener.onClick(item);
                }
            }
        });

    }

    public interface OnClickListener{
        void onClick(UIDPorcessNode item);
    }
}