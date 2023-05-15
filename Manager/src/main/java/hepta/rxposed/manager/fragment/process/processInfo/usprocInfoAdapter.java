package hepta.rxposed.manager.fragment.process.processInfo;

import android.view.View;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.module.LoadMoreModule;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.process.UIDPorcessNode;

public class usprocInfoAdapter  extends BaseQuickAdapter<UIDPorcessNode.subprocess, BaseViewHolder> implements LoadMoreModule {


    private OnClickInjectListener onClickInjectListener = null;

    public usprocInfoAdapter(int layoutResId) {
        super(layoutResId);
    }


    public void setOnClickInjectListener(OnClickInjectListener onClickInjectListener){
        this.onClickInjectListener = onClickInjectListener;
    }


    @Override
    protected void convert(@NonNull BaseViewHolder baseViewHolder, UIDPorcessNode.subprocess subprocess) {
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


    public interface OnClickInjectListener {
        void onClick(UIDPorcessNode.subprocess item);
    }
}