#include "rcraft/model/map_model.hpp"

using namespace rcraft::viz;

MapModel::MapModel() = default;
MapModel::~MapModel() = default;

bool MapModel::loadMapFromPath(const std::string &path)
{
    QFileInfo file(QString::fromStdString(path));

    if (!file.exists())
    {
        qWarning() << "[ERROR][M] File does not exist:" << QString::fromStdString(path);
        return false;
    }

    QString ext = file.suffix().toLower();
    qInfo() << "[INFO][M] Loaded Map Image:" << QString::fromStdString(path) << "," << ext;

    try
    {
        if (ext == "pgm")
        {
            this->mapImage_ = cv::imread(path, cv::IMREAD_GRAYSCALE);
            if (this->mapImage_.empty())
                throw std::runtime_error("OpenCV failed to load PGM file");
            cv::cvtColor(this->mapImage_, this->mapImage_, cv::COLOR_GRAY2RGB);
            qInfo() << "[INFO][M] Loaded PGM map via OpenCV:" << QString::fromStdString(path);
        }
        else
        {
            this->mapImage_ = cv::imread(path, cv::IMREAD_COLOR);
            if (this->mapImage_.empty())
                throw std::runtime_error("OpenCV failed to load image");
            cv::cvtColor(this->mapImage_, this->mapImage_, cv::COLOR_BGR2RGB);
            qInfo() << "[INFO][M] Loaded map:" << QString::fromStdString(path);
        }

        this->mapImagePath_ = path;
        qInfo() << "[INFO][M] Loaded map from" << QString::fromStdString(path);
        return true;
    }
    catch (const std::exception &e)
    {
        qWarning() << "[ERROR][M] Failed to load image:" << e.what();
        return false;
    }
}

QImage MapModel::getQImage() const
{
    if (this->mapImage_.empty())
        return {};

    QImage img(this->mapImage_.data, this->mapImage_.cols, this->mapImage_.rows, this->mapImage_.step, QImage::Format_RGB888);
    return img.copy();
}
