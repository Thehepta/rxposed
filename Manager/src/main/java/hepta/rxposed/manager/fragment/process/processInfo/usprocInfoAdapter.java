package hepta.rxposed.manager.fragment.process.processInfo;

import android.view.View;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.module.LoadMoreModule;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.process.UIDPorcessItem;

public class usprocInfoAdapter  extends BaseQuickAdapter<UIDPorcessItem.subprocessItem, BaseViewHolder> implements LoadMoreModule {


    private OnClickInjectListener onClickInjectListener = null;

    public usprocInfoAdapter(int layoutResId) {
        super(layoutResId);
    }


    public void setOnClickInjectListener(OnClickInjectListener onClickInjectListener){
        this.onClickInjectListener = onClickInjectListener;
    }


    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, UIDPorcessItem.subprocessItem subprocess) {
        baseViewHolder.setText(R.id.proacee_pid,subprocess.getPid());
        baseViewHolder.setText(R.id.process_name,subprocess.getProcessName());
        baseViewHolder.findView(R.id.injectButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(onClickInjectListener!=null){
                    onClickInjectListener.onClick(subprocess);
                }
            }
        });


    }


    @Override
    protected int getDefItemViewType(int position) {   //不写这个容易导致item混乱
        return position;
    }

    public interface OnClickInjectListener {
        void onClick(UIDPorcessItem.subprocessItem item);
    }
}