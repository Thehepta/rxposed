package hepta.rxposed.manager.fragment.LoadExten;

import android.content.pm.ApplicationInfo;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseNodeAdapter;
import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import org.jetbrains.annotations.NotNull;

import java.util.Collection;
import java.util.List;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.LoadExten.ExtenInfoProvider;
import hepta.rxposed.manager.fragment.LoadModule.ModuleListAdapter;
import hepta.rxposed.manager.fragment.PlugInject.SupportInfoProvider;
import hepta.rxposed.manager.fragment.base.AppInfoNode;
import hepta.rxposed.manager.fragment.base.AppInfoNodeProvider;
import hepta.rxposed.manager.fragment.base.SectionBarNode;
import hepta.rxposed.manager.fragment.base.SectionBarNodeProvider;

public class AppInfoAdapter extends BaseQuickAdapter<ApplicationInfo, BaseViewHolder > {

    public  AppInfoAdapter(int layoutResId) {
        super(layoutResId);

    }
    protected void convert(@NonNull BaseViewHolder baseViewHolder,ApplicationInfo item) {
        if (item == null) {
            return;
        }

        baseViewHolder.setText(R.id.app_name, item.name);
        baseViewHolder.setText(R.id.app_packageName, item.packageName);
//        baseViewHolder.setImageDrawable(R.id.app_icon, item.getIcon());
    }
}