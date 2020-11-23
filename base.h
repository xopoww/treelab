#pragma once

#include "common.h"
#include "target_interface.h"

#include <functional>
#include <queue>

// General implementation of the KeyValueTree interface. Except typenames kT
// and vT also requires a Node class as a template parameter. This class controls
// the behavior of the tree. It must:
//  - have fields key (of type kT) and value (of type vT);
//  - have fields parent, left and right (of type Node*, i.e. ptr to another
//    instance of the same class);
//  - have constructor with signature (std::pair<kT, vT>&, Node*), where first
//    argument is an item to be stored at this node and second is a ptr to the
//    parent node (children should be initialized with nullptr);
//  - have copy constructor (which should not copy pointer values but initialize
//    them with nullptr);
//  - have a method void adjust_insert() which is called on the Node right after
//    it is inserted into the tree (e.g. this method should implement balancing
//    in a self-balancing tree);
//  - have a method void adjust_delete() which is called on the Node that is
//    about to be deleted right before the deletion. This method should make
//    sure that after the deletion of the node the tree will still be valid and
//    there will be no memory leak. It also should implement balancing in a
//    self-balancing tree);
//  - [debug only] have a method bool is_valid() that tells if a tree is valid;
//    in debub mode, this method will be called after each call to insert_at / 
//    delete_at and, it returns false, an exception will be thrown;
//  - [debug only] have a method void print() to print the value of the node (
//    possibly with additional data).
//
// The SimpleNode class in "simple_tree.h" may serve as a basic example of how
// to implement Node class.
//
// It is also a good idea to provide template alias for BaseTree next to the Node
// implementation.
template <typename kT, typename vT, class Node>
class BaseTree : public KeyValueTree<kT,vT>
{
private:

    // Utility function used by copy ctor and assignment.
    Node *copy_node(Node *other, Node *parent);

    Node *root;
    std::size_t _size;

    using search_t = std::pair<Node*, char>;

    // Find a node for a specified key.
    // If key is present, the first return value is a ptr to the node with this 
    // key, and the second one is 0.
    // If key is not present, first value is a ptr to a node that the node with
    // this key would be a child of and the second one is -1 if the new node
    // would be a left child and 1 if it would be a right child.
    search_t search_by_key(const kT &key) const;

    // Insert a new node with item as a child of parent (right child if right is
    // true, left otherwise). The parent node must not have this child.
    // If parent is null, new node with null parent is created and assigned to
    // this->root.
    // Returns a ptr to created Node.
    Node *insert_at(
        Node *parent, const bool right, const std::pair<kT,vT> &item);
    
    // Utility overload. Should only be called if s.second != 0.
    Node *insert_at(search_t s, const std::pair<kT,vT> &item)
    {
        return this->insert_at(s.first, (s.second == 1), item);
    }

    // Delete a node (not null).
    void delete_at(Node* node);

    // Utility method for recusively getting the depth of the tree.
    std::size_t node_depth(Node *node) const;

    // Utility for crearing/printing the tree.
    // Traverses the tree in top-to-bottom left-to-right order and applies func
    // to every node visited.
    void traverse(std::function<void(Node*)> func);

public:

    BaseTree();

    BaseTree(const BaseTree<kT, vT, Node> &other);
    BaseTree<kT, vT, Node> &operator=(const BaseTree<kT, vT, Node> &other);

    BaseTree(BaseTree<kT, vT, Node> &&other);
    BaseTree<kT, vT, Node> &operator=(BaseTree<kT, vT, Node> &&other);

    ~BaseTree();

public:

    vT& operator[](const kT &key) override;

    bool insert(const kT &key, const vT &value) override;

    bool find(const kT &key, vT &dst) const override;

    bool contains(const kT &key) const override;

    std::size_t size() const override { return this->_size; };

    bool erase(const kT &key) override;

    void clear() override;

    std::size_t depth() const override { return this->node_depth(this->root); }

#if defined _DEBUG && _DEBUG > 0
    // node printing is defined in the node class to allow for printing extra
    // data
    void print() { this->traverse([](Node* n){ n->print(); }); };
#endif
};


// Constructors

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node>::BaseTree():
    root(nullptr),
    _size(0)
{
    LOG("Tree constructed (default).");
};

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node>::BaseTree(const BaseTree<kT, vT, Node> &other)
{
    this->root = this->copy_node(other.root, nullptr);
    this->_size = other._size;
    LOG("Tree constructed (copy).");
};

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node> &BaseTree<kT, vT, Node>::operator=(const BaseTree<kT, vT, Node> &other)
{
    this->clear();
    this->root = this->copy_node(other.root, nullptr);
    this->_size = other._size;
    LOG("Tree assigned (copy).");
    return *this;
}

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node>::BaseTree(BaseTree<kT, vT, Node> &&other)
{
    this->root = other.root;
    other.root = nullptr;
    this->_size = other._size;
    other._size = 0;
    LOG("Tree constructed (move).");
};

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node> &BaseTree<kT, vT, Node>::operator=(BaseTree<kT, vT, Node> &&other)
{
    this->clear();
    this->root = other.root;
    other.root = nullptr;
    this->_size = other._size;
    other._size = 0;
    LOG("Tree assigned (move).");
    return *this;
}

template <typename kT, typename vT, class Node>
BaseTree<kT, vT, Node>::~BaseTree()
{
    this->clear();
}


// Utils

template <typename kT, typename vT, class Node>
Node *BaseTree<kT, vT, Node>::copy_node(Node *other, Node *parent)
{
    if (other == nullptr)
    {
        LOGV("other is null");
        return nullptr;
    }
    LOGV("copying " << other->key);

    Node *copied = new Node(*other);
    LOG("[ MEMORY ] Created Node using new.");
    copied->parent = parent;
    LOGV("going left");
    copied->left = this->copy_node(other->left, copied);
    LOGV("going right");
    copied->right = this->copy_node(other->right, copied);

    return copied;
}

template <typename kT, typename vT, class Node>
typename BaseTree<kT, vT, Node>::search_t BaseTree<kT, vT, Node>::search_by_key(const kT &key) const
{
    LOGV("->");
    Node *current = this->root, *previous = nullptr;

    while (current != nullptr && current->key != key)
    {
        LOGV("going " << (key < current->key ? "left" : "right"));
        LOGV("search = " << key << ", current = " << current->key);
        previous = current;
        current = (key < current->key) ? current->left : current->right;
    }

    LOGV("result is " << IS_NULL(current));
    LOGV("<-");
    if (current == nullptr)
    {
        if (previous == nullptr)
        {
            return BaseTree<kT, vT, Node>::search_t(nullptr, 1);
        }
        return BaseTree<kT, vT, Node>::search_t(
            previous,
            (key < previous->key) ? -1 : 1
        );
    }

    return BaseTree<kT, vT, Node>::search_t(current, 0);
}

template <typename kT, typename vT, class Node>
Node* BaseTree<kT, vT, Node>::insert_at(Node *parent, bool right, const std::pair<kT, vT> &item)
{
    Node **dst;

    if (parent == nullptr)
    {
        dst = &this->root;
    }
    else
    {
        dst = &(right ? parent->right : parent->left);
    }

    *dst = new Node(item, parent);
    LOG("[ MEMORY ] Created Node using new.");
    this->_size++;
    (*dst)->adjust_insert();

    while(this->root->parent != nullptr)
    {
        this->root = this->root->parent;
    }

#if defined _DEBUG && _DEBUG > 0
    if (!this->root->is_valid())
    {
        LOG("!!! TREE IS INVALIDATED, TERMINATING !!!");
        throw "Invalid tree";
    }
#endif
    return *dst;
}

template <typename kT, typename vT, class Node>
void BaseTree<kT, vT, Node>::delete_at(Node *node)
{
    node->adjust_delete();
    delete node;
    this->_size--;
    LOG("[ MEMORY ] Node deleted.");

    while(this->root->parent != nullptr)
    {
        this->root = this->root->parent;
    }

#if defined _DEBUG && _DEBUG > 0
    if (!this->root->is_valid())
    {
        LOG("!!! TREE IS INVALIDATED, TERMINATING !!!");
        this->print();
        throw "Invalid tree";
    }
#endif
}

template <typename kT, typename vT, class Node>
std::size_t BaseTree<kT, vT, Node>::node_depth(Node *node) const
{
    if (node == nullptr)
    {
        return 0;
    }
    std::size_t left_d = this->node_depth(node->left), right_d = this->node_depth(node->right);
    return 1 + (left_d > right_d ? left_d : right_d);
}


// Main methods (of the kVTree interface)

template <typename kT, typename vT, class Node>
vT& BaseTree<kT, vT, Node>::operator[](const kT &key)
{
    BaseTree<kT, vT, Node>::search_t search = this->search_by_key(key);
    Node *node = search.first;

    if (search.second != 0)
    {
        LOGV("inserting default value");
        node = this->insert_at(search, { key, vT{} });
    }

    return node->value;
}

template <typename kT, typename vT, class Node>
bool BaseTree<kT, vT, Node>::insert(const kT &key, const vT &value)
{
    LOGV("->");
    BaseTree<kT, vT, Node>::search_t search = this->search_by_key(key);
    if (search.second == 0)
    {
        // key already exists - just replace the value
        search.first->value = value;
        LOGV("<-");
        return false;
    }
    this->insert_at(search, { key, value });
    LOGV("<-");
    return true;
}

template <typename kT, typename vT, class Node>
bool BaseTree<kT, vT, Node>::find(const kT &key, vT& dst) const
{
    BaseTree<kT, vT, Node>::search_t search = this->search_by_key(key);
    if (search.second != 0)
    {
        return false;
    }
    dst = search.first->value;
    return true;
}

template <typename kT, typename vT, class Node>
bool BaseTree<kT, vT, Node>::contains(const kT &key) const
{
    return this->search_by_key(key).second == 0;
}

template <typename kT, typename vT, class Node>
bool BaseTree<kT, vT, Node>::erase(const kT &key)
{
    BaseTree<kT, vT, Node>::search_t search = this->search_by_key(key);
    if (search.second == 0)
    {
        this->delete_at(search.first);
        return true;
    }
    return false;
}

template <typename kT, typename vT, class Node>
void BaseTree<kT, vT, Node>::clear()
{
    this->traverse([](Node *node)
    {
        delete node;
        LOG("[ MEMORY ] Deleted Node.");
    });
    this->root = nullptr;
    this->_size = 0;
}

template <typename kT, typename vT, class Node>
void BaseTree<kT, vT, Node>::traverse(std::function<void(Node*)> func)
{
    if (this->root == nullptr)
    {
        return;
    }

    std::queue<Node*> queue;
    queue.push(this->root);

    while (queue.size() != 0)
    {
        Node* current = queue.front();
        queue.pop();
        if (current->left)
        {
            queue.push(current->left);
        }
        if (current->right)
        {
            queue.push(current->right);
        }
        func(current);
    }
}

