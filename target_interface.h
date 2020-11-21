#pragma once

#include <cstddef>

// An interface for key-value binary search tree.
template<typename kT, typename vT>
class KeyValueTree
{
public:

    // Find or insert a default value at specified key.
    // Returns a reference to that value.
    virtual vT& operator[](const kT &key) = 0;

    // Insert or replace a value at specified key.
    // Returns true if a new node has been created and false otherwise.
    virtual bool insert(const kT &key, const vT &value) = 0;

    // Lookup a specific key and populate val with a value if it is found.
    // Returns true if the key exists in the tree.
    virtual bool find(const kT &key, vT &dst) const = 0;

    // Lookup a specific key.
    // Returns true if the key exists in the tree.
    virtual bool contains(const kT &key) const = 0;

    // Get the number of elements stored in a tree.
    virtual std::size_t size() const = 0;

    // Lookup a specific key and delete the node if it is found.
    // Returns true if the key existed in the tree.
    virtual bool erase(const kT &key) = 0;

    // Remove all nodes.
    virtual void clear() = 0;

    // Get the depth of the tree.
    // For research purposes.
    virtual std::size_t depth() const = 0;
};