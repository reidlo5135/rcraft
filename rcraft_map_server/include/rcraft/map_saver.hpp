#ifndef RCRAFT_MAP_SAVER_HPP
#define RCRAFT_MAP_SAVER_HPP

#include <memory>
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>


namespace fs = std::filesystem;

namespace rcraft::map
{
    class MapSaver final
    {
    public:
        explicit MapSaver();
        virtual ~MapSaver();

        static void save(const cv::Mat &map, const std::string &path);

    public:
        using SharedPtr = std::shared_ptr<MapSaver>;
        using UniquePtr = std::shared_ptr<MapSaver>;
    };
}

#endif // RCRAFT_MAP_SAVER_HPP
