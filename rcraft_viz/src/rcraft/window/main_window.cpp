#include "rcraft/window/main_window.hpp"

using namespace rcraft::viz;

/**
 * @file
 * @brief Implementation of the application's top-level QMainWindow.
 */

/**
 * @brief Construct the main window and install the central OverlayView.
 *
 * @param parent Optional Qt parent widget.
 *
 * @details
 * - Sets a window title and initial geometry (1920x1080 at (100,100)).
 * - Allocates an @c OverlayView (parented to this) and assigns it as the central widget.
 * - Additional UI elements (menus/toolbars/status bar) may be added later.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("rcraft");
    setGeometry(100, 100, 1920, 1080);

    this->overlayView_ = std::make_unique<OverlayView>(this);
    setCentralWidget(this->overlayView_.get());
}

/**
 * @brief Destroy the main window (default).
 *
 * @details
 * The unique_ptr releases the @ref overlayView_. QObject-based children are
 * also cleaned up by Qt's parent–child ownership model.
 */
MainWindow::~MainWindow() = default;
