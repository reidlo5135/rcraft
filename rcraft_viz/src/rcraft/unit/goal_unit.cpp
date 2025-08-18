#include "rcraft/unit/goal_unit.hpp"

using namespace rcraft::viz;

/**
 * @file
 * @brief Implementation of GoalUnit, a simple circular goal marker for the map scene.
 */

/**
 * @brief Construct a new GoalUnit.
 *
 * @param mapResolution Map resolution/scaling factor used to derive a reasonable marker radius.
 * @param parent Optional QGraphicsItem parent.
 *
 * @details
 * The visual radius is set to @c 6 * mapResolution so the marker scales with the map.
 * The item's local origin is at (0,0); callers typically position it via @c setPos(x,y).
 */
GoalUnit::GoalUnit(double mapResolution, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , radius_(6 * mapResolution)
{
}

/**
 * @brief Destroy the GoalUnit (default).
 */
GoalUnit::~GoalUnit() = default;

/**
 * @brief Return the item's logical bounding rectangle in local coordinates.
 *
 * @details
 * The rectangle must fully contain all painting done in @ref paint().
 * It is centered around the local origin with a small padding (2 px) to
 * account for the pen width.
 *
 * @return QRectF Bounding rectangle used by the scene for culling/updates.
 */
QRectF GoalUnit::boundingRect() const
{
    // NOTE: If you want it perfectly centered around (0,0), an alternative is:
    // return QRectF(-(radius_ + 2), -(radius_ + 2), 2*(radius_ + 2), 2*(radius_ + 2));
    return QRectF(
        this->radius_ - 2, -this->radius_ - 2,
        (this->radius_ + 2) * 2, (this->radius_ + 2) * 2
    );
}

/**
 * @brief Paint the goal marker.
 *
 * @param painter Painter provided by the view.
 * @param option  Style options (unused).
 * @param widget  Optional widget (unused).
 *
 * @details
 * Draws a filled circle with a black outline and a cyan fill. You can customize
 * colors, pen width, and styling as needed. For best on-screen readability across
 * zoom levels, consider using a cosmetic pen (uncomment below).
 */
void GoalUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(QColor("black"));
    pen.setWidth(2);
    // pen.setCosmetic(true); // Uncomment to keep outline thickness constant w.r.t. view zoom
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor("#0096ff"))); // cyan-ish fill
    painter->drawEllipse(QPointF(0, 0), this->radius_, this->radius_);
}
