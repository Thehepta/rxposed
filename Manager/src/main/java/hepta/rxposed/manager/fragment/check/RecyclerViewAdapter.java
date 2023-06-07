package hepta.rxposed.manager.fragment.check;


import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import java.util.List;

import hepta.rxposed.manager.R;

public class RecyclerViewAdapter extends BaseQuickAdapter<ItemBean, BaseViewHolder> {

    public RecyclerViewAdapter(int layoutResId, @Nullable List<ItemBean> data){
        super(layoutResId, data);
    }
    @Override
    protected void convert(BaseViewHolder helper, ItemBean item) {
        helper.setText(R.id.text1, item.getMsg());
        if(item.status){
            helper.setBackgroundResource(R.id.status,R.drawable.check_ok);
        }else {
            helper.setBackgroundResource(R.id.status,R.drawable.check_error);
        }
    }
}

