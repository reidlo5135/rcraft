/**
* @file
 * @brief Application entry point for the rcraft visualization tool.
 *
 * @details
 * Creates a Qt application, constructs the top-level @c MainWindow, shows it,
 * and enters the Qt event loop.
 */

#include <QApplication>
#include "rcraft/window/main_window.hpp"

/**
 * @brief Main entry point.
 *
 * @param argc Argument count from the host process.
 * @param argv Argument vector from the host process.
 * @return int Process exit code (Qt event loop result).
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create the main window and display it.
    rcraft::viz::MainWindow::UniquePtr window =
        std::make_unique<rcraft::viz::MainWindow>();
    window->show();

    // Enter Qt's event loop.
    return app.exec();
}
