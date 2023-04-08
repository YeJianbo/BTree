#include <bits/stdc++.h>

using namespace std;

//
const int MAX_CHILDREN = 3;
const int MIN_CHILDREN = 2;

// B树节点
class Node {
    bool isLeaf; // 当前节点是否是叶子节点
    vector<int> keys; // 节点中的关键字数组
    vector<Node*> children; // 节点的孩子节点集合
public:
    Node(bool isLeaf){
        this->isLeaf = isLeaf;
    }

    //遍历Node
    void traverse();

    //查找关键字n是否在该节点的子树上
    Node* search(int n);

    friend class BTree;
};

//B树
class BTree {
private:
    //根节点
    Node *root;
public:
    //B树的构造函数，用于初始化一个空树
    BTree() {
        root = NULL;
    }

    // 插入关键字:
    // 如果根节点为空，直接初始化根节点返回
    // 从根节点开始向下查找，如果节点全满，则分裂节点，取中间节点变为父节点
    void insert(int key);
    //删除关键字
    void remove(int key);
    //从节点中查找关键字的位置
    int findKey(Node *node, int key);
    //插入关键字到非满节点
    void insertNonFull(Node *node, int key);
    //分裂节点
    void split(Node *parent, int index, Node *node);
    //从节点中移除关键字
    void remove(Node *node, int key);
    //打印B树
    void print();
    //打印节点
    void printNode(Node *node);
};
void BTree::printNode(Node *node) {
    cout << "[ ";
    for (int i = 0; i < node->keys.size(); i++) {
        cout << node->keys[i] << " ";
    }
    cout << "]";

    if (!node->isLeaf) {
        cout << " {";
        for (int i = 0; i < node->children.size(); i++) {
            printNode(node->children[i]);
            if (i < node->children.size() - 1) {
                cout << ", ";
            }
        }
        cout << "}";
    }
}

void BTree::print() {
    printNode(root);
}

void BTree::remove(Node *node, int key) {
    //找到关键字的位置
    int i = findKey(node, key);
    //如果存在该关键字
    if (i < node->keys.size() && node->keys[i] == key) {
        //如果是叶子节点，直接删除
        if (node->isLeaf) {
            node->keys.erase(node->keys.begin() + i);
        } else {
            //否则，寻找前驱节点，前驱节点关键字下移，删除当前关键字
            Node *predecessor = node->children[i];
            while (!predecessor->isLeaf) {
                predecessor = predecessor->children[predecessor->children.size() - 1];
            }
            node->keys[i] = predecessor->keys[predecessor->keys.size() - 1];
            remove(predecessor, node->keys[i]);
        }
    } else {
        //不是叶子节点，递归调用
        Node *child = node->children[i];
        if (child->keys.size() == MIN_CHILDREN) {
            //子节点关键字太少，需要合并
            if (i > 0 && node->children[i - 1]->keys.size() > MIN_CHILDREN) {
                //从左边借关键字
                Node *leftSibling = node->children[i - 1];
                child->keys.insert(child->keys.begin(), node->keys[i - 1]);
                node->keys[i - 1] = leftSibling->keys[leftSibling->keys.size() - 1];
                leftSibling->keys.erase(leftSibling->keys.end() - 1);
                if (!child->isLeaf) {
                    child->children.insert(child->children.begin(),
                                           leftSibling->children[leftSibling->children.size() - 1]);
                    leftSibling->children.erase(leftSibling->children.end() - 1);
                }
            } else if (i < node->children.size() - 1 && node->children[i + 1]->keys.size() > MIN_CHILDREN) {
                // 从右边借关键字
                Node *rightSibling = node->children[i + 1];
                child->keys.push_back(node->keys[i]);
                node->keys[i] = rightSibling->keys[0];
                rightSibling->keys.erase(rightSibling->keys.begin());
                if (!child->isLeaf) {
                    child->children.push_back(rightSibling->children[0]);
                    rightSibling->children.erase(rightSibling->children.begin());
                }
            } else {
                //合并子节点
                if (i > 0) {
                    Node *leftSibling = node->children[i - 1];
                    leftSibling->keys.push_back(node->keys[i - 1]);
                    leftSibling->keys.insert(leftSibling->keys.end(), child->keys.begin(), child->keys.end());
                    if (!child->isLeaf) {
                        leftSibling->children.insert(leftSibling->children.end(), child->children.begin(),
                                                     child->children.end());
                    }
                    node->keys.erase(node->keys.begin() + i - 1);
                    node->children.erase(node->children.begin() + i);
                    delete child;
                } else {
                    Node *rightSibling = node->children[i + 1];
                    child->keys.push_back(node->keys[i]);
                    child->keys.insert(child->keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
                    if (!child->isLeaf) {
                        child->children.insert(child->children.end(), rightSibling->children.begin(),
                                               rightSibling->children.end());
                    }
                    node->keys.erase(node->keys.begin() + i);
                    node->children.erase(node->children.begin() + i + 1);
                    delete rightSibling;
                }
            }
        } else
            remove(child, key);
    }
}

void BTree::split(Node *parent, int index, Node *node) {
    //创建新节点
    Node *newNode = new Node(node->isLeaf);
    //找到中间位置
    int mid = node->keys.size() / 2;
    int key = node->keys[mid];
    //中间位置右边的关键字移动到新节点中
    newNode->keys.insert(newNode->keys.begin(), node->keys.begin() + mid + 1, node->keys.end());
    node->keys.erase(node->keys.begin() + mid, node->keys.end());
    //如果要分裂的节点不是叶子节点，将该节点右边的所有子节点都移动到新节点下
    if (!node->isLeaf) {
        newNode->children.insert(newNode->children.begin(), node->children.begin() + mid + 1, node->children.end());
        node->children.erase(node->children.begin() + mid + 1, node->children.end());
    }
    //将中间值以及分裂的两个新节点插入到父节点对应位置中
    parent->keys.insert(parent->keys.begin() + index, key);
    parent->children.insert(parent->children.begin() + index + 1, newNode);
}

void BTree::insert(int key) {
    //B树为空，新建根节点，返回
    if (root == NULL) {
        root = new Node(true);
        root->keys.push_back(key);
        return;
    }
    //根节点已满，新建一个非叶子节点作为新的根节点，分裂原来的根节点
    if (root->keys.size() == MAX_CHILDREN) {
        Node *newRoot = new Node(false);
        newRoot->children.push_back(root);
        split(newRoot, 0, root);
        root = newRoot;
    }
    //根节点未满，直接插入到根节点中
    insertNonFull(root, key);
}

void BTree::remove(int key) {
    if (root == NULL) {
        return;
    }
    remove(root, key);
    if (root->keys.size() == 0) {
        Node *oldRoot = root;
        root = root->children[0];
        delete oldRoot;
    }
}

int BTree::findKey(Node *node, int key) {
    int index = 0;
    while (index < node->keys.size() && node->keys[index] < key) {
        ++index;
    }
    return index;
}

void BTree::insertNonFull(Node *node, int key) {
    //找到要插入的位置
    int i = findKey(node, key);
    //如果是叶子节点，直接插入，否则找到应该插入的子节点
    if (node->isLeaf) {
        node->keys.insert(node->keys.begin() + i, key);
    } else {
        Node *child = node->children[i];
        //如果子节点满，分裂子节点
        if (child->keys.size() == MAX_CHILDREN) {
            split(node, i, child);
            if (key > node->keys[i]) {
                ++i;
            }
        }
        insertNonFull(node->children[i], key);
    }
}


int main() {
    BTree b;
    b.insert(8);
    b.insert(9);
    b.insert(6);
    b.insert(4);
    b.insert(29);
    b.insert(52);
    b.insert(5);
    b.insert(2);
    b.print();
    cout<<endl;
    b.remove(2);
    b.remove(6);
    b.print();
    return 0;
}