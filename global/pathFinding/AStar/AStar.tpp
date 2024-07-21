#ifndef ASTAR_TPP
#define ASTAR_TPP

#include "AStar.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace tin
{

    bool Position::operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }

    template <typename Board>
    AStar<Board>::Node::Node(Position pos, std::shared_ptr<Node> par, int g, int h)
        : position(pos), parent(par), gCost(g), hCost(h) {}

    template <typename Board>
    int AStar<Board>::Node::fCost() const { return gCost + hCost; }

    template <typename Board>
    AStar<Board>::AStar() : row(0), col(0), board() {}

    template <typename Board>
    AStar<Board>::AStar(const Board &b) : board(b)
    {
        if (board.empty() || board[0].empty())
        {
            throw std::invalid_argument("Board cannot be empty");
        }
        row = board.size();
        col = board[0].size();
    }

    template <typename Board>
    void AStar<Board>::setBoard(const Board &newBoard)
    {
        if (newBoard.empty() || newBoard[0].empty())
        {
            throw std::invalid_argument("New board cannot be empty");
        }
        board = newBoard;
        row = board.size();
        col = board[0].size();
    }

    template <typename Board>
    std::vector<Position> AStar<Board>::getNeighbors(const Position &pos)
    {
        std::vector<Position> neighbors;
        std::vector<Position> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (const auto &dir : directions)
        {
            int newX = pos.x + dir.x;
            int newY = pos.y + dir.y;

            if (is_valid({newX, newY}))
            {
                neighbors.emplace_back(newX, newY);
            }
        }

        return neighbors;
    }

    template <typename Board>
    int AStar<Board>::heuristic(const Position &a, const Position &b)
    {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    template <typename Board>
    bool AStar<Board>::is_valid(const Position &pos)
    {
        return pos.x >= 0 && pos.x < col && pos.y >= 0 && pos.y < row;
    }

    template <typename Board>
    bool AStar<Board>::is_unblocked(const Position &pos)
    {
        return board[pos.y][pos.x] == 0;
    }

    template <typename Board>
    bool AStar<Board>::is_destination(const Position &pos, const Position &dest)
    {
        return pos == dest;
    }

    template <typename Board>
    std::vector<Position> AStar<Board>::findPath(const Position &start, const Position &end, bool isShow)
    {
        if (!is_valid(start) || !is_valid(end))
        {
            throw std::invalid_argument("Start or end position is invalid");
        }

        if (!is_unblocked(start) || !is_unblocked(end))
        {
            throw std::invalid_argument("Start or end position is blocked");
        }

        auto compare = [](const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b)
        {
            return a->fCost() > b->fCost();
        };

        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(compare)> openSet(compare);
        std::unordered_map<int, std::shared_ptr<Node>> allNodes;

        auto getNodeKey = [&](const Position &pos)
        {
            return pos.x * col + pos.y;
        };

        auto startNode = std::make_shared<Node>(start, nullptr, 0, heuristic(start, end));
        openSet.push(startNode);
        allNodes[getNodeKey(start)] = startNode;

        while (!openSet.empty())
        {
            auto currentNode = openSet.top();
            openSet.pop();

            if (is_destination(currentNode->position, end))
            {
                std::vector<Position> path;
                while (currentNode)
                {
                    path.push_back(currentNode->position);
                    currentNode = currentNode->parent;
                }
                std::reverse(path.begin(), path.end());
                if (isShow)
                {
                    displayFindPath(path);
                }

                return path;
            }

            for (const auto &neighborPos : getNeighbors(currentNode->position))
            {
                if (!is_unblocked(neighborPos))
                {
                    continue;
                }

                int newGCost = currentNode->gCost + 1;
                auto neighborKey = getNodeKey(neighborPos);
                auto neighborNode = allNodes.find(neighborKey);

                if (neighborNode == allNodes.end() || newGCost < neighborNode->second->gCost)
                {
                    int hCost = heuristic(neighborPos, end);
                    auto newNode = std::make_shared<Node>(neighborPos, currentNode, newGCost, hCost);
                    openSet.push(newNode);
                    allNodes[neighborKey] = newNode;
                }
            }
        }

        return {};
    }

    template <typename Board>
    void AStar<Board>::displayBoard() const
    {
        for (const auto &row : board)
        {
            for (const auto &cell : row)
            {
                if (cell == 0)
                    std::cout << "- ";
                else
                    std::cout << "\033[31m" << "x " << "\033[0m";
            }
            std::cout << std::endl;
        }
    }

    template <typename Board>
    void AStar<Board>::displayFindPath(const std::vector<Position> &path) const
    {
        auto boardCopy = board;
        for (const auto &pos : path)
        {
            boardCopy[pos.y][pos.x] = 2;
        }

        std::cout << "Board with path:" << std::endl;
        for (const auto &row : boardCopy)
        {
            for (const auto &cell : row)
            {

                if (cell == 0)
                    std::cout << "- ";
                else if (cell == 1)
                    std::cout << "\033[31m" << "x " << "\033[0m";
                else if (cell == 2)
                    std::cout << "\033[32m" << "o " << "\033[0m";
            }
            std::cout << std::endl;
        }
    }

}

#endif
