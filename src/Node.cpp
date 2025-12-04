#include <RaeptorCogs/Node.hpp>

namespace RaeptorCogs {

Node::~Node() {}

void Node::addChild(Node* child) {
    children.push_back(child);
    child->setParent(this);
}

void Node::removeChild(Node* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
        child->parent = nullptr;
    }
}

void Node::setParent(Node* parent) {
    this->parent = parent;
}

const std::vector<Node*>& Node::getChildren() const {
    return children;
}

Node* Node::getParent() const {
    return this->parent;
}

}