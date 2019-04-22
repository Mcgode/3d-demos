//
// Created by max on 22/04/19.
//

#include <common/HierarchicalNode.h>


HierarchicalNode::HierarchicalNode(glm::mat4 transform)
{
    this->transform = glm::mat4(transform);
}


HierarchicalNode::HierarchicalNode(BaseObject *object, glm::mat4 transform)
{
    this->objects.push_back(object);
    this->transform = glm::mat4(transform);
}


HierarchicalNode *HierarchicalNode::addObject(BaseObject *object)
{
    this->objects.push_back(object);
    return this;
}


HierarchicalNode *HierarchicalNode::addChild(HierarchicalNode *child)
{
    this->children.push_back(child);
    return this;
}


HierarchicalNode::~HierarchicalNode()
{
    for (BaseObject *obj: objects)
        delete(obj);
    for (HierarchicalNode *child: children)
        delete(child);
}


