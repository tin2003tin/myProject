#include <iostream>
#include "../RedBlackTree.hpp"

int main()
{
    tin::RedBlackTree<int, std::string> tree;
    // Insert elements
    tree[3] = "Three";
    tree[1] = "One";
    tree[5] = "Five";
    tree[10] = "Ten";
    tree[8] = "Eight";
    tree[9] = "Nine";
    tree[2] = "Two";
    tree[4] = "Four";
    tree[7] = "Seven";
    tree[6] = "Six";

    // Display elements in-order
    std::cout << "Elements in Red-Black Tree (In-Order):" << std::endl;
    tree.displayInOrder();

    // Display elements in breadth-first order
    std::cout << "Elements in Red-Black Tree (Breadth-First):" << std::endl;
    tree.displayBreadth();

    std::cout << "key: 9 " << tree[9] << std::endl;

    tree.erase(6);
    tree.erase(5);
    tree.erase(2);
    tree.erase(9);
    tree.erase(1);
    tree.erase(8);
    std::cout
        << "\n\n";
    tree.displayInOrder();
    std::cout << "\n";
    tree.displayBreadth();
    return 0;
}
