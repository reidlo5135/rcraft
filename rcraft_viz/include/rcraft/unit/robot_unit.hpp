#ifndef RCRAFT_ROBOT_UNIT_HPP
#define RCRAFT_ROBOT_UNIT_HPP

#pragma once

/**
 * @file
 * @brief QGraphicsItem that visualizes the robot footprint/heading on the map scene.
 *
 * @details
 * The item draws a simple rectangle (body) with an orientation indicator. The on-screen
 * size in pixels is derived from a real-world footprint using the provided map resolution.
 * Use @c setPos(x,y) to place the robot in scene coordinates and @c setRotation(deg) to
 * indicate its heading.
 */

#include <memory>

#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QColor>

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
    class RobotUnit : public QGraphicsItem
    {
    private:
        /** @brief Robot width in scene pixels (computed from map resolution). */
        double robotWidthPx_{0.0};
        /** @brief Robot height in scene pixels (computed from map resolution). */
        double robotHeightPx_{0.0};

        /** @brief Real robot width in map units (e.g., centimeters). */
        static constexpr double kRobotRealWidth  = 20.0;
        /** @brief Real robot height in map units (e.g., centimeters). */
        static constexpr double kRobotRealHeight = 10.0;

    public:
        /**
         * @brief Construct a RobotUnit with footprint scaled by map resolution.
         * @param mapResolution Pixels per one real unit (same unit as kRobotReal* constants).
         * @param parent Optional QGraphicsItem parent.
         *
         * @details
         * The constructor computes @ref robotWidthPx_ and @ref robotHeightPx_ as:
         * @code
         * robotWidthPx_  = kRobotRealWidth  * mapResolution;
         * robotHeightPx_ = kRobotRealHeight * mapResolution;
         * @endcode
         */
        explicit RobotUnit(double mapResolution, QGraphicsItem *parent = nullptr);

        /**
         * @brief Destroy the RobotUnit.
         */
        ~RobotUnit() override;

        /**
         * @brief Returns the local bounding rectangle of the item.
         *
         * @details
         * The rect fully contains the painted footprint (with a small pen margin).
         * It is centered at the local origin (0,0) so rotation occurs about the center.
         */
        QRectF boundingRect() const override;

        /**
         * @brief Paint the robot footprint and heading indicator.
         *
         * @param painter Painter supplied by the view.
         * @param option  Style options (unused).
         * @param widget  Optional widget (unused).
         *
         * @details
         * Default implementation:
         * - Draws a filled body rectangle with a contrasting outline
         * - Draws a front-facing marker (e.g., a short line/triangle) to indicate heading
         * For consistent on-screen thickness under zoom, a cosmetic pen may be used.
         */
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<RobotUnit>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<RobotUnit>;
    };
}

#endif // RCRAFT_ROBOT_UNIT_HPP
