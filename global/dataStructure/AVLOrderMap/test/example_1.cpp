#include <iostream>
#include <string>
#include "../AVLOrderMap.hpp"
struct User
{
    int id;
    std::string name;
    int age;

    bool operator<(const User &other) const
    {
        return id < other.id;
    }

    bool operator>(const User &other) const
    {
        return id > other.id;
    }

    friend std::ostream &operator<<(std::ostream &os, const User &user)
    {
        os << "User{id: " << user.id << ", name: " << user.name << ", age: " << user.age << "}";
        return os;
    }
};

User createUser(int id, std::string name, int age)
{
    User user{id, name, age};
    return user;
}

int main()
{
    tin::AVLOrderMap<int, User> map;

    // Create users
    User user1 = createUser(1, "tin", 20);
    User user2 = createUser(2, "nut", 20);
    User user3 = createUser(3, "mark", 20);
    User user4 = createUser(4, "great", 20);
    User user5 = createUser(5, "job", 20);

    // Insert elements
    map[user1.id] = user1;
    map[user2.id] = user2;
    map[user3.id] = user3;
    map[user4.id] = user4;
    map[user5.id] = user5;

    // Display elements
    std::cout << "Elements in AVL OrderMap:" << std::endl;
    map.displayInOrder();

    // Find elements
    std::cout << "Find key 3: " << *map.find(3) << std::endl;
    std::cout << "Find key 5: " << *map.find(5) << std::endl;

    // Erase an element
    map.erase(3);
    std::cout << "After erasing key 3:" << std::endl;
    map.displayInOrder();

    return 0;
}