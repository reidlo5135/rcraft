#ifndef RCRAFT_MAP_MODEL_HPP
#define RCRAFT_MAP_MODEL_HPP

#include <memory>
#include <string>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

namespace rcraft::viz
{
    class MapModel final
    {
    private:
        cv::Mat mapImage_;
        std::string mapImagePath_;
        float mapResolution_ = 1.0f;
        std::tuple<float, float> mapOrigin_ = {0.0f, 0.0f};

    public:
        explicit MapModel();
        virtual ~MapModel();

        bool loadMapFromPath(const std::string& path);
        QImage getQImage() const;

        [[nodiscard]] const std::string& mapImagePath() const { return mapImagePath_; }
        float mapResolution() const { return mapResolution_; }
        std::tuple<float, float> mapOrigin() const { return mapOrigin_; }
        bool isValid() const { return !mapImage_.empty(); }

    public:
        using SharedPtr = std::shared_ptr<MapModel>;
        using UniquePtr = std::unique_ptr<MapModel>;
    };
}

#endif // RCRAFT_MAP_MODEL_HPP
