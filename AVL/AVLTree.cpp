#include "AVLTree.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <queue>
#include <cstdlib>

using namespace std;

#ifndef max
#define max(x, y) (x > y ? x : y)
#endif

AVLTree::~AVLTree()
{
    if (top) {
        delete top;
    }
}

void AVLTree::print(TraversalType type)
{
    if (type != BFS) {
        printDFS(type);
    } else {
        printBFS();
    }
}

void AVLTree::printBFS()
{
    if (this->empty()) {
        return;
    }

    cout << "\nBFS traversal:";

    queue<Node*> queue;
    queue.push(top);

    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop();

        cout << ' ' << node->key;

        if (node->left) {
            queue.push(node->left);
        }

        if (node->right) {
            queue.push(node->right);
        }
    }
}

void AVLTree::printDFS(TraversalType type)
{
    if (this->empty()) {
        return;
    }

    cout << "\nDFS ";
    if (type == DFS_INORDER) {
        cout << "in-order ";
    } else if (type == DFS_PREORDER) {
        cout << "pre-order ";
    } else {
        cout << "post-order ";
    }

    cout << "traversal: ";

    this->printDFS(type, top);
}

void AVLTree::printDFS(TraversalType type, Node* node)
{
    if (!node) {
        return;
    }

    switch (type)
    {
        case DFS_INORDER:
            this->printDFS(type, node->left);
            cout << ' ' << node->key;
            this->printDFS(type, node->right);
            break;
        case DFS_PREORDER:
            cout << ' ' << node->key;
            this->printDFS(type, node->left);
            this->printDFS(type, node->right);
            break;
        case DFS_POSTORDER:
            this->printDFS(type, node->left);
            this->printDFS(type, node->right);
            cout << ' ' << node->key;
            break;
        default:
            throw invalid_argument("Unknown tree traversal type.");
    }
}

void AVLTree::insertItem(int itemKey)
{
    Node* newNode = new Node(itemKey);

    if (this->empty()) {
        top = newNode;

        return;
    }

    this->insertNode(newNode, top, nullptr);
}

void AVLTree::insertNode(Node* newNode, Node* parent, Node* grandParent)
{
    Direction childDirection;
    if (newNode < parent) {
        childDirection = LEFT;

        if (!parent->left) {
            parent->left = newNode;
        } else {
            this->insertNode(newNode, parent->left, parent);
        }
    } else {
        childDirection = RIGHT;

        if (!parent->right) {
            parent->right = newNode;
        } else {
            this->insertNode(newNode, parent->right, parent);
        }
    }

    this->updateNodeHeight(parent);

    this->balanceNode(newNode, parent, grandParent, childDirection);
}

void AVLTree::deleteItem(int itemKey)
{
    this->deleteItem(itemKey, top, nullptr);
}

void AVLTree::deleteItem(int itemKey, Node* node, Node* parent)
{
    if (node == nullptr) {
        stringstream s;
        s << "Failed to delete " << itemKey;
        throw invalid_argument(s.str());
    }

    if (node->key == itemKey) {
        this->deleteNode(node, parent);

        return;
    }

    Node* child;
    Direction childDirection;

    if (itemKey < node->key) {
        child = node->left;
        childDirection = LEFT;
    } else {
        child = node->right;
        childDirection = RIGHT;
    }

    this->deleteItem(itemKey, child, node);

    if (abs(this->getNodeBalance(node)) != 1) {
        this->balanceNode(child, node, parent, childDirection);
    }
}

void AVLTree::deleteNode(Node* node, Node* parent)
{
    if (node->left || node->right) { // at least one child
        if (node->left && node->right) { //two children
            this->deleteNodeWithBothKids(node, parent);
        } else { //one child
            this->deleteNodeWithOneKid(node, parent);
        }

    } else { //no children
        this->deleteLeaf(node, parent);
    }
}

void AVLTree::deleteLeaf(Node* leaf, Node* parent)
{
    if (parent) {
        if (parent->left == leaf) {
            parent->left = nullptr;
        } else {
            parent->right = nullptr;
        }
    } else {
        top = nullptr;
    }

    delete leaf;
}

void AVLTree::deleteNodeWithOneKid(Node* node, Node* parent)
{
    Node* childNode = node->left ? node->left : node->right;

    if (parent->left == node) {
        parent->left = childNode;
    } else {
        parent->right = childNode;
    }

    delete node;
}

void AVLTree::deleteNodeWithBothKids(Node* node, Node* parent)
{
    Node* inorderSuccessorParent = nullptr;
    Node* inorderSuccessor = this->getInorderSuccessor(node, &inorderSuccessorParent);

    node->key = inorderSuccessor->key;

    if (inorderSuccessorParent->left == inorderSuccessor) {
        inorderSuccessorParent->left = nullptr;
    } else {
        inorderSuccessorParent->right = nullptr;
    }

    delete inorderSuccessor;
}

AVLTree::Node* AVLTree::getInorderSuccessor(Node* baseNode, Node** inorderSuccessorParent)
{
    Node* inorderSuccessor = baseNode->right;
    *inorderSuccessorParent = baseNode;

    while (inorderSuccessor->left) {
        *inorderSuccessorParent = inorderSuccessor;
        inorderSuccessor = inorderSuccessor->left;
    }

    return inorderSuccessor;
}

void AVLTree::updateNodeHeight(Node* node)
{
    if (!node) {
        return;
    }

    node->height = 1 + max(
        this->getNodeHeight(node->left),
        this->getNodeHeight(node->right)
    );
}

void AVLTree::balanceNode(Node* newNode, Node* parent, Node* grandParent, Direction weightDirection)
{
    BalanceFactor balanceFactor = this->getNodeBalanceFactor(newNode);

    if (balanceFactor == BALANCED) {
        return;
    }

    if (balanceFactor == LEFTHEAVY) {
        if (weightDirection == RIGHT) {
            Node* child = newNode->right;
            this->rotateLeft(child, newNode, parent);
            this->rotateRight(child, parent, grandParent);
        } else {
            this->rotateRight(newNode, parent, grandParent);
        }
    } else { //balanceFactor == RIGHTHEAVY
        if (weightDirection == LEFT) {
            Node* child = newNode->left;
            this->rotateRight(child, newNode, parent);
            this->rotateLeft(child, parent, grandParent);
        } else {
            this->rotateLeft(newNode, parent, grandParent);
        }
    }
}

void AVLTree::rotateLeft(Node* pivotNode, Node* parent, Node* grandParent)
{
    Node* formerPivotLeft = pivotNode->left;
    pivotNode->left = parent;
    parent->right = formerPivotLeft;

    this->updateNodeHeight(parent);
    this->updateNodeHeight(pivotNode);

    this->fixGrandParentAfterRotation(pivotNode, parent, grandParent);
}

void AVLTree::rotateRight(Node* pivotNode, Node* parent, Node* grandParent)
{
    Node* formerPivotRight = pivotNode->right;
    pivotNode->right = parent;
    parent->left = formerPivotRight;

    this->updateNodeHeight(parent);
    this->updateNodeHeight(pivotNode);

    this->fixGrandParentAfterRotation(pivotNode, parent, grandParent);
}

void AVLTree::fixGrandParentAfterRotation(Node* newParent, Node* oldParent, Node* grandParent)
{
    if (grandParent->left == oldParent) {
        grandParent->left = newParent;
    } else {
        grandParent->right = newParent;
    }
}

int AVLTree::getNodeBalance(Node* node)
{
    return (node
        ? this->getNodeHeight(node->right) - this->getNodeHeight(node->left)
        : 0
    );
}

AVLTree::BalanceFactor AVLTree::getNodeBalanceFactor(Node* node)
{
    int balance = this->getNodeBalance(node);

    if (balance < -1) {
        return LEFTHEAVY;
    } else if (balance > 1) {
        return RIGHTHEAVY;
    }

    return BALANCED;
}