#ifndef AVL_ORDERMAP_HPP
#define AVL_ORDERMAP_HPP

#include <iostream>
#include <functional>
#include <memory>
#include <queue>

namespace tin
{
    template <typename Key, typename Value, typename Compare = std::less<Key>>
    class AVLOrderMap
    {
    private:
        struct AVLOrderMapNode
        {
            std::pair<Key, Value> data;
            size_t height;
            std::weak_ptr<AVLOrderMapNode> parent;
            std::shared_ptr<AVLOrderMapNode> left;
            std::shared_ptr<AVLOrderMapNode> right;

            explicit AVLOrderMapNode(const std::pair<Key, Value> &data)
                : data(data), height(1), parent(), left(nullptr), right(nullptr) {}
        };

    public:
        using Node = AVLOrderMapNode;

        AVLOrderMap(Compare comp = Compare())
            : head(nullptr), compareFunc(comp), size(0) {}

        ~AVLOrderMap() = default;

        void insert(const Key &key, const Value &value);
        void erase(const Key &key);
        Value *find(const Key &key);
        bool empty() const { return size == 0; }
        size_t getSize() const { return size; }
        size_t getHeight() const { return getHeight(head); }
        void displayInOrder() const;
        void displayBreadth() const;
        Value &operator[](const Key &key);

    private:
        std::shared_ptr<Node> head;
        Compare compareFunc;
        size_t size;

        std::shared_ptr<Node> insert(std::shared_ptr<Node> node, const Key &key, const Value &value);
        std::shared_ptr<Node> erase(std::shared_ptr<Node> node, const Key &key);
        std::shared_ptr<Node> findMin(std::shared_ptr<Node> node) const;
        std::shared_ptr<Node> findMax(std::shared_ptr<Node> node) const;
        int getHeight(std::shared_ptr<Node> node) const;
        int getBalance(std::shared_ptr<Node> node) const;
        void updateHeight(std::shared_ptr<Node> node);
        std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> y);
        std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> x);
        void displayInOrder(std::shared_ptr<Node> node) const;
        void displayBreadth(std::shared_ptr<Node> node) const;
    };
}
#include "AVLOrderMap.tpp"

#endif // AVL_ORDERMAP_HPP
