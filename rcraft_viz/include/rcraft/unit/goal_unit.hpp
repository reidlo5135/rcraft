#ifndef RCRAFT_GOAL_UNIT_HPP
#define RCRAFT_GOAL_UNIT_HPP

#pragma once

#include <memory>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QColor>

namespace rcraft::viz
{
    class GoalUnit : public QGraphicsItem
    {
    private:
        double radius_;

    public:
        explicit GoalUnit(double mapResolution = 1.0, QGraphicsItem *parent = nullptr);
        virtual ~GoalUnit() override;

        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    public:
        using SharedPtr = std::shared_ptr<GoalUnit>;
        using UniquePtr = std::unique_ptr<GoalUnit>;
    };
}

#endif // RCRAFT_GOAL_UNIT_HPP
