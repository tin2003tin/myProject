#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <string>
namespace tin
{

    class BTreeNode : public std::enable_shared_from_this<BTreeNode>
    {
        std::vector<std::string> keys;
        std::vector<std::string> values;
        std::vector<std::shared_ptr<BTreeNode>> children;
        int t;
        int n;
        bool leaf;

    public:
        BTreeNode(int _t, bool _leaf);

        void traverse();
        void bfs();

        std::shared_ptr<BTreeNode> search(const std::string &k);

        void insertNonFull(const std::string &k, const std::string &value);

        void splitChild(int i, std::shared_ptr<BTreeNode> y);

        void remove(const std::string &k);

        int findKey(const std::string &k);

        void removeFromLeaf(int idx);

        void removeFromNonLeaf(int idx);

        std::string getPred(int idx);

        std::string getSucc(int idx);

        void fill(int idx);

        void borrowFromPrev(int idx);

        void borrowFromNext(int idx);

        void merge(int idx);

        friend class BTree;
    };
}
#include "BTreeNode.cpp"
#endif
