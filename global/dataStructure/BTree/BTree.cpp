#include "BTree.hpp"

namespace tin
{

    void BTree::insert(const std::string &k, const std::string &value)
    {
        if (root == nullptr)
        {
            root = std::make_shared<BTreeNode>(t, true);
            root->keys[0] = k;
            root->values[0] = value;
            root->n = 1;
        }
        else
        {
            if (root->n == 2 * t - 1)
            {
                std::shared_ptr<BTreeNode> s = std::make_shared<BTreeNode>(t, false);
                s->children[0] = root;
                s->splitChild(0, root);
                int i = 0;
                if (s->keys[0] < k)
                    i++;
                s->children[i]->insertNonFull(k, value);
                root = s;
            }
            else
            {
                root->insertNonFull(k, value);
            }
        }
    }

    void BTree::remove(const std::string &k)
    {
        if (!root)
        {
            std::cout << "The tree is empty\n";
            return;
        }

        root->remove(k);

        if (root->n == 0)
        {
            std::shared_ptr<BTreeNode> tmp = root;
            if (root->leaf)
                root = nullptr;
            else
                root = root->children[0];
        }
        return;
    }
}