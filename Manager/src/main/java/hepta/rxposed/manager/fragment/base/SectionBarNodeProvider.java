package hepta.rxposed.manager.fragment.base;

import android.view.View;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;
import com.chad.library.adapter.base.provider.BaseNodeProvider;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;

import org.jetbrains.annotations.NotNull;

import java.util.List;

import hepta.rxposed.manager.R;

public class SectionBarNodeProvider<T extends SectionBarNodeProvider.baseSectionNode> extends BaseNodeProvider {


    public static class baseSectionNode extends BaseExpandNode {


        private List<BaseNode> childNode;
        private String title;

        public baseSectionNode(List<BaseNode> childNode, String title) {
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



    @Override
    public int getItemViewType() {
        return 0;
    }

    @Override
    public int getLayoutId() {
        return R.layout.item_section_bar;
    }

    @Override
    public void convert(@NotNull BaseViewHolder baseViewHolder, @NotNull BaseNode baseNode) {
        // 数据类型需要自己强转
        T entity = (T) baseNode;
        baseViewHolder.setText(R.id.tv_name, entity.getTitle());
    }

    @Override
    public void onClick(@NotNull BaseViewHolder helper, @NotNull View view, BaseNode data, int position) {
        getAdapter().expandOrCollapse(position);
    }
}
