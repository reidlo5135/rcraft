#include "rcraft/unit/goal_unit.hpp"

using namespace rcraft::viz;

/**
 * @brief Construct a new GoalUnit.
 * @param mapResolution Map resolution used to derive a reasonable icon size.
 *                      Previously radius_ = 6*mapResolution, so icon height ~= 12*mapResolution.
 * @param parent Optional graphics parent.
 */
GoalUnit::GoalUnit(double mapResolution, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    // 1) Load SVG
    this->svg_ = std::make_unique<QSvgRenderer>(QString::fromUtf8(kGoalSvgPath));

    if (this->svg_ && this->svg_->isValid())
    {
        this->svg_default_size_ = this->svg_->defaultSize();

        if (this->svg_default_size_.isEmpty())
        {
            this->svg_default_size_ = QSizeF(24.0, 24.0);
        }
    }
    else
    {
        // Fallback: simple dot
        this->svg_.reset();
    }

    // 2) Compute target size in scene units
    //    Keep the previous visual scale: height ~= 12 * mapResolution
    this->icon_h_ = std::max(12.0 * mapResolution, 8.0); // 최소 8 유닛 보장
    const double aspect = this->svg_default_size_.height() > 0.0 ? (this->svg_default_size_.width() / this->svg_default_size_.height()) : 1.0;
    this->icon_w_ = this->icon_h_ * aspect;

    // Fallback circle radius (roughly half height)
    this->fallback_radius_ = 0.5 * this->icon_h_;

    // Cache for better performance
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

GoalUnit::~GoalUnit() = default;

QRectF
GoalUnit::boundingRect() const
{
    // Origin(0,0)을 아이콘 "중앙"으로 유지 (pos == center)
    // => 좌상단 (-w/2, -h/2), 크기 (w, h)
    return QRectF(-this->icon_w_ * 0.5, -this->icon_h_ * 0.5, this->icon_w_, this->icon_h_);
}

void
GoalUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (this->svg_)
    {
        // 렌더 타깃을 boundingRect로 지정하면 알아서 맞춰 그림
        this->svg_->render(painter, boundingRect());
        return;
    }

    // Fallback: 단색 원
    QPen pen(Qt::black);
    pen.setWidthF(2.0);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor("#0096ff")));
    painter->drawEllipse(QPointF(0, 0), this->fallback_radius_, this->fallback_radius_);
}
