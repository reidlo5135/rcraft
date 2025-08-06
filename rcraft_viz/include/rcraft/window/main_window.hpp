#ifndef RCRAFT_MAIN_WINDOW_HPP
#define RCRAFT_MAIN_WINDOW_HPP

#include <memory>
#include <QMainWindow>
#include <QIcon>
#include <QDebug>

#include "rcraft/view/overlay_view.hpp"

namespace rcraft::viz
{
    class OverlayView;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    private:
        OverlayView::UniquePtr overlayView_;

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        virtual ~MainWindow() override;

    public:
        using SharedPtr = std::shared_ptr<MainWindow>;
        using UniquePtr = std::unique_ptr<MainWindow>;
    };
}

#endif // RCRAFT_MAIN_WINDOW_HPP
