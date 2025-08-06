#ifndef RCRAFT_MAP_VIEW_HPP
#define RCRAFT_MAP_VIEW_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QPen>
#include <QBrush>
#include <QImage>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <memory>
#include <QPixmap>
#include <QDebug>
#include <QGraphicsRectItem>

#include "rcraft/view_model/map_view_model.hpp"

namespace rcraft::viz
{
    class GoalItem;
    class RobotItem;

    class MapView : public QGraphicsView
    {
        Q_OBJECT
    private:
        void SetScene();
        void UpdateButtonPosition();

        MapViewModel *viewModel_;
        QGraphicsScene *scene_ = nullptr;
        QGraphicsPixmapItem *pixmapItem_ = nullptr;
        QPushButton *mapLoadButton_ = nullptr;
        float mapResolution_ = 1.0f;
        bool mapLoaded_ = false;
        QSize mapSize_ = {0, 0};

        RobotItem *robotItem_ = nullptr;
        GoalItem *goalItem_ = nullptr;
        QPoint goalItemPos_ = {0, 0};

    protected:
        void resizeEvent(QResizeEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

    public:
        explicit MapView(MapViewModel *viewModel, QWidget *parent = nullptr);
        virtual ~MapView() override;

        void OnResolutionChanged(float resolution);
        void OnMapUpdated(const QImage &qImage);

    public:
        using SharedPtr = std::shared_ptr<MapView>;
        using UniquePtr = std::unique_ptr<MapView>;

    };
}

#endif // RCRAFT_MAP_VIEW_HPP
