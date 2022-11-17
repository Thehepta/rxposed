package hepta.rxposed.manager.fragment.apps;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

/**
 * 第二个节点SecondNode，里面没有子节点了
 */
public class SecondNode extends BaseNode {

    private String title;

    public SecondNode(String title) {
        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    /**
     * 重写此方法，返回子节点
     */
    @Nullable
    @Override
    public List<BaseNode> getChildNode() {
        return null;
    }
}