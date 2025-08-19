#ifndef RCRAFT_GOAL_UNIT_HPP
#define RCRAFT_GOAL_UNIT_HPP

#pragma once

/**
 * @file
 * @brief QGraphicsItem representing a goal marker drawn on top of the map scene.
 *
 * @details
 * This item renders a goal icon from an SVG resource. The icon size is derived
 * from the provided map resolution so it scales consistently with the map content.
 * The item's local origin is at the icon center (pos == visual center).
 */

#include <memory>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QSvgRenderer>

static constexpr const char* kGoalSvgPath = ":/rcraft_viz/resource/image/flag-solid-full.svg";

namespace rcraft::viz
{
    class GoalUnit final : public QGraphicsItem
    {
    private:
        std::unique_ptr<QSvgRenderer> svg_;
        QSizeF svg_default_size_{24.0, 24.0};

        double icon_w_{24.0};
        double icon_h_{24.0};

        double fallback_radius_{6.0};

    protected:
        QRectF boundingRect() const override;
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

    public:
        explicit GoalUnit(double mapResolution = 1.0, QGraphicsItem *parent = nullptr);
        ~GoalUnit() override;

    public:
        using SharedPtr = std::shared_ptr<GoalUnit>;
        using UniquePtr = std::unique_ptr<GoalUnit>;
    };
}

#endif // RCRAFT_GOAL_UNIT_HPP
