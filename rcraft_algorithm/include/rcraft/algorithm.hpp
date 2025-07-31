#ifndef RCRAFT_ALGORITHM_HPP
#define RCRAFT_ALGORITHM_HPP


#include <opencv2/opencv.hpp>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace rcraft
{
    namespace algorithm
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

        class Path final
        {
        private:
            cv::Mat map_;

        public:
            explicit Path();
            virtual ~Path();

            void load_map(const std::string &map_file_path);
            std::vector<std::pair<int, int>> calculate_path_a_star(
                int start_x, int start_y,
                int goal_x, int goal_y);

        public:
            using SharedPtr = std::shared_ptr<Path>;
        };
    }
}

#endif // RCRAFT_ALGORITHM_HPP
