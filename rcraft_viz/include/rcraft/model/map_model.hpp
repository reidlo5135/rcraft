#ifndef RCRAFT_MAP_MODEL_HPP
#define RCRAFT_MAP_MODEL_HPP

/**
 * @file
 * @brief Lightweight data model holding the loaded map image and basic metadata.
 *
 * @details
 * Responsibilities:
 * - Store an OpenCV map image (@c cv::Mat) and its file path.
 * - Keep simple metadata such as resolution and origin.
 * - Provide loading from disk and conversion to @c QImage for Qt display.
 *
 * @note Threading: This class performs no internal synchronization and is intended
 *       to be used from a single thread (typically the UI or a worker thread).
 */

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
    /**
     * @class MapModel
     * @brief Container for a map image and its basic attributes.
     *
     * @details
     * - The stored image @ref mapImage_ is an OpenCV matrix. When loading via
     *   @ref loadMapFromPath, implementations typically ensure it is in a
     *   Qt-friendly RGB layout before generating a @c QImage via
     *   @ref copyMapImage2QImage.
     * - Resolution/origin semantics are application-defined and can be set/get
     *   via @ref setMapResolution, @ref setMapOrigin.
     */
    class MapModel final
    {
    private:
        cv::Mat mapImage_;                 ///< Loaded map image (OpenCV matrix).
        std::string mapImagePath_;         ///< Source file path of the loaded map image.
        float mapResolution_;              ///< Map resolution (application-defined units).
        std::tuple<float, float> mapOrigin_; ///< Map origin (application-defined coordinates).

    public:
        /**
         * @brief Construct a new MapModel with default metadata.
         */
        explicit MapModel();

        /**
         * @brief Destroy the MapModel.
         */
        virtual ~MapModel();

        /**
         * @brief Get the path of the currently loaded map image.
         * @return const std::string& File system path.
         */
        [[nodiscard]] const std::string &getMapImagePath() const;

        /**
         * @brief Set the path of the currently loaded map image.
         * @param mapImagePath New file system path.
         * @note This does not trigger any I/O; use @ref loadMapFromPath to load data.
         */
        void setMapImagePath(const std::string &mapImagePath);

        /**
         * @brief Get the map resolution.
         * @return float Resolution value (application-defined units).
         */
        float getMapResolution() const;

        /**
         * @brief Set the map resolution.
         * @param mapResolution New resolution value.
         */
        void setMapResolution(const float &mapResolution);

        /**
         * @brief Get the map origin.
         * @return std::tuple<float, float> Origin coordinates (application-defined).
         */
        std::tuple<float, float> getMapOrigin() const;

        /**
         * @brief Set the map origin.
         * @param mapOrigin Origin coordinates (application-defined).
         */
        void setMapOrigin(const std::tuple<float, float> &mapOrigin);

        /**
         * @brief Check whether a valid map image is currently stored.
         * @return true if @ref mapImage_ is non-empty; otherwise false.
         */
        bool isMapImageValid() const;

        /**
         * @brief Load a map image from the given file path.
         *
         * @details
         * Implementations typically:
         * - Validate file existence and extension
         * - Load with OpenCV (e.g., GRAYSCALE for PGM, COLOR for typical images)
         * - Convert to an RGB layout suitable for Qt display if needed
         * - Update @ref mapImage_ and @ref mapImagePath_
         *
         * @param path File system path to the map image.
         * @return true on success; false on failure (logs an error).
         */
        bool loadMapFromPath(const std::string &path);

        /**
         * @brief Get the stored OpenCV map image.
         * @return cv::Mat A (shallow-copied) OpenCV matrix referencing the internal data.
         * @note The returned @c cv::Mat follows OpenCV's shared-ownership semantics.
         */
        cv::Mat getMapImage() const;

        /**
         * @brief Replace the stored map image with the provided matrix.
         * @param mapImage New OpenCV matrix to store.
         */
        void setMapImage(const cv::Mat &mapImage);

        /**
         * @brief Create a @c QImage copy of the stored OpenCV map image.
         *
         * @details
         * The returned @c QImage is a deep copy and remains valid even after the internal
         * @ref mapImage_ is modified or goes out of scope.
         *
         * @return QImage Deep-copied @c QImage (empty if no map is loaded).
         */
        QImage copyMapImage2QImage() const;

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<MapModel>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<MapModel>;
    };
}

#endif // RCRAFT_MAP_MODEL_HPP
