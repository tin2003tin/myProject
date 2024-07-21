#ifndef REDBLACKTREE_HPP
#define REDBLACKTREE_HPP

#include <memory>
#include <functional>
#include <iostream>

namespace tin
{
    template <typename Key, typename Value, typename Compare = std::function<bool(const Key &, const Key &)>>
    class RedBlackTree
    {
    private:
        struct Node
        {
            std::pair<Key, Value> data;
            bool isBlack;
            std::weak_ptr<Node> parent;
            std::shared_ptr<Node> left;
            std::shared_ptr<Node> right;

            explicit Node(const std::pair<Key, Value> &data, bool isBlack = false)
                : data(data), isBlack(isBlack), parent(), left(nullptr), right(nullptr) {}
        };

    public:
        RedBlackTree(Compare comp = std::greater<Key>()) : head(nullptr), compareFunc(comp), size(0) {}

        ~RedBlackTree() = default;

        void insert(const Key &key, const Value &value);
        void erase(const Key &key);
        Value &find(const Key &key);
        Value &operator[](const Key &key);
        bool empty() const { return size == 0; }
        size_t getSize() const { return size; }
        size_t getHeight() const;
        void displayInOrder() const;
        void displayBreadth() const;

    private:
        std::shared_ptr<Node> head;
        Compare compareFunc;
        size_t size;

        std::shared_ptr<Node> findMin(std::shared_ptr<Node> node) const;
        std::shared_ptr<Node> findMax(std::shared_ptr<Node> node) const;
        std::shared_ptr<Node> findNode(const Key &key) const;
        std::shared_ptr<Node> findParentNode(std::shared_ptr<Node> node, const Key &key) const;
        int getHeight(std::shared_ptr<Node> node) const;
        int getBalance(std::shared_ptr<Node> node) const;
        void updateHeight(std::shared_ptr<Node> node);
        void rotateRight(std::shared_ptr<Node> y);
        void rotateLeft(std::shared_ptr<Node> x);
        void eraseLeafNode(std::shared_ptr<Node> node);
        void eraseNodeOneChild(std::shared_ptr<Node> node);
        void eraseNodeTwoChild(std::shared_ptr<Node> node);
        void fixInsert(std::shared_ptr<Node> node);
        void fixErase(std::shared_ptr<Node> node);
        void displayInOrder(std::shared_ptr<Node> node) const;
        void displayBreadth(std::shared_ptr<Node> node) const;
    };
}

#include "RedBlackTree.tpp"

#endif // REDBLACKTREE_HPP
