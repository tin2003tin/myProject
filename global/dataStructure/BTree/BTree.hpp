#ifndef BTREE_HPP
#define BTREE_HPP

#include "BTreeNode.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <string>

namespace tin
{

    class BTree
    {
        std::shared_ptr<BTreeNode> root;
        int t;

    public:
        BTree(int _t)
        {
            root = nullptr;
            t = _t;
        }

        void traverse()
        {
            if (root != nullptr)
                root->traverse();
        }

        void bfs()
        {
            if (root != nullptr)
                root->bfs();
        }

        std::shared_ptr<BTreeNode> search(const std::string &k)
        {
            return (root == nullptr) ? nullptr : root->search(k);
        }

        void insert(const std::string &k, const std::string &value);

        void remove(const std::string &k);
    };

} // namespace tin
#include "BTree.cpp"
#endif
