#ifndef RCRAFT_GOAL_UNIT_HPP
#define RCRAFT_GOAL_UNIT_HPP

#pragma once

/**
 * @file
 * @brief QGraphicsItem representing a goal marker drawn on top of the map scene.
 *
 * @details
 * The goal is rendered as a simple circular marker (ring/dot style configurable
 * in @ref paint). Its size is derived from the provided map resolution so it
 * scales consistently with the map content.
 *
 * Typical usage:
 * @code
 * auto goal = std::make_shared<rcraft::viz::GoalUnit>(mapResolution);
 * goal->setPos(goalX, goalY);
 * goal->setZValue(10);           // ensure it draws above the path/map
 * scene->addItem(goal.get());
 * @endcode
 */

#include <memory>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QColor>

namespace rcraft::viz
{
    /**
     * @class GoalUnit
     * @brief Lightweight graphics item that visualizes the goal position.
     *
     * @details
     * - The visual footprint is defined by @ref radius_ returned via @ref boundingRect().
     * - The item is non-interactive by default (no mouse handling); interaction is handled
     *   in the owning view/controller.
     * - Z-order should be set by the caller to avoid being occluded by the path/pixmap.
     */
    class GoalUnit final : public QGraphicsItem
    {
    private:
        /** @brief Marker radius in scene units; derived from the map resolution. */
        double radius_;

    public:
        /**
         * @brief Construct a new GoalUnit.
         * @param mapResolution Resolution/scaling factor used to derive a reasonable radius.
         * @param parent Optional QGraphicsItem parent.
         *
         * @note The caller should set position via @c setPos(x,y) and Z via @c setZValue().
         */
        explicit GoalUnit(double mapResolution = 1.0, QGraphicsItem *parent = nullptr);

        /**
         * @brief Destroy the GoalUnit.
         */
        ~GoalUnit() override;

        /**
         * @brief Return the item's logical bounding rectangle in local coordinates.
         *
         * @details
         * The returned rect must fully contain all painting performed in @ref paint.
         * It is typically a square of side @c 2*radius_ centered at (0,0).
         */
        QRectF boundingRect() const override;

        /**
         * @brief Paint the goal marker.
         *
         * @param painter Painter provided by the view.
         * @param option Style options (unused).
         * @param widget Optional widget (unused).
         *
         * @details
         * The default implementation draws a ring (stroke) and a small filled center
         * dot using colors that stand out against typical map imagery. Customize as needed.
         */
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;

    public:
        /// @brief Shared pointer alias.
        using SharedPtr  = std::shared_ptr<GoalUnit>;
        /// @brief Unique pointer alias.
        using UniquePtr  = std::unique_ptr<GoalUnit>;
    };
}

#endif // RCRAFT_GOAL_UNIT_HPP
