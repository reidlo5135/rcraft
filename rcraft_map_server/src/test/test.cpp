#include "rcraft/map_server.hpp"

int main(int argc, const char *const argv)
{
    rcraft::map::MapServer::SharedPtr map_server = std::make_shared<rcraft::map::MapServer>();
    cv::Mat map = map_server->load_map("C:/mn_ws/rcraft/rcraft_map_server/maps/start.bmp");
    return 0;
}