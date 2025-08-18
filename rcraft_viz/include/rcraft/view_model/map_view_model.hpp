#ifndef RCRAFT_MAP_VIEW_MODEL_HPP
#define RCRAFT_MAP_VIEW_MODEL_HPP

#pragma once

/**
 * @file
 * @brief View-model that mediates between the UI (MapView) and the underlying map data (MapModel).
 *
 * Responsibilities:
 * - Hold the current map file path and a @c MapModel instance
 * - Provide a handler to select/load a map image via a file dialog
 * - Emit UI-facing signals when the map image or resolution changes
 */

#include <memory>
#include <QString>
#include <QObject>
#include <QImage>
#include <QFileDialog>
#include <QApplication>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "rcraft/model/map_model.hpp"

/**
 * @def MAP_DEFAULT_WIN_PATH
 * @brief Default directory used on Windows for opening the map selection dialog.
 *
 * @note Adjust this path to match your deployment environment.
 */
#define MAP_DEFAULT_WIN_PATH "C:/mn_ws/rcraft/rcraft_map_server/maps"

namespace rcraft::viz
{
    /**
     * @class MapViewModel
     * @brief Lightweight view-model for map-related state and actions.
     *
     * @details
     * This class owns a @c MapModel instance and exposes a small API to:
     * - Get/Set the current map path
     * - Launch a file dialog to select a map (see @ref handleMapSelect)
     * - Emit signals with the loaded map image and resolution so that the view can update
     *
     * Threading:
     * - Designed to live on the GUI thread. Signals are emitted from the same thread unless
     *   otherwise connected with @c Qt::QueuedConnection.
     */
    class MapViewModel final : public QObject
    {
        Q_OBJECT

    private:
        /** @brief Underlying model storing the OpenCV map image and metadata. */
        MapModel::UniquePtr model_;

        /** @brief Currently selected map file path. */
        QString mapPath_;

    public:
        /**
         * @brief Construct a new MapViewModel.
         * @param parent Optional QObject parent.
         */
        explicit MapViewModel(QObject *parent = nullptr);

        /**
         * @brief Destroy the MapViewModel.
         */
        ~MapViewModel() override;

        /**
         * @brief Get the currently selected map path.
         * @return Map file path as a QString (may be empty if not set).
         */
        QString getMapPath() const;

        /**
         * @brief Update the map path (does not load the file by itself).
         * @param mapPath New map file path.
         *
         * @note Typically followed by a call that attempts to load and emit @ref mapUpdated.
         */
        void setMapPath(const QString &mapPath);

        /**
         * @brief Open a file dialog to select a map image and load it via the model.
         *
         * @details
         * - On success, emits @ref mapUpdated with a @c QImage copy of the loaded OpenCV image.
         * - May also emit @ref resolutionChanged if the resolution is updated in the model.
         * - Updates @ref mapPath_ to the chosen file path.
         *
         * @note Uses @ref MAP_DEFAULT_WIN_PATH as the initial directory on Windows.
         */
        void handleMapSelect();

    signals:
        /**
         * @brief Emitted when a map image has been successfully loaded.
         * @param image The loaded map as a @c QImage (RGB888 expected).
         */
        void mapUpdated(const QImage& image);

        /**
         * @brief Emitted when the map resolution changes.
         * @param resolution New resolution value (application-defined semantics).
         */
        void resolutionChanged(float resolution);

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<MapViewModel>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<MapViewModel>;
    };
}

#endif // RCRAFT_MAP_VIEW_MODEL_HPP
