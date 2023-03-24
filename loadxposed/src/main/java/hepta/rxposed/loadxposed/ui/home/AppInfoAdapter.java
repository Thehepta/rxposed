package hepta.rxposed.loadxposed.ui.home;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseNodeAdapter;
import com.chad.library.adapter.base.entity.node.BaseNode;

import org.jetbrains.annotations.NotNull;

import java.util.Collection;
import java.util.List;


public class AppInfoAdapter extends BaseNodeAdapter {

    public AppInfoAdapter() {
        super();
        // 注册Provider，总共有如下三种方式
        // 需要占满一行的，使用此方法（例如section）
        // 普通的item provider
        addNodeProvider(new AppInfoNodeProvider(ExtenDataProvider.getInstance()));
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
        if (node instanceof AppInfoNode) {
            return 1;
        }
        return -1;
    }
}