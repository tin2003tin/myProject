#ifndef ASTAR_HPP
#define ASTAR_HPP

#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <iostream>

namespace tin
{

    typedef struct Position
    {
        int x, y;

        Position(int x = 0, int y = 0) : x(x), y(y) {}

        bool operator==(const Position &other) const;
    } Position;

    template <typename Board = std::vector<std::vector<int>>>
    class AStar
    {
    private:
        struct Node
        {
            Position position;
            std::shared_ptr<Node> parent;
            int gCost;
            int hCost;
            int fCost() const;

            Node(Position pos, std::shared_ptr<Node> par, int g, int h);
        };

        int row;
        int col;
        Board board;

        std::vector<Position> getNeighbors(const Position &pos);
        int heuristic(const Position &a, const Position &b);
        bool is_valid(const Position &pos);
        bool is_unblocked(const Position &pos);
        bool is_destination(const Position &pos, const Position &dest);

    public:
        AStar();
        explicit AStar(const Board &b);

        void setBoard(const Board &newBoard);

        std::vector<Position> findPath(const Position &start, const Position &end, bool isShow = false);
        void displayBoard() const;
        void displayFindPath(const std::vector<Position> &path) const;
    };
}

#include "AStar.tpp"

#endif
