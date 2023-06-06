package hepta.rxposed.manager.fragment.check;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

/**
 * 第二个节点SecondNode，里面没有子节点了
 */
public class SecondNode extends BaseNode {

    private String process_name;
    private String pid;

    public SecondNode(String pid,String process_name) {
        this.pid = pid;
        this.process_name = process_name;
    }

    public String getPid() {
        return pid;
    }
    public String getProcess_name() {
        return process_name;
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