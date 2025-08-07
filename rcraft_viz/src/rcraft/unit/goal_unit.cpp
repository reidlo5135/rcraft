#include "rcraft/unit/goal_unit.hpp"

using namespace rcraft::viz;

GoalUnit::GoalUnit(double mapResolution, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    this->radius_ = 6 * mapResolution;
}

GoalUnit::~GoalUnit() = default;

QRectF GoalUnit::boundingRect() const
{
    return QRectF(
        this->radius_ - 2, -this->radius_ - 2,
        (this->radius_ + 2) * 2, (this->radius_ + 2) * 2
        );
}

void GoalUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen *qPen = new QPen(QColor("black"));
    qPen->setWidth(2);
    painter->setPen(*qPen);
    painter->setBrush(QBrush(QColor("#0096ff")));
    painter->drawEllipse(QPointF(0, 0), this->radius_, this->radius_);

    delete qPen;
}
