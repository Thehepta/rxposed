package hepta.rxposed.manager.fragment.apps;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseNodeAdapter;
import com.chad.library.adapter.base.entity.node.BaseNode;

import org.jetbrains.annotations.NotNull;

import java.util.Collection;
import java.util.List;

public class AppsAdapter extends BaseNodeAdapter {

    public AppsAdapter() {
        super();
        // 注册Provider，总共有如下三种方式

        // 需要占满一行的，使用此方法（例如section）
        addFullSpanNodeProvider(new RootNodeProvider());
        // 普通的item provider
        addNodeProvider(new SecondNodeProvider());
        // 脚布局的 provider
        addNodeProvider(new AppInfoNodeProvider());
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
        if (node instanceof RootNode) {
            return 0;
        } else if (node instanceof SecondNode) {
            return 1;
        } else if (node instanceof AppInfo) {
            return 2;
        }
        return -1;
    }
}