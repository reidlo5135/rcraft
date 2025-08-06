#include "rcraft/window/main_window.hpp"

using namespace rcraft::viz;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("rcraft");
    setGeometry(100, 100, 1920, 1080);
    setWindowTitle("rcraft");

    this->overlayView_ = std::make_unique<OverlayView>(this);
    setCentralWidget(this->overlayView_.get());
}

MainWindow::~MainWindow() = default;
