package hepta.rxposed.manager.fragment.check;

import android.util.Log;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

public class FirstNode extends BaseExpandNode {

    private List<BaseNode> childNode;
    private String title;

    public FirstNode( String title) {

        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    public void setStatus(boolean status){
        Log.e("rzx",title+" setStatus:"+status);
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