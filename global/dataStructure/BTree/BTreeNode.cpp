#include "BTreeNode.hpp"

namespace tin
{

    BTreeNode::BTreeNode(int t1, bool leaf1)
    {
        t = t1;
        leaf = leaf1;
        keys.resize(2 * t - 1);
        values.resize(2 * t - 1);
        children.resize(2 * t);
        n = 0;
    }

    void BTreeNode::traverse()
    {
        int i;
        for (i = 0; i < n; i++)
        {
            if (!leaf)
                children[i]->traverse();
            std::cout << " " << keys[i] << ":" << values[i];
        }

        if (!leaf)
            children[i]->traverse();
    }

    void BTreeNode::bfs()
    {
        std::queue<std::pair<std::shared_ptr<BTreeNode>, int>> q;
        q.push({shared_from_this(), 0});
        int currentLevel = 0;

        while (!q.empty())
        {
            std::shared_ptr<BTreeNode> node = q.front().first;
            int level = q.front().second;
            q.pop();

            if (level != currentLevel)
            {
                std::cout << std::endl;
                currentLevel = level;
            }

            std::cout << "[";
            for (int i = 0; i < node->n; i++)
            {
                std::cout << " " << node->keys[i] << ":" << node->values[i];
            }
            std::cout << " ]";

            if (!node->leaf)
            {
                for (int i = 0; i <= node->n; i++)
                {
                    q.push({node->children[i], level + 1});
                }
            }
        }
        std::cout << std::endl;
    }

    std::shared_ptr<BTreeNode> BTreeNode::search(const std::string &k)
    {
        int i = 0;
        while (i < n && k > keys[i])
            i++;

        if (keys[i] == k)
            return shared_from_this();

        if (leaf)
            return nullptr;

        return children[i]->search(k);
    }

    void BTreeNode::insertNonFull(const std::string &k, const std::string &value)
    {
        int i = n - 1;

        if (leaf)
        {
            while (i >= 0 && keys[i] > k)
            {
                keys[i + 1] = keys[i];
                values[i + 1] = values[i];
                i--;
            }

            keys[i + 1] = k;
            values[i + 1] = value;
            n = n + 1;
        }
        else
        {
            while (i >= 0 && keys[i] > k)
                i--;

            if (children[i + 1]->n == 2 * t - 1)
            {
                splitChild(i + 1, children[i + 1]);

                if (keys[i + 1] < k)
                    i++;
            }
            children[i + 1]->insertNonFull(k, value);
        }
    }

    void BTreeNode::splitChild(int i, std::shared_ptr<BTreeNode> y)
    {
        std::shared_ptr<BTreeNode> z = std::make_shared<BTreeNode>(y->t, y->leaf);
        z->n = t - 1;

        for (int j = 0; j < t - 1; j++)
        {
            z->keys[j] = y->keys[j + t];
            z->values[j] = y->values[j + t];
        }

        if (!y->leaf)
        {
            for (int j = 0; j < t; j++)
                z->children[j] = y->children[j + t];
        }

        y->n = t - 1;

        for (int j = n; j >= i + 1; j--)
            children[j + 1] = children[j];

        children[i + 1] = z;

        for (int j = n - 1; j >= i; j--)
        {
            keys[j + 1] = keys[j];
            values[j + 1] = values[j];
        }

        keys[i] = y->keys[t - 1];
        values[i] = y->values[t - 1];

        n = n + 1;
    }

    void BTreeNode::remove(const std::string &k)
    {
        int idx = findKey(k);

        if (idx < n && keys[idx] == k)
        {
            if (leaf)
                removeFromLeaf(idx);
            else
                removeFromNonLeaf(idx);
        }
        else
        {
            if (leaf)
            {
                std::cout << "The key " << k << " does not exist in the tree\n";
                return;
            }

            bool flag = ((idx == n) ? true : false);

            if (children[idx]->n < t)
                fill(idx);

            if (flag && idx > n)
                children[idx - 1]->remove(k);
            else
                children[idx]->remove(k);
        }
        return;
    }

    int BTreeNode::findKey(const std::string &k)
    {
        int idx = 0;
        while (idx < n && keys[idx] < k)
            ++idx;
        return idx;
    }

    void BTreeNode::removeFromLeaf(int idx)
    {
        for (int i = idx + 1; i < n; ++i)
        {
            keys[i - 1] = keys[i];
            values[i - 1] = values[i];
        }
        n--;
        return;
    }

    void BTreeNode::removeFromNonLeaf(int idx)
    {
        std::string k = keys[idx];

        if (children[idx]->n >= t)
        {
            std::string pred = getPred(idx);
            keys[idx] = pred;
            children[idx]->remove(pred);
        }
        else if (children[idx + 1]->n >= t)
        {
            std::string succ = getSucc(idx);
            keys[idx] = succ;
            children[idx + 1]->remove(succ);
        }
        else
        {
            merge(idx);
            children[idx]->remove(k);
        }
        return;
    }

    std::string BTreeNode::getPred(int idx)
    {
        std::shared_ptr<BTreeNode> cur = children[idx];
        while (!cur->leaf)
            cur = cur->children[cur->n];
        return cur->keys[cur->n - 1];
    }

    std::string BTreeNode::getSucc(int idx)
    {
        std::shared_ptr<BTreeNode> cur = children[idx + 1];
        while (!cur->leaf)
            cur = cur->children[0];
        return cur->keys[0];
    }

    void BTreeNode::fill(int idx)
    {
        if (idx != 0 && children[idx - 1]->n >= t)
            borrowFromPrev(idx);
        else if (idx != n && children[idx + 1]->n >= t)
            borrowFromNext(idx);
        else
        {
            if (idx != n)
                merge(idx);
            else
                merge(idx - 1);
        }
        return;
    }

    void BTreeNode::borrowFromPrev(int idx)
    {
        std::shared_ptr<BTreeNode> child = children[idx];
        std::shared_ptr<BTreeNode> sibling = children[idx - 1];

        for (int i = child->n - 1; i >= 0; --i)
        {
            child->keys[i + 1] = child->keys[i];
            child->values[i + 1] = child->values[i];
        }

        if (!child->leaf)
        {
            for (int i = child->n; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        child->keys[0] = keys[idx - 1];
        child->values[0] = values[idx - 1];

        if (!leaf)
            child->children[0] = sibling->children[sibling->n];

        keys[idx - 1] = sibling->keys[sibling->n - 1];
        values[idx - 1] = sibling->values[sibling->n - 1];

        child->n += 1;
        sibling->n -= 1;

        return;
    }

    void BTreeNode::borrowFromNext(int idx)
    {
        std::shared_ptr<BTreeNode> child = children[idx];
        std::shared_ptr<BTreeNode> sibling = children[idx + 1];

        child->keys[child->n] = keys[idx];
        child->values[child->n] = values[idx];

        if (!child->leaf)
            child->children[child->n + 1] = sibling->children[0];

        keys[idx] = sibling->keys[0];
        values[idx] = sibling->values[0];

        for (int i = 1; i < sibling->n; ++i)
        {
            sibling->keys[i - 1] = sibling->keys[i];
            sibling->values[i - 1] = sibling->values[i];
        }

        if (!sibling->leaf)
        {
            for (int i = 1; i <= sibling->n; ++i)
                sibling->children[i - 1] = sibling->children[i];
        }

        child->n += 1;
        sibling->n -= 1;

        return;
    }

    void BTreeNode::merge(int idx)
    {
        std::shared_ptr<BTreeNode> child = children[idx];
        std::shared_ptr<BTreeNode> sibling = children[idx + 1];

        child->keys[t - 1] = keys[idx];
        child->values[t - 1] = values[idx];

        for (int i = 0; i < sibling->n; ++i)
        {
            child->keys[i + t] = sibling->keys[i];
            child->values[i + t] = sibling->values[i];
        }

        if (!child->leaf)
        {
            for (int i = 0; i <= sibling->n; ++i)
                child->children[i + t] = sibling->children[i];
        }

        for (int i = idx + 1; i < n; ++i)
        {
            keys[i - 1] = keys[i];
            values[i - 1] = values[i];
        }

        for (int i = idx + 2; i <= n; ++i)
            children[i - 1] = children[i];

        child->n += sibling->n + 1;
        n--;

        return;
    }
}