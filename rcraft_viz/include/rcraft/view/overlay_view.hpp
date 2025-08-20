#ifndef RCRAFT_OVERLAY_VIEW_HPP
#define RCRAFT_OVERLAY_VIEW_HPP

#pragma once

/**
 * @file
 * @brief Top-level container widget that hosts the map view and wires it to its view-model.
 *
 * @details
 * Responsibilities:
 * - Create and own a @c MapViewModel and a @c MapView
 * - Arrange the view inside a simple layout
 * - Apply initial screen-related sizing/position via @ref setScreen()
 * - Connect UI actions/signals between view and view-model in @ref setQConnected()
 */

#include <memory>
#include <QGuiApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QDebug>
#include <QWidget>

#include "rcraft/view/map_view.hpp"
#include "rcraft/view_model/map_view_model.hpp"

namespace rcraft::viz
{
    /**
     * @class OverlayView
     * @brief Composite widget that displays the interactive map view.
     *
     * @details
     * Typical lifecycle:
     * - Construct: allocates @ref mapViewModel_ and @ref mapView_
     * - @ref setDefaultLayout : place @c MapView into a @c QVBoxLayout
     * - @ref setQConnected : wire model/view signals
     * - @ref setScreen : optional full-screen–aware geometry hinting
     *
     * Threading:
     * - This widget and its children are intended to live on the GUI thread.
     */
    class OverlayView final : public QWidget
    {
        Q_OBJECT

    private:
        /** @brief View-model that owns the map data and exposes UI-facing signals. */
        std::unique_ptr<MapViewModel> mapViewModel_;

        /** @brief The interactive graphics view that renders the map/robot/goal/path. */
        std::unique_ptr<MapView> mapView_;

        /**
         * @brief Apply screen-related defaults (e.g., initial geometry).
         *
         * @details
         * Queries the primary screen via @c QGuiApplication::primaryScreen() and may
         * size or position the window accordingly. Safe to call multiple times.
         */
        void setScreen();

        /**
         * @brief Connect signals/slots between the view and the view-model.
         *
         * @details
         * Wires actions like "Load Map" to the model's handler and ensures map update
         * signals propagate to the view. Intended to be invoked during construction.
         */
        void setQConnection();

        /**
         * @brief Install a simple layout and insert the @ref mapView_ widget.
         *
         * @details
         * Creates a @c QVBoxLayout owned by this widget and adds @ref mapView_ with
         * appropriate margins/stretch so it fills the available space.
         */
        void setDefaultLayout();

    protected:
        /**
         * @brief React to widget resizes.
         * @param event Qt resize event.
         *
         * @details
         * Forwards to the base class after any container-specific adjustments.
         * The embedded @ref MapView will manage its own fit-to-scene behavior.
         */
        void resizeEvent(QResizeEvent *event) override;

    public:
        /**
         * @brief Construct a new OverlayView.
         * @param parent Optional Qt parent.
         *
         * @details
         * Allocates the view-model and view, installs the layout, performs signal
         * connections, and applies initial screen sizing via @ref setScreen().
         */
        explicit OverlayView(QWidget *parent = nullptr);

        /**
         * @brief Destroy the OverlayView and its owned children.
         */
        ~OverlayView() override;

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<OverlayView>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<OverlayView>;
    };
}

#endif // RCRAFT_OVERLAY_VIEW_HPP
