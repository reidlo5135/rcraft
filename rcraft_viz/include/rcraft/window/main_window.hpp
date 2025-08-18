#ifndef RCRAFT_MAIN_WINDOW_HPP
#define RCRAFT_MAIN_WINDOW_HPP

#pragma once

/**
 * @file
 * @brief Top-level application window that hosts the OverlayView.
 *
 * @details
 * Responsibilities:
 * - Create and own an @c OverlayView instance
 * - Act as the main Qt window (menus/toolbars/status bar may be added later)
 * - Configure basic window properties (icon, title, size) in the constructor
 */

#include <memory>
#include <QMainWindow>
#include <QIcon>
#include <QDebug>

#include "rcraft/view/overlay_view.hpp"

namespace rcraft::viz
{
    class OverlayView;

    /**
     * @class MainWindow
     * @brief The application's primary window embedding an @ref OverlayView.
     *
     * @details
     * Typical lifecycle:
     * - Construct: allocate @ref overlayView_ and set it as the central widget
     * - (Optional) Configure window icon/title/geometry
     * - Show the window from @c main()
     *
     * Threading:
     * - Lives on the GUI thread; no special synchronization is performed.
     */
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT
    private:
        /** @brief The central composite widget presenting the map/robot/path UI. */
        OverlayView::UniquePtr overlayView_;

    public:
        /**
         * @brief Construct the main window and install the central @ref OverlayView.
         * @param parent Optional Qt parent.
         *
         * @details
         * The constructor typically:
         * - Allocates @ref overlayView_
         * - Calls @c setCentralWidget(overlayView_.get())
         * - Optionally sets window title, icon, and initial size
         */
        explicit MainWindow(QWidget *parent = nullptr);

        /**
         * @brief Destroy the main window.
         *
         * @details
         * The unique_ptr cleans up @ref overlayView_. Qt handles QObject-based children.
         */
        ~MainWindow() override;

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<MainWindow>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<MainWindow>;
    };
}

#endif // RCRAFT_MAIN_WINDOW_HPP
