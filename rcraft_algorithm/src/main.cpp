#include "rcraft/algorithm.hpp"

int main(int argc, const char *const *argv)
{
    rcraft::algorithm::Path::SharedPtr path = std::make_shared<rcraft::algorithm::Path>();

    std::string map_path = "C:/mn_ws/rcraft/rcraft_maps/start.bmp";
    path->load_map(map_path);

    int start_x = 130, start_y = 383;
    int goal_x  = 259, goal_y = 384;

    auto result_path = path->calculate_path_a_star(start_x, start_y, goal_x, goal_y);

    if (result_path.empty())
    {
        std::cout << "Path Not Found!" << std::endl;
    }
    else
    {
        std::cout << "Path (Total " << result_path.size() << '\n';
        for (const std::pair<int, int> &p : result_path)
        {
            std::cout << "Point (" << p.first << ", " << p.second << ")" << '\n';
        }
    }
    return 0;
}