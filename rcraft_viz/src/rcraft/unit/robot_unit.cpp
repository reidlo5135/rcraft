#include "rcraft/unit/robot_unit.hpp"

using namespace rcraft::viz;

RobotUnit::RobotUnit(double mapResolution, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    this->robotWidthPx_ = this->kRobotRealWidth / mapResolution;
    this->robotHeightPx_ = this->kRobotRealHeight / mapResolution;
}

RobotUnit::~RobotUnit() = default;

QRectF RobotUnit::boundingRect() const
{
    const int margin = 4;
    return QRectF(
        -this->robotWidthPx_ / 2.0 - margin,
        -this->robotHeightPx_ / 2.0 - this->robotHeightPx_ / 6.0 - margin,
        this->robotWidthPx_ + 2 * margin,
        this->robotHeightPx_ + this->robotHeightPx_ / 3.0 + 2 * margin
    );
}

void RobotUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Body
    QRectF bodyRect(
        -this->robotWidthPx_ / 2.0,
        -this->robotHeightPx_ / 2.0,
        this->robotWidthPx_,
        this->robotHeightPx_
    );
    painter->setBrush(QBrush(QColor(255, 132, 40)));
    painter->setPen(QColor(0, 0, 0));
    painter->drawRect(bodyRect);

    // Left Wheel
    QRectF leftWheelRect(
        -this->robotWidthPx_ / 2.0,
        -this->robotHeightPx_ / 2.0 - this->robotHeightPx_ / 6.0,
        this->robotWidthPx_,
        this->robotHeightPx_ / 6.0
    );
    painter->setBrush(QBrush(QColor(160, 160, 160)));
    painter->drawRect(leftWheelRect);

    // Right Wheel
    QRectF rightWheelRect(
        -this->robotWidthPx_ / 2.0,
        this->robotHeightPx_ / 2.0,
        this->robotWidthPx_,
        this->robotHeightPx_ / 6.0
    );
    painter->drawRect(rightWheelRect);
}