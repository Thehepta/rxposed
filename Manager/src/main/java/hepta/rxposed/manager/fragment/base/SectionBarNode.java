package hepta.rxposed.manager.fragment.apps;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseExpandNode;
import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.List;

import hepta.rxposed.manager.fragment.base.SectionBarProvider;

public class RootNode extends SectionBarProvider.baseSectionNode {



    public RootNode(List<BaseNode> childNode, String title) {
        super(childNode,title);

    }


}