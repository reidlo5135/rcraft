#include "rcraft/algorithm.hpp"

int main(int argc, const char *const *argv)
{
    rcraft::algorithm::Path::SharedPtr path = std::make_shared<rcraft::algorithm::Path>();

    std::string map_path = "C:/mn_ws/rcraft/rcraft_maps/test-2.bmp";
    path->load_map(map_path);

    int start_x = 0, start_y = 329;
    int goal_x  = 87, goal_y = 111;

    auto result_path = path->calculate_path_a_star(start_x, start_y, goal_x, goal_y);

    if (result_path.empty())
    {
        std::cout << "Path Not Found!" << std::endl;
    }
    else
    {
        std::cout << "Path (총 " << result_path.size() << "Point):" << std::endl;
        for (const auto& p : result_path)
        {
            std::cout << "(" << p.first << ", " << p.second << ")" << std::endl;
        }
    }
    return 0;
}

// int main() {
//     std::string map_path = "C:/mn_ws/rcraft/rcraft_maps/test-2.bmp";
//     cv::Mat img = cv::imread(map_path, cv::IMREAD_COLOR);
//     if (img.empty()) {
//         std::cout << "Failed to Load Image" << std::endl;
//     } else {
//         std::cout << "Succeeded to Load Image" << std::endl;
//     }
//     return 0;
// }