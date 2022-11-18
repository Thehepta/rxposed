package hepta.rxposed.manager.fragment.apps;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseNodeAdapter;
import com.chad.library.adapter.base.entity.node.BaseNode;

import org.jetbrains.annotations.NotNull;

import java.util.Collection;
import java.util.List;


import hepta.rxposed.manager.fragment.base.AppModule;
import hepta.rxposed.manager.fragment.base.SectionBarNode;
import hepta.rxposed.manager.fragment.base.SectionBarNodeProvider;
import hepta.rxposed.manager.fragment.PlugSupport.FrameData;

import hepta.rxposed.manager.fragment.PlugSupport.FrameNodeProvider;
import hepta.rxposed.manager.fragment.PlugExtend.ExtendNodeProvider;
import hepta.rxposed.manager.fragment.PlugExtend.ModuleData;

public class AppInfoAdapter extends BaseNodeAdapter {

    public  AppInfoAdapter() {
        super();
        // 注册Provider，总共有如下三种方式
        // 需要占满一行的，使用此方法（例如section）
        addFullSpanNodeProvider(new SectionBarNodeProvider<SectionBarNode>());
        // 普通的item provider
        addNodeProvider(new AppInfoNodeProvider());
        addNodeProvider(new ExtendNodeProvider());
        addNodeProvider(new FrameNodeProvider());

    }

    public AppInfoAdapter(AppModule appModule) {
        this();
    }

        @Override
    public void setList(@Nullable Collection<? extends BaseNode> list) {
        super.setList(list);
    }

    /**
     * 自行根据数据、位置等信息，返回 item 类型
     */
    @Override
    protected int getItemType(@NotNull List<? extends BaseNode> data, int position) {
        BaseNode node = data.get(position);
        if (node instanceof SectionBarNode) {
            return 0;
        } else if (node instanceof AppInfoNode) {
            return 1;
        }else if (node instanceof ModuleData.Modules) {
            return 2;
        }else if (node instanceof FrameData.Frames) {
            return 3;
        }
        return -1;
    }
}