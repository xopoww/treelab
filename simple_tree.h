#pragma once

#include "base.h"

// An implementation of the Node (required for the BaseTree) that does not
// balance itself. Used for comparison and to practise implementing the Node.
template <typename kT, typename vT>
struct SimpleNode
{
    kT key;
    vT value;

    SimpleNode<kT, vT> *parent;

    SimpleNode<kT, vT> *left;
    SimpleNode<kT, vT> *right;

    SimpleNode(const std::pair<kT, vT> &item, SimpleNode *_parent):
        key(item.first),
        value(item.second),
        parent(_parent),
        left(nullptr),
        right(nullptr)
    {}

    SimpleNode(const SimpleNode<kT, vT> &other):
        key(other.key),
        value(other.value),
        parent(nullptr),
        left(nullptr),
        right(nullptr)
    {}

    SimpleNode() = delete;
    SimpleNode(SimpleNode<kT, vT> &&) = delete;

    void adjust_insert()
    {
        // in simple tree, nothing needs to be done after the insertion
        return;
    }

    void adjust_delete()
    {
        // in simple tree, before deleting a node we search for the replacement

        SimpleNode<kT, vT> *replacement;

        if (this->left != nullptr)
        {
            // replacement is a rightmost element in the left subtree
            for (
                replacement = this->left;
                replacement->right != nullptr;
                replacement = replacement->right
            );
        }
        else if (this->right != nullptr)
        {
            // this does not have left subtree - same logic for the right one
            for (
                replacement = this->right;
                replacement->left != nullptr;
                replacement = replacement->left
            );
        }
        else
        {
            // this is a leaf = replacement is not needed
            replacement = nullptr;
        }

        // if this has a parent - set its child (which now is this)
        // to be replacement
        if (this->parent != nullptr)
        {
            if (this->parent->left == this)
            {
                this->parent->left = replacement;
            }
            else
            {
                this->parent->right = replacement;
            }
        }

        if (replacement != nullptr)
        {
            replacement->adjust_delete();

            // set replacement's parent children to those of this
            replacement->parent = this->parent;
            replacement->left = this->left;
            replacement->right = this->right;
        }
    }
};

template <typename kT, typename vT>
using SimpleTree = BaseTree<kT, vT, SimpleNode<kT, vT>>;
