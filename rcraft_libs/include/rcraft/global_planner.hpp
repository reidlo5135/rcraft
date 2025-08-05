#ifndef RCRAFT_GLOBAL_PLANNER_HPP
#define RCRAFT_GLOBAL_PLANNER_HPP

#include <opencv2/opencv.hpp>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>

namespace rcraft::planner
{
    struct Node
    {
        int x;
        int y;
        float cost;
        float priority;
        Node *parent;
        Node(int x, int y, float c, float p, Node *pr)
            : x(x), y(y), cost(c), priority(p), parent(pr) {}
    };

    inline float heuristic(int x1, int y1, int x2, int y2)
    {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    class GlobalPlanner final
    {
    private:
        cv::Mat map_;

    public:
        explicit GlobalPlanner();
        virtual ~GlobalPlanner();

        void load_map(const std::string &map_file_path);
        std::vector<std::pair<int, int>> plan_by_a_star(int start_x, int start_y, int goal_x, int goal_y);

    public:
        using SharedPtr = std::shared_ptr<GlobalPlanner>;
    };
}

#endif // RCRAFT_GLOBAL_PLANNER_HPP
