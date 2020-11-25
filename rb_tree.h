#pragma once

#include "base.h"

template <typename kT, typename vT>
struct RBNode
{
    kT key;
    vT value;

    RBNode<kT, vT> *parent;

    RBNode<kT, vT> *left;
    RBNode<kT, vT> *right;

    enum Color
    {
        RED,
        BLACK
    };

    Color color;

    RBNode(const std::pair<kT, vT> &item, RBNode *_parent):
        key(item.first),
        value(item.second),
        parent(_parent),
        left(nullptr),
        right(nullptr),
        color(RED)
    {}

    RBNode(const RBNode<kT, vT> &other):
        key(other.key),
        value(other.value),
        parent(nullptr),
        left(nullptr),
        right(nullptr),
        color(other.color)
    {}

    RBNode() = delete;
    RBNode(RBNode<kT, vT> &&) = delete;

    // utils for tree navigation

    RBNode<kT, vT> *grandparent() const
    {
        return this->parent == nullptr ? nullptr : this->parent->parent;
    }

    RBNode<kT, vT> *sibling() const
    {
        if (this->parent == nullptr)
        {
            return nullptr;
        }
        return this->parent->left == this ? this->parent->right : this->parent->left;
    }

    RBNode<kT, vT> *uncle() const
    {
        return this->parent == nullptr ? nullptr : this->parent->sibling();
    }

    void rotate_left()
    {
        LOGV("->");
        RBNode<kT, vT> *pivot = this->right;

        if (pivot == nullptr)
        {
            throw "what the fuck";
        }

        pivot->parent = this->parent;
        if (this->parent != nullptr)
        {
            if (this->parent->left == this)
            {
                this->parent->left = pivot;
            }
            else
            {
                this->parent->right = pivot;
            }
        }

        LOGV("set parent");

        this->right = pivot->left;
        if (pivot->left != nullptr)
        {
            pivot->left->parent = this;
        }

        this->parent = pivot;
        pivot->left= this;
        LOGV("<-");
    }

    void rotate_right()
    {
        LOGV("->");
        RBNode<kT, vT> *pivot = this->left;

        pivot->parent = this->parent;
        if (this->parent != nullptr)
        {
            if (this->parent->right == this)
            {
                this->parent->right = pivot;
            }
            else
            {
                this->parent->left = pivot;
            }
        }

        this->left = pivot->right;
        if (pivot->right != nullptr)
        {
            pivot->right->parent = this;
        }

        this->parent = pivot;
        pivot->right= this;
        LOGV("<-");
    }

    void adjust_insert()
    {
        if (this->parent == nullptr)
        {
            this->color = BLACK;
            LOGV("Case 1 fired");
            return;
        }

        if (this->parent->color == BLACK)
        {
            LOGV("Case 2 fired");
            return;
        }

        if (this->uncle() != nullptr && this->uncle()->color == RED)
        {
            this->parent->color = BLACK;
            this->uncle()->color = BLACK;
            this->grandparent()->color = RED;
            this->grandparent()->adjust_insert();
            LOGV("Case 3 fired");
            return;
        }

        // n is needed because last case might not be applied to this
        RBNode<kT, vT> *n;

        if (
            this->parent->right == this &&
            this->parent == this->grandparent()->left
        )
        {
            this->parent->rotate_left();
            n = this->left;
            LOGV("n = this->left");
        }
        else if (
            this->parent->left == this &&
            this->parent == this->grandparent()->right
        )
        {
            this->parent->rotate_right();
            n = this->right;
            LOGV("n = this->right");
        }
        else
        {
            n = this;
            LOGV("n = this");
        }

        n->parent->color = BLACK;
        n->grandparent()->color = RED;
        if (n == n->parent->left && n->parent == n->grandparent()->left)
        {
            n->grandparent()->rotate_right();
        }
        else if (n == n->parent->right && n->parent == n->grandparent()->right)
        {
            n->grandparent()->rotate_left();
        }
        else
        {
            LOGV(">>> SOMETHING WEIRD <<<");
        }
    }

    // replace_with modifies all connections in the tree
    // involving this to involve replacement instead;
    // it does nothing to connections involving replacement,
    // so this method should be used carefully to prevent data loss/
    // memory leaks/appearance of cycles in the tree
    void replace_with(RBNode<kT, vT> *replacement)
    {
        if (replacement != nullptr)
        {
            replacement->parent == this->parent;
        }

        if (this->parent != nullptr)
        {
            if (this == this->parent->left)
            {
                this->parent->left = replacement;
            }
            else
            {
                this->parent->right = replacement;
            }
        }
    }

    // this function performs specific for RBTree adjustments
    // required to delete the node;
    // it has to be a separate function for easier implementation
    void rb_adjust_delete()
    {
        // case 1
        if (this->parent == nullptr)
        {
            LOGV("Case 1 fired");
            return;
        }

        auto s = this->sibling();

        // case 2
        if (s->color == RED)
        {
            LOGV("Case 2 fired (falling)");
            this->parent->color = RED;
            s->color = BLACK;
            if (this == this->parent->left)
            {
                this->parent->rotate_left();
            }
            else
            {
                this->parent->rotate_right();
            }
            // fallthrough after case 2
        }

        s = this->sibling();

        LOGV("past case 2");

        // case 3
        if (
            this->parent->color == BLACK &&
            s->color == BLACK &&
            (s->left == nullptr || s->left->color == BLACK) &&
            (s->right == nullptr || s->right->color == BLACK)
        )
        {
            LOGV("Case 3 fired");
            s->color = RED;
            this->parent->rb_adjust_delete();
            return;
        }

        LOGV("past case 3");

        LOGV(IS_NULL(this->parent));
        LOGV(IS_NULL(s));

        // case 4
        if (
            this->parent->color == RED &&
            s->color == BLACK &&
            (s->left == nullptr || s->left->color == BLACK) &&
            (s->right == nullptr || s->right->color == BLACK)
        )
        {
            LOGV("Case 4 fired");
            s->color = RED;
            this->parent->color = BLACK;
            return;
        }

        LOGV("past case 4");

        // case 5
        if (s->color == BLACK)
        {
            LOGV("Case 5 fired (falling)");
            if (
                this == this->parent->left &&
                (s->right == nullptr || s->right->color == BLACK) &&
                (s->left != nullptr && s->left->color == RED)
            )
            {
                s->color = RED;
                s->left->color = BLACK;
                s->rotate_right();
            }
            else if (
                this == this->parent->right &&
                (s->left == nullptr || s->left->color == BLACK) &&
                (s->right != nullptr && s->right->color) == RED
            )
            {
                s->color = RED;
                s->right->color = BLACK;
                s->rotate_left();
            }
            // fallthrough after case 5
        }

        s = this->sibling();

        // case 6
        LOGV("Case 6 fired");
        s->color = this->parent->color;
        this->parent->color = BLACK;

        if (this == this->parent->left)
        {
            s->right->color = BLACK;
            this->parent->rotate_left();
        }
        else
        {
            s->left->color = BLACK;
            this->parent->rotate_right();
        }
    }

    void adjust_delete()
    {

        if (this->left != nullptr && this->right != nullptr)
        {
            // if this has both children, we take rightmost node from left
            // subtree as replacement, adjust it and replace this with it
            RBNode<kT, vT> *replacement;
            for (
                replacement = this->left;
                replacement->right != nullptr;
                replacement = replacement->right
            );

            replacement->adjust_delete();

            this->replace_with(replacement);
            return;
        }
        
        // now this has at most 1 non-null child

        // the only child:
        RBNode<kT, vT> *child = this->left == nullptr ? this->right : this->left;

        if (child == nullptr)
        {
            if (this->color == BLACK)
            {
                this->rb_adjust_delete();
            }
            this->replace_with(nullptr);
            return;
        }

        this->replace_with(child);

        if (this->color == BLACK)
        {
            if (child->color == RED)
            {
                child->color = BLACK;
            }
            else
            {
                child->rb_adjust_delete();
            }
        }
    }

#if defined _TREE_DEBUG && _TREE_DEBUG > 0

    bool is_valid() const
    {
        if (this->parent == nullptr)
        {
            if (this->color == RED)
            {
                LOGV("invalid rbtree: red root");
                return false;
            }
            if (this->black_depth() == -1)
            {
                LOGV("invalid rbtree: black depth mismatch");
                return false;
            }
        }

        if (
            this->color == RED &&
            (
                this->left != nullptr && this->left->color == RED ||
                this->right != nullptr && this->right->color == RED
            )
        )
        {
            LOGV("invalid rbtree: red node has red children");
            return false;
        }

        if (
            this->left != nullptr && !this->left->is_valid() ||
            this->right != nullptr && !this->right->is_valid()
        )
        {
            return false;
        }

        return true;
    }

    int black_depth() const
    {
        LOGV("->");
        int ld, rd;
        ld = this->left == nullptr ? 1 : this->left->black_depth();
        rd = this->right == nullptr ? 1 : this->right->black_depth();
        if (ld == -1 || rd == -1 || ld != rd)
        {
            if (ld * rd > 0 && ld != rd)
            {
                LOGV("left (" << ld << ") != right (" << rd << ")");
            }
            LOGV("<-");
            return -1;
        }
        LOGV("<-");
        return ld + (this->color == BLACK ? 1 : 0);
    }

    void print() const
    {
        std::cout << this->key << " : " << this->value <<
            (this->color == RED ? " [red]" : " [black]") << std::endl;
    }

#endif

};

template <typename kT, typename vT>
using RBTree = BaseTree<kT, vT, RBNode<kT, vT>>;
