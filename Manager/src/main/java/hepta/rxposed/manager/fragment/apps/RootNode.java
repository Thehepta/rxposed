package hepta.rxposed.manager.fragment.apps;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

public class RootNode extends BaseExpandNode {


    private List<BaseNode> childNode;
    private String title;

    public RootNode(List<BaseNode> childNode, String title) {
        // 默认不展开
//        setExpanded(false);
        this.childNode = childNode;
        this.title = title;
    }

    public String getTitle() {
        return title;
    }


    @Nullable
    @Override
    public List<BaseNode> getChildNode() {
        return childNode;
    }

}