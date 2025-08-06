#ifndef RCRAFT_OVERLAY_VIEW_HPP
#define RCRAFT_OVERLAY_VIEW_HPP

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
    class OverlayView : public QWidget
    {
        Q_OBJECT
    private:
        std::unique_ptr<MapViewModel> mapViewModel_;
        std::unique_ptr<MapView> mapView_;

    protected:
        void resizeEvent(QResizeEvent *event) override;

    public:
        explicit OverlayView(QWidget *parent = nullptr);
        virtual ~OverlayView() override;

    public:
        using SharedPtr = std::shared_ptr<OverlayView>;
        using UniquePtr = std::unique_ptr<OverlayView>;

    };
}

#endif // RCRAFT_OVERLAY_VIEW_HPP
