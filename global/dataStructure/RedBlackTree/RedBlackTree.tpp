#ifndef REDBLACKTREE_TPP
#define REDBLACKTREE_TPP

#include "RedBlackTree.hpp"
#include <queue>
#include <stdexcept>

namespace tin
{
    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::insert(const Key &key, const Value &value)
    {
        if (head == nullptr)
        {
            head = std::make_shared<Node>(std::make_pair(key, value), true);
            size++;
            return;
        }
        std::shared_ptr<Node> parent = findParentNode(head, key);
        std::shared_ptr<Node> node = nullptr;
        if (parent == nullptr)
        {
            std::make_shared<Node>(std::make_pair(key, value), true);
        }
        else if (compareFunc(parent->data.first, key))
        {
            if (parent->left != nullptr)
            {
                parent->left->data.second = value;
            }
            node = std::make_shared<Node>(std::make_pair(key, value), false);
            parent->left = node;
            node->parent = parent;
        }
        else
        {
            if (parent->right != nullptr)
            {
                parent->right->data.second = value;
            }
            node = std::make_shared<Node>(std::make_pair(key, value), false);
            parent->right = node;
            node->parent = parent;
        }
        if (node)
        {
            size++;
            fixInsert(node);
        }
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::fixInsert(std::shared_ptr<Node> node)
    {
        std::shared_ptr<Node> parent, grandparent;
        while ((node != head) && (node->isBlack == false) &&
               (node->parent.lock()->isBlack == false))
        {
            parent = node->parent.lock();
            grandparent = parent->parent.lock();
            if (parent == grandparent->left)
            {
                auto uncle = grandparent->right;
                if (uncle != nullptr && uncle->isBlack == false)
                {
                    grandparent->isBlack = false;
                    parent->isBlack = true;
                    uncle->isBlack = true;
                    node = grandparent;
                }
                else
                {
                    if (node == parent->right)
                    {
                        rotateLeft(parent);
                        node = parent;
                        parent = node->parent.lock();
                    }
                    rotateRight(grandparent);
                    std::swap(parent->isBlack, grandparent->isBlack);
                    node = parent;
                }
            }
            else
            {
                auto uncle = grandparent->left;
                if (uncle != nullptr && uncle->isBlack == false)
                {
                    grandparent->isBlack = false;
                    parent->isBlack = true;
                    uncle->isBlack = true;
                    node = grandparent;
                }
                else
                {
                    if (node == parent->left)
                    {
                        rotateRight(parent);
                        node = parent;
                        parent = node->parent.lock();
                    }
                    rotateLeft(grandparent);
                    std::swap(parent->isBlack, grandparent->isBlack);
                    node = parent;
                }
            }
        }
        head->isBlack = true;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::erase(const Key &key)
    {
        std::shared_ptr<Node> node = findNode(key);

        if (node == nullptr)
            return;
        if (!node->left && !node->right)
        {
            eraseLeafNode(node);
        }
        else if (node->left && node->right)
        {
            eraseNodeTwoChild(node);
        }
        else
        {
            eraseNodeOneChild(node);
        }
        size--;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::fixErase(std::shared_ptr<Node> node)
    {
        while (node != head && node->isBlack)
        {
            auto parent = node->parent.lock();
            if (node == parent->left)
            {
                auto sibling = parent->right;
                if (!sibling)
                    return;
                if (!sibling->isBlack)
                {
                    sibling->isBlack = true;
                    parent->isBlack = false;
                    rotateLeft(parent);
                    sibling = parent->right;
                }
                if ((sibling->left == nullptr || sibling->left->isBlack) &&
                    (sibling->right == nullptr || sibling->right->isBlack))
                {
                    sibling->isBlack = false;
                    node = parent;
                }
                else
                {
                    if (sibling->right == nullptr || sibling->right->isBlack)
                    {
                        sibling->left->isBlack = true;
                        sibling->isBlack = false;
                        rotateRight(sibling);
                        sibling = parent->right;
                    }
                    sibling->isBlack = parent->isBlack;
                    parent->isBlack = true;
                    if (sibling->right)
                        sibling->right->isBlack = true;
                    rotateLeft(parent);
                    node = head;
                }
            }
            else
            {
                auto sibling = parent->left;
                if (!sibling)
                    return;
                if (!sibling->isBlack)
                {
                    sibling->isBlack = true;
                    parent->isBlack = false;
                    rotateRight(parent);
                    sibling = parent->left;
                }
                if ((sibling->left == nullptr || sibling->left->isBlack) &&
                    (sibling->right == nullptr || sibling->right->isBlack))
                {
                    sibling->isBlack = false;
                    node = parent;
                }
                else
                {
                    if (sibling->left == nullptr || sibling->left->isBlack)
                    {
                        sibling->right->isBlack = true;
                        sibling->isBlack = false;
                        rotateLeft(sibling);
                        sibling = parent->left;
                    }
                    sibling->isBlack = parent->isBlack;
                    parent->isBlack = true;
                    if (sibling->left)
                        sibling->left->isBlack = true;
                    rotateRight(parent);
                    node = head;
                }
            }
        }
        node->isBlack = true;
    }

    template <typename Key, typename Value, typename Compare>
    Value &RedBlackTree<Key, Value, Compare>::find(const Key &key)
    {
        auto node = findNode(key);
        if (node == nullptr)
            throw std::runtime_error("Key not found");
        return node->data.second;
    }

    template <typename Key, typename Value, typename Compare>
    Value &RedBlackTree<Key, Value, Compare>::operator[](const Key &key)
    {
        std::shared_ptr<Node> node = findNode(key);
        if (node == nullptr)
        {
            Value defaultValue{};
            insert(key, defaultValue);
            node = findNode(key);
        }
        return (node->data.second);
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename RedBlackTree<Key, Value, Compare>::Node> RedBlackTree<Key, Value, Compare>::findNode(const Key &key) const
    {
        auto current = head;
        while (current != nullptr)
        {
            if (compareFunc(current->data.first, key))
            {
                current = current->left;
            }
            else if (compareFunc(key, current->data.first))
            {
                current = current->right;
            }
            else
            {
                return current;
            }
        }
        return nullptr;
    }
    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename RedBlackTree<Key, Value, Compare>::Node> RedBlackTree<Key, Value, Compare>::findParentNode(std::shared_ptr<Node> node, const Key &key) const
    {
        if (node == NULL)
        {
            return NULL;
        }
        if (this->compareFunc(node->data.first, key))
        {
            if (node->left == NULL)
            {
                return node;
            }
            return findParentNode(node->left, key);
        }
        else
        {
            if (node->right == NULL)
            {
                return node;
            }
            return findParentNode(node->right, key);
        }
    }

    template <typename Key, typename Value, typename Compare>
    size_t RedBlackTree<Key, Value, Compare>::getHeight() const
    {
        return getHeight(head);
    }

    template <typename Key, typename Value, typename Compare>
    int RedBlackTree<Key, Value, Compare>::getHeight(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
            return 0;
        return 1 + std::max(getHeight(node->left), getHeight(node->right));
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename RedBlackTree<Key, Value, Compare>::Node> RedBlackTree<Key, Value, Compare>::findMin(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
        {
            return nullptr;
        }
        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename RedBlackTree<Key, Value, Compare>::Node> RedBlackTree<Key, Value, Compare>::findMax(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
        {
            return nullptr;
        }
        while (node->right != nullptr)
        {
            node = node->right;
        }
        return node;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::rotateLeft(std::shared_ptr<Node> x)
    {
        auto y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent.lock() == nullptr)
        {
            head = y;
        }
        else if (x == x->parent.lock()->left)
        {
            x->parent.lock()->left = y;
        }
        else
        {
            x->parent.lock()->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::rotateRight(std::shared_ptr<Node> y)
    {
        auto x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent.lock() == nullptr)
        {
            head = x;
        }
        else if (y == y->parent.lock()->right)
        {
            y->parent.lock()->right = x;
        }
        else
        {
            y->parent.lock()->left = x;
        }
        x->right = y;
        y->parent = x;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::displayInOrder() const
    {
        displayInOrder(head);
        std::cout << std::endl;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::displayInOrder(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
            return;
        displayInOrder(node->left);
        std::cout << "(" << node->data.first << ", " << node->data.second << ") ";
        displayInOrder(node->right);
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::displayBreadth() const
    {
        if (head == nullptr)
            return;
        std::queue<std::shared_ptr<Node>> q;
        q.push(head);
        while (!q.empty())
        {
            auto node = q.front();
            q.pop();
            std::cout << "(" << node->data.first << ", " << node->data.second << ", " << (node->isBlack ? "black" : "red") << ") ";
            if (node->left != nullptr)
                q.push(node->left);
            if (node->right != nullptr)
                q.push(node->right);
        }
        std::cout << std::endl;
    }

    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::eraseLeafNode(std::shared_ptr<Node> node)
    {
        fixErase(node);
        if (node == head)
        {
            head = nullptr;
        }
        else
        {
            if (node->parent.lock()->left == node)
            {
                node->parent.lock()->left = nullptr;
            }
            else
            {
                node->parent.lock()->right = nullptr;
            }
        }
    }
    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::eraseNodeOneChild(std::shared_ptr<Node> node)
    {
        std::shared_ptr<Node> child;
        if (node == head)
        {
            head = (node->right != nullptr) ? node->right : node->left;
            if (head != nullptr)
            {
                head->parent.lock() = nullptr;
            }
            fixErase(head);
        }
        else
        {
            if (node->left != nullptr)
            {
                child = node->left;
            }
            else
            {
                child = node->right;
            }
            if (node->parent.lock()->left == node)
            {
                node->parent.lock()->left = child;
            }
            else
            {
                node->parent.lock()->right = child;
            }

            if (child)
            {
                child->parent = node->parent;
                fixErase(child);
            }
        }
    }
    template <typename Key, typename Value, typename Compare>
    void RedBlackTree<Key, Value, Compare>::eraseNodeTwoChild(std::shared_ptr<Node> node)
    {
        if (node->left == nullptr)
        {
            return;
        }
        std::shared_ptr<Node> maxLeft = findMax(node->left);

        std::swap(node->data, maxLeft->data);

        if (maxLeft->left || maxLeft->right)
        {
            eraseNodeOneChild(maxLeft);
        }
        else
        {
            eraseLeafNode(maxLeft);
        }
    }
}
#endif // REDBLACKTREE_TPP
