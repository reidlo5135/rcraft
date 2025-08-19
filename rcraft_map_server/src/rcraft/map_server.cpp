#include "rcraft/map_server.hpp"

using namespace rcraft::map;

MapServer::MapServer()
    : map_converter_(nullptr)
{
}

MapServer::~MapServer() = default;

cv::Mat MapServer::load_map(const std::string &map_file_path)
{
    cv::Mat raw_map = cv::Mat();
    std::cout << "[INFO][MapServer] LoadMap map file path : " << map_file_path << '\n';

    std::string ext;

    try
    {
        if (const size_t &dot = map_file_path.find_last_of('.'); dot != std::string::npos)
        {
            ext = map_file_path.substr(dot + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        }
        else
        {
            ext = "";
            throw std::runtime_error("MapServer LoadMap Unknown file extension");
        }

        std::cout << "[INFO][MapServer] LoadMap ext : " << ext << '\n';

        if (ext == EXT_PGM)
        {
            const cv::Mat &gray_map = cv::imread(map_file_path, cv::IMREAD_GRAYSCALE);

            if (gray_map.empty())
            {
                std::cerr << "" << '\n';
                raw_map = cv::Mat();
                throw std::runtime_error("MapServer LoadMap GrayMap is empty");
            }

            raw_map = gray_map;
        }
        else
        {
            const cv::Mat &color_map = cv::imread(map_file_path, cv::IMREAD_COLOR);

            if (color_map.empty())
            {
                std::cerr << "Path Image Map is Empty" << '\n';
                raw_map = cv::Mat();
                throw std::runtime_error("MapServer LoadMap ColorMap is empty");
            }

            raw_map = color_map;
        }
        std::cout << "[INFO][MapServer] LoadedMap size : " << raw_map.size << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "[INFO][MapServer] LoadMap error : " << e.what() << '\n';
        raw_map = cv::Mat();
        return raw_map;
    }

    const cv::Mat &optimized_map = this->optimization(raw_map, ext);

    return optimized_map;
}

cv::Mat MapServer::optimization(const cv::Mat &map, const std::string &ext)
{
    cv::Mat optimized_map = cv::Mat();
    this->map_converter_ = std::make_unique<MapConverter>();

    if (ext != EXT_PGM)
    {
        std::cout << "[INFO][MapServer] Optimization Color2Gray" << '\n';
        optimized_map = this->map_converter_->color2gray(map);
    }
    else
    {
        optimized_map = map;
    }

    std::cout << "[INFO][MapServer] Optimization Map size : " << optimized_map.size << '\n';

    MapSaver::SharedPtr map_saver = std::make_shared<MapSaver>();
    map_saver->save(optimized_map, "C:/mn_ws/rcraft/rcraft_map_server/maps/drive_map.bmp");

    const int &w = optimized_map.cols;
    const int &h = optimized_map.rows;

    return optimized_map;
}