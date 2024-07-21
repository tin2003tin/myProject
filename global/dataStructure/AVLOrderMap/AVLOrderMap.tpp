#ifndef AVL_ORDERMAP_TPP
#define AVL_ORDERMAP_TPP

#include "AVLOrderMap.hpp"

namespace tin
{
    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::insert(const Key &key, const Value &value)
    {
        head = insert(head, key, value);
        size++;
    }

    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::erase(const Key &key)
    {
        head = erase(head, key);
        size--;
    }

    template <typename Key, typename Value, typename Compare>
    Value *AVLOrderMap<Key, Value, Compare>::find(const Key &key)
    {
        std::shared_ptr<Node> current = head;
        while (current != nullptr)
        {
            if (compareFunc(key, current->data.first))
            {
                current = current->left;
            }
            else if (compareFunc(current->data.first, key))
            {
                current = current->right;
            }
            else
            {
                return &current->data.second;
            }
        }
        return nullptr;
    }

    template <typename Key, typename Value, typename Compare>
    Value &AVLOrderMap<Key, Value, Compare>::operator[](const Key &key)
    {
        Value *value = find(key);
        if (value == nullptr)
        {
            Value defaultValue{};
            insert(key, defaultValue);
            value = find(key);
        }
        return *value;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::insert(
        std::shared_ptr<Node> node, const Key &key, const Value &value)
    {
        if (node == nullptr)
        {
            return std::make_shared<Node>(std::make_pair(key, value));
        }
        if (compareFunc(key, node->data.first))
        {
            node->left = insert(node->left, key, value);
            node->left->parent = node;
        }
        else if (compareFunc(node->data.first, key))
        {
            node->right = insert(node->right, key, value);
            node->right->parent = node;
        }
        else
        {
            node->data.second = value;
            return node;
        }
        updateHeight(node);
        int balance = getBalance(node);
        if (balance > 1 && compareFunc(key, node->right->data.first))
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        if (balance < -1 && compareFunc(node->left->data.first, key))
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance > 1)
        {
            return rotateLeft(node);
        }
        if (balance < -1)
        {
            return rotateRight(node);
        }
        return node;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::erase(
        std::shared_ptr<Node> node, const Key &key)
    {
        if (node == nullptr)
        {
            return node;
        }
        if (compareFunc(key, node->data.first))
        {
            node->left = erase(node->left, key);
        }
        else if (compareFunc(node->data.first, key))
        {
            node->right = erase(node->right, key);
        }
        else
        {
            if (node->left == nullptr || node->right == nullptr)
            {
                std::shared_ptr<Node> temp = node->left ? node->left : node->right;
                if (temp == nullptr)
                {
                    temp = node;
                    node = nullptr;
                }
                else
                {
                    *node = *temp;
                }
            }
            else
            {
                std::shared_ptr<Node> temp = findMin(node->right);
                node->data = temp->data;
                node->right = erase(node->right, temp->data.first);
            }
        }
        if (node == nullptr)
        {
            return node;
        }
        updateHeight(node);
        int balance = getBalance(node);
        if (balance > 1 && getBalance(node->right) >= 0)
        {
            return rotateLeft(node);
        }
        if (balance > 1 && getBalance(node->right) < 0)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        if (balance < -1 && getBalance(node->left) <= 0)
        {
            return rotateRight(node);
        }
        if (balance < -1 && getBalance(node->left) > 0)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        return node;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::findMin(
        std::shared_ptr<Node> node) const
    {
        std::shared_ptr<Node> current = node;
        while (current->left != nullptr)
        {
            current = current->left;
        }
        return current;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::findMax(
        std::shared_ptr<Node> node) const
    {
        std::shared_ptr<Node> current = node;
        while (current->right != nullptr)
        {
            current = current->right;
        }
        return current;
    }

    template <typename Key, typename Value, typename Compare>
    int AVLOrderMap<Key, Value, Compare>::getHeight(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
        {
            return 0;
        }
        return node->height;
    }

    template <typename Key, typename Value, typename Compare>
    int AVLOrderMap<Key, Value, Compare>::getBalance(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
        {
            return 0;
        }
        return getHeight(node->right) - getHeight(node->left);
    }

    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::updateHeight(std::shared_ptr<Node> node)
    {
        if (node != nullptr)
        {
            node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
        }
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::rotateRight(
        std::shared_ptr<Node> y)
    {
        std::shared_ptr<Node> x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
        {
            x->right->parent = y;
        }
        x->right = y;
        x->parent = y->parent;
        y->parent = x;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    template <typename Key, typename Value, typename Compare>
    std::shared_ptr<typename AVLOrderMap<Key, Value, Compare>::Node> AVLOrderMap<Key, Value, Compare>::rotateLeft(
        std::shared_ptr<Node> x)
    {
        std::shared_ptr<Node> y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
        {
            y->left->parent = x;
        }
        y->left = x;
        y->parent = x->parent;
        x->parent = y;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::displayInOrder() const
    {
        std::cout << "{ ";
        displayInOrder(head);
        std::cout << "}" << std::endl;
    }

    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::displayBreadth() const
    {
        std::cout << "{ ";
        displayBreadth(head);
        std::cout << "}" << std::endl;
    }

    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::displayInOrder(std::shared_ptr<Node> node) const
    {
        if (node != nullptr)
        {
            displayInOrder(node->left);
            std::cout << "(" << node->data.first << ", " << node->data.second << ") ";
            displayInOrder(node->right);
        }
    }
    template <typename Key, typename Value, typename Compare>
    void AVLOrderMap<Key, Value, Compare>::displayBreadth(std::shared_ptr<Node> node) const
    {
        if (node == nullptr)
        {
            return;
        }
        std::queue<std::shared_ptr<Node>> queue;
        queue.push(node);
        while (!queue.empty())
        {
            std::shared_ptr<Node> top = queue.front();
            if (top->left)
            {
                queue.push(top->left);
            }
            if (top->right)
            {
                queue.push(top->right);
            }
            std::cout << "(" << top->data.first << ", " << top->data.second << ") ";
            queue.pop();
        }
    }
}

#endif // AVL_ORDERMAP_TPP
