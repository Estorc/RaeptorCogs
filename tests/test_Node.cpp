#include <gtest/gtest.h>
#include <RaeptorCogs/Node.hpp>

namespace RaeptorCogs {

// Test classes
class TestNode : public RegisterNode<TestNode, Node> {};
class DerivedTestNode : public RegisterNode<DerivedTestNode, TestNode> {};

} // namespace RaeptorCogs

using namespace RaeptorCogs;

TEST(NodeTest, BasicNodeCreation) {
    Node* root = new Node();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getParent(), nullptr);
    EXPECT_TRUE(root->getChildren().empty());
    delete root;
}

TEST(NodeTest, AddChild) {
    Node* root = new Node();
    Node* child = new TestNode();
    
    root->addChild(child);
    
    EXPECT_EQ(child->getParent(), root);
    EXPECT_EQ(root->getChildren().size(), 1);
    EXPECT_EQ(root->getChildren()[0], child);
    
    delete root;
    delete child;
}

TEST(NodeTest, IsInstanceOf) {
    Node* root = new Node();
    Node* child = new TestNode();
    
    EXPECT_TRUE(child->isInstanceOf<TestNode>());
    EXPECT_TRUE(child->isInstanceOf<Node>());
    EXPECT_FALSE(root->isInstanceOf<TestNode>());
    
    delete root;
    delete child;
}

TEST(NodeTest, MultipleChildren) {
    Node* root = new Node();
    Node* child1 = new TestNode();
    Node* child2 = new TestNode();
    
    root->addChild(child1);
    root->addChild(child2);
    
    EXPECT_EQ(root->getChildren().size(), 2);
    
    delete root;
    delete child1;
    delete child2;
}

TEST(NodeTest, RemoveChild) {
    Node* root = new Node();
    Node* child = new TestNode();
    
    root->addChild(child);
    root->removeChild(child);
    
    EXPECT_EQ(child->getParent(), nullptr);
    EXPECT_EQ(root->getChildren().size(), 0);
    
    delete root;
    delete child;
}

TEST(NodeTest, DerivedNodeHierarchy) {
    Node* root = new TestNode();
    DerivedTestNode* derived = new DerivedTestNode();
    
    root->addChild(derived);
    
    EXPECT_TRUE(derived->isInstanceOf<DerivedTestNode>());
    EXPECT_TRUE(derived->isInstanceOf<TestNode>());
    EXPECT_TRUE(derived->isInstanceOf<Node>());
    
    delete root;
    delete derived;
}