#ifndef RCRAFT_ROBOT_UNIT_HPP
#define RCRAFT_ROBOT_UNIT_HPP

#pragma once

#include <memory>

#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QColor>

namespace rcraft::viz
{
    class RobotUnit : public QGraphicsItem
    {
    private:
        double robotWidthPx_;
        double robotHeightPx_;
        static constexpr double kRobotRealWidth = 20.0;
        static constexpr double kRobotRealHeight = 10.0;

    public:
        explicit RobotUnit(double mapResolution, QGraphicsItem *parent = nullptr);
        virtual ~RobotUnit() override;

        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    public:
        using SharedPtr = std::shared_ptr<RobotUnit>;
        using UniquePtr = std::unique_ptr<RobotUnit>;
    };
}

#endif // RCRAFT_ROBOT_UNIT_HPP
