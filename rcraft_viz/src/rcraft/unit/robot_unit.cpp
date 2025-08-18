#include "rcraft/unit/robot_unit.hpp"

using namespace rcraft::viz;

/**
 * @file
 * @brief Implementation of RobotUnit, a QGraphicsItem that renders the robot footprint and wheels.
 *
 * @details
 * Coordinate & scale convention in this implementation:
 * - @p mapResolution is interpreted as **real-units-per-pixel** (e.g., cm/pixel).
 * - Pixel size is computed as: @c pixel = real_size / mapResolution.
 *   Hence:
 *   @code
 *   robotWidthPx_  = kRobotRealWidth  / mapResolution;
 *   robotHeightPx_ = kRobotRealHeight / mapResolution;
 *   @endcode
 * If your project uses the opposite convention (pixels-per-real-unit), adjust the math accordingly.
 */

/**
 * @brief Construct a RobotUnit and derive footprint size in pixels from map resolution.
 *
 * @param mapResolution Real-units-per-pixel scale (e.g., centimeters per pixel).
 * @param parent Optional graphics parent item.
 *
 * @details
 * The robot footprint is modeled as a body rectangle plus two “wheel” strips.
 * Dimensions in pixels are computed from the real-world constants:
 * - @ref kRobotRealWidth
 * - @ref kRobotRealHeight
 */
RobotUnit::RobotUnit(double mapResolution, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    this->robotWidthPx_  = this->kRobotRealWidth  / mapResolution;
    this->robotHeightPx_ = this->kRobotRealHeight / mapResolution;
}

/**
 * @brief Destroy the RobotUnit (default).
 */
RobotUnit::~RobotUnit() = default;

/**
 * @brief Return the local bounding rectangle used for scene culling and updates.
 *
 * @details
 * The bounding rect fully contains the painted body and wheel strips, with an extra
 * margin to account for the outline pen. It is centered around the local origin so
 * that rotations occur about the center of the body rectangle.
 *
 * @return QRectF Local-space bounding rectangle.
 */
QRectF RobotUnit::boundingRect() const
{
    constexpr int margin = 4;
    return QRectF(
        -this->robotWidthPx_ / 2.0 - margin,
        -this->robotHeightPx_ / 2.0 - this->robotHeightPx_ / 6.0 - margin,
        this->robotWidthPx_ + 2 * margin,
        this->robotHeightPx_ + this->robotHeightPx_ / 3.0 + 2 * margin
    );
}

/**
 * @brief Paint the robot body and wheels.
 *
 * @param painter Painter provided by the view.
 * @param option  Style options (unused).
 * @param widget  Optional widget (unused).
 *
 * @details
 * The visual consists of:
 * - A filled body rectangle (orange with black outline)
 * - Two wheel strips (gray) above and below the body
 *
 * Tips:
 * - To keep line thickness constant when zooming the view, you may use a cosmetic pen:
 *   @code
 *   QPen pen(Qt::black);
 *   pen.setWidthF(1.5);
 *   pen.setCosmetic(true);
 *   painter->setPen(pen);
 *   @endcode
 */
void RobotUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Body rectangle (centered at origin)
    QRectF bodyRect(
        -this->robotWidthPx_ / 2.0,
        -this->robotHeightPx_ / 2.0,
        this->robotWidthPx_,
        this->robotHeightPx_
    );
    painter->setBrush(QBrush(QColor(255, 132, 40))); // orange
    painter->setPen(QColor(0, 0, 0));                // black outline
    painter->drawRect(bodyRect);

    // Upper wheel strip
    QRectF leftWheelRect(
        -this->robotWidthPx_ / 2.0,
        -this->robotHeightPx_ / 2.0 - this->robotHeightPx_ / 6.0,
        this->robotWidthPx_,
        this->robotHeightPx_ / 6.0
    );
    painter->setBrush(QBrush(QColor(160, 160, 160))); // gray
    painter->drawRect(leftWheelRect);

    // Lower wheel strip
    QRectF rightWheelRect(
        -this->robotWidthPx_ / 2.0,
        this->robotHeightPx_ / 2.0,
        this->robotWidthPx_,
        this->robotHeightPx_ / 6.0
    );
    painter->drawRect(rightWheelRect);
}
