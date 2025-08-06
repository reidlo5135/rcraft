#include <QApplication>
#include "rcraft/window/main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    rcraft::viz::MainWindow::UniquePtr window = std::make_unique<rcraft::viz::MainWindow>();
    window->show();
    return app.exec();
}