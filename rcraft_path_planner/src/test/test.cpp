#include "rcraft/global_planner.hpp"

int main(int argc, const char *const *argv)
{
    rcraft::planner::GlobalPlanner::SharedPtr global_planner = std::make_shared<rcraft::planner::GlobalPlanner>();

    std::string map_path = "C:/mn_ws/rcraft/rcraft_map_server/maps/start.bmp";
    global_planner->load_map(map_path);

    int start_x = 130, start_y = 383;
    int goal_x  = 259, goal_y = 384;

    auto result_path = global_planner->plan_by_a_star(start_x, start_y, goal_x, goal_y);

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