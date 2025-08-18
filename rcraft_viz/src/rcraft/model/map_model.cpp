#include "rcraft/model/map_model.hpp"

using namespace rcraft::viz;

/**
 * @brief Construct a new MapModel with default metadata.
 *
 * @details
 * Initializes:
 * - mapResolution_ = 1.0f
 * - mapOrigin_     = (0.0f, 0.0f)
 * The image/path are left empty until @ref loadMapFromPath() or @ref setMapImage().
 */
MapModel::MapModel()
    : mapResolution_(1.0f)
    , mapOrigin_({0.0f, 0.0f})
{
}

/**
 * @brief Destroy the MapModel.
 */
MapModel::~MapModel() = default;

/**
 * @brief Get the file-system path of the currently stored map image.
 * @return const std::string& Map image path (may be empty).
 */
const std::string &MapModel::getMapImagePath() const
{
    return this->mapImagePath_;
}

/**
 * @brief Set the file-system path associated with the current map image.
 * @param mapImagePath New path to record.
 * @note This function does not perform any I/O.
 */
void MapModel::setMapImagePath(const std::string &mapImagePath)
{
    this->mapImagePath_ = mapImagePath;
}

/**
 * @brief Get the map resolution.
 * @return float Resolution value (application-defined units).
 */
float MapModel::getMapResolution() const
{
    return this->mapResolution_;
}

/**
 * @brief Set the map resolution.
 * @param mapResolution New resolution value.
 */
void MapModel::setMapResolution(const float &mapResolution)
{
    this->mapResolution_ = mapResolution;
}

/**
 * @brief Get the map origin.
 * @return std::tuple<float,float> Origin coordinates (application-defined).
 */
std::tuple<float, float> MapModel::getMapOrigin() const
{
    return this->mapOrigin_;
}

/**
 * @brief Set the map origin.
 * @param mapOrigin Origin coordinates (application-defined).
 */
void MapModel::setMapOrigin(const std::tuple<float, float> &mapOrigin)
{
    this->mapOrigin_ = mapOrigin;
}

/**
 * @brief Check whether a non-empty OpenCV image is currently stored.
 * @return true if an image is available; false otherwise.
 */
bool MapModel::isMapImageValid() const
{
    return !this->getMapImage().empty();
}

/**
 * @brief Load a map image from disk and convert it to an RGB layout for Qt rendering.
 *
 * @details
 * Behavior by extension:
 * - \c .pgm : loaded with @c cv::IMREAD_GRAYSCALE then converted (GRAY->RGB)
 * - others  : loaded with @c cv::IMREAD_COLOR then converted (BGR->RGB)
 *
 * On success, updates:
 * - @ref mapImage_  (RGB layout suitable for QImage::Format_RGB888)
 * - @ref mapImagePath_
 *
 * On failure, logs a warning and returns false.
 *
 * @param path File-system path to the image.
 * @return true on success; false on failure.
 */
bool MapModel::loadMapFromPath(const std::string &path)
{
    const QFileInfo file(QString::fromStdString(path));

    if (!file.exists())
    {
        qWarning() << "[ERROR][M] File does not exist:" << QString::fromStdString(path);
        return false;
    }

    const QString &ext = file.suffix().toLower();
    qInfo() << "[INFO][M] Loaded Map Image:" << QString::fromStdString(path) << "," << ext;

    try
    {
        if (ext == "pgm")
        {
            // Load as grayscale then convert to RGB (Qt-friendly)
            this->setMapImage(cv::imread(path, cv::IMREAD_GRAYSCALE));

            if (this->mapImage_.empty())
            {
                throw std::runtime_error("OpenCV failed to load PGM file");
            }

            cv::cvtColor(this->mapImage_, this->mapImage_, cv::COLOR_GRAY2RGB);
            qInfo() << "[INFO][M] Loaded PGM map via OpenCV:" << QString::fromStdString(path);
        }
        else
        {
            // Load as BGR then convert to RGB (Qt-friendly)
            this->setMapImage(cv::imread(path, cv::IMREAD_COLOR));

            if (this->mapImage_.empty())
            {
                throw std::runtime_error("OpenCV failed to load image");
            }

            cv::cvtColor(this->mapImage_, this->mapImage_, cv::COLOR_BGR2RGB);
            qInfo() << "[INFO][M] Loaded map:" << QString::fromStdString(path);
        }

        this->setMapImagePath(path);
        qInfo() << "[INFO][M] Loaded map from" << QString::fromStdString(path);
        return true;
    }
    catch (const std::exception &e)
    {
        qWarning() << "[ERROR][M] Failed to load image:" << e.what();
        return false;
    }
}

/**
 * @brief Get the stored OpenCV image.
 * @return cv::Mat Shallow copy referencing the internal buffer (OpenCV semantics).
 */
cv::Mat MapModel::getMapImage() const
{
    return this->mapImage_;
}

/**
 * @brief Replace the stored OpenCV image.
 * @param mapImage New image matrix to store.
 */
void MapModel::setMapImage(const cv::Mat &mapImage)
{
    this->mapImage_ = mapImage;
}

/**
 * @brief Create a deep-copied QImage view of the stored OpenCV image.
 *
 * @details
 * Assumes @ref mapImage_ is in RGB888-compatible layout (3 channels, 8-bit).
 * The returned @c QImage is a deep copy and remains valid independently of @ref mapImage_.
 *
 * @return QImage Non-empty on success; empty if no image is stored.
 */
QImage MapModel::copyMapImage2QImage() const
{
    if (this->getMapImage().empty())
    {
        return {};
    }

    const QImage qImage(
        this->getMapImage().data,
        this->getMapImage().cols,
        this->getMapImage().rows,
        this->getMapImage().step,
        QImage::Format_RGB888
    );
    return qImage.copy();
}
