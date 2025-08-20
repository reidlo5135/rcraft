#ifndef RCRAFT_MAP_CONVERTER_HPP
#define RCRAFT_MAP_CONVERTER_HPP

#include <memory>
#include <filesystem>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#define COLOR_BG "green"
#define COLOR_OBSTACLE "red"
#define COLOR_START "magenta"
#define COLOR_GOAL "yellow"

constexpr static int S_MIN = 90;
constexpr static int V_MIN = 60;
constexpr static int OBSTACLE_MAX = 32;
constexpr static int RADIUS = 10;

namespace rcraft::map
{
    class MapConverter final
    {
    private:
        cv::Mat map_;

    public:
        explicit MapConverter();
        virtual ~MapConverter();

        static cv::Mat color2gray(const cv::Mat &map);
        static cv::Mat obstacle_inflation(const cv::Mat &map);

    public:
        using SharedPtr = std::shared_ptr<MapConverter>;
        using UniquePtr = std::unique_ptr<MapConverter>;
    };
}

#endif // RCRAFT_MAP_CONVERTER_HPP
