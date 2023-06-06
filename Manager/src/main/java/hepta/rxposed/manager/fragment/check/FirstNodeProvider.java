package hepta.rxposed.manager.fragment.check;

import android.util.Log;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.provider.BaseNodeProvider;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import hepta.rxposed.manager.R;

public class FirstNodeProvider extends BaseNodeProvider {

    public CompoundButton CurrentCheckBox = null;
    public FirstNode CurrentbaseNode = null;

    @Override
    public int getItemViewType() {
        return 2;
    }

    @Override
    public int getLayoutId() {
        return R.layout.item_radioframe;
    }

    @Override
    public void convert(@NonNull BaseViewHolder baseViewHolder, BaseNode baseNode) {
        FirstNode entity = (FirstNode) baseNode;
        baseViewHolder.setText(R.id.app_name, entity.getTitle());
        CheckBox checkBox = baseViewHolder.findView(R.id.radiobtn);

        checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Log.e("rzx","before status:"+buttonView.isChecked());
                entity.setStatus(isChecked);
                if(isChecked){
                    if(CurrentCheckBox == null){
                        CurrentCheckBox = buttonView;
                        CurrentbaseNode = entity;
                    }else if(CurrentCheckBox.equals(buttonView)){

                    }else {
                        CurrentCheckBox.setChecked(false);
                        CurrentbaseNode.setStatus(false);
                        CurrentCheckBox = buttonView;
                        CurrentbaseNode = entity;
                    }
                }
//                buttonView.isChecked();
                Log.e("rzx","after status:"+buttonView.isChecked());

            }
        });

    }
}


//            @Override
//            public void onClick(View v) {
//                Log.e("rzx","current status:"+checkBox.isChecked());
////                CheckBox radioButton1 = (CheckBox) v;
////                Boolean checked = radioButton.isChecked();
////                radioButton.setChecked(checked);
//            }