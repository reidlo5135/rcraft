#ifndef RCRAFT_MAP_SERVER_HPP
#define RCRAFT_MAP_SERVER_HPP

#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "rcraft/map_converter.hpp"
#include "rcraft/map_saver.hpp"

#define EXT_BMP "bmp"
#define EXT_JPG "jpg"
#define EXT_JPEG "jpeg"
#define EXT_PNG "png"
#define EXT_PGM "pgm"

namespace rcraft::map
{
    class MapServer final
    {
    private:
        MapConverter::UniquePtr map_converter_;
        cv::Mat optimization(const cv::Mat &map, const std::string &ext);

    public:
        explicit MapServer();
        virtual ~MapServer();
        cv::Mat load_map(const std::string &map_file_path);

    public:
        using SharedPtr = std::shared_ptr<MapServer>;
        using UniquePtr = std::unique_ptr<MapServer>;
    };
}

#endif // RCRAFT_MAP_SERVER_HPP
