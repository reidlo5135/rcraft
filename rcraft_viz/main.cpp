#include <QApplication>
#include <QWidget>
#include <rcraft/global_planner.hpp>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // rcraft::planner::GlobalPlanner::SharedPtr gp = std::make_shared<rcraft::planner::GlobalPlanner>();
    // std::string map_path = "C:/mn_ws/rcraft/rcraft_map_server/maps/start.bmp";
    // gp->load_map(map_path);
    //
    // int start_x = 130, start_y = 383;
    // int goal_x  = 259, goal_y = 384;
    //
    // auto result_path = gp->plan_by_a_star(start_x, start_y, goal_x, goal_y);
    //
    // if (result_path.empty())
    // {
    //     std::cout << "Path Not Found!" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Path (Total " << result_path.size() << '\n';
    //     for (const std::pair<int, int> &p : result_path)
    //     {
    //         std::cout << "Point (" << p.first << ", " << p.second << ")" << '\n';
    //     }
    // }

    QWidget window;
    window.resize(320, 240);
    window.setWindowTitle("rcraft_viz - Qt5 Test");
    window.show();

    return app.exec();
}