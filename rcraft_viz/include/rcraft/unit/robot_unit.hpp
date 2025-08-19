#ifndef RCRAFT_ROBOT_UNIT_HPP
#define RCRAFT_ROBOT_UNIT_HPP

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

static constexpr const char* kRobotSvgPath = ":/rcraft_viz/resource/image/location-arrow-solid.svg";

namespace rcraft::viz
{
    /**
     * @class RobotUnit
     * @brief Lightweight graphics item for rendering the robot footprint and heading.
     *
     * @details
     * - The footprint is represented by a rectangle of size
     *   @ref kRobotRealWidth × @ref kRobotRealHeight (in real units), scaled by
     *   the constructor's @p mapResolution to compute pixel dimensions.
     * - The local item origin is the rectangle center; rotation is applied around
     *   this origin (clockwise degrees as in Qt's @c setRotation()).
     * - Z-order should be configured by the caller (e.g., above the map pixmap).
     */
    class RobotUnit final : public QGraphicsItem
    {
    private:
        std::unique_ptr<QSvgRenderer> svg_;
        QSizeF svg_default_size_{44.0, 44.0};

        double icon_w_{44.0};
        double icon_h_{44.0};

        double fallback_radius_{6.0};

    protected:
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    public:
        explicit RobotUnit(double mapResolution = 1.0, QGraphicsItem *parent = nullptr);
        ~RobotUnit() override;

    public:
        using SharedPtr = std::shared_ptr<RobotUnit>;
        using UniquePtr = std::unique_ptr<RobotUnit>;
    };;
}

#endif // RCRAFT_ROBOT_UNIT_HPP
