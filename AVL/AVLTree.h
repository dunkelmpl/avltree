#pragma once

class AVLTree
{
private:
    typedef enum {
        BFS, DFS_PREORDER, DFS_INORDER, DFS_POSTORDER,
    } TraversalType;

    typedef enum {
        LEFTHEAVY = -1, BALANCED = 0, RIGHTHEAVY = 1,
    } BalanceFactor;

    typedef enum {
        LEFT, RIGHT
    } Direction;

    typedef struct TNode {
        TNode(int key) : key(key), left(nullptr), right(nullptr), height(1) {}
        virtual ~TNode()
        {
            if (left) {
                delete left;
            }

            if (right) {
                delete right;
            }
        }

        bool operator<(const TNode& another) {
            return this->key < another.key;
        }
        
        bool operator>(const TNode& another) {
            return this->key > another.key;
        }

        int key;
        int height;

        TNode* left;
        TNode* right;
    } Node;

public:
    AVLTree() : top(nullptr) {}
    virtual ~AVLTree();

public:
    void print(TraversalType type);
    void insertItem(int itemKey);
    void deleteItem(int itemKey);

public:
    inline bool empty() { return top == nullptr; }

protected:
    void printBFS();
    void printDFS(TraversalType type);
    void printDFS(TraversalType type, Node* node);

protected:
    void insertNode(Node* newNode, Node* parent, Node* grandParent);
    void deleteItem(int itemKey, Node* node, Node* parent);

protected:
    inline int getNodeHeight(Node* node) { return node ? node->height : 0; }
    void updateNodeHeight(Node* node);

protected:
    void balanceNode(Node* newNode, Node* parent, Node* grandParent, Direction weightDirection);
    void rotateLeft(Node* pivotNode, Node* parent, Node* grandParent);
    void rotateRight(Node* pivotNode, Node* parent, Node* grandParent);
    void fixGrandParentAfterRotation(Node* newParent, Node* oldParent, Node* grandParent);
    BalanceFactor getNodeBalanceFactor(Node* node);
    int getNodeBalance(Node* node);

private:
    void deleteNode(Node* node, Node* parent);
    void deleteLeaf(Node* leaf, Node* parent);
    void deleteNodeWithOneKid(Node* node, Node* parent);
    void deleteNodeWithBothKids(Node* node, Node* parent);
    Node* getInorderSuccessor(Node* baseNode, Node** inorderSuccessorParent);

private:
    Node* top;
};
