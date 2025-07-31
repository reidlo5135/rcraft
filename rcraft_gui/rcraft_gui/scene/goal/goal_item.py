from PyQt5.QtCore import QRectF, QPointF
from PyQt5.QtGui import QColor, QBrush, QPen, QPainter
from PyQt5.QtWidgets import QGraphicsItem, QStyleOptionGraphicsItem, QWidget

class GoalItem(QGraphicsItem):

    def __init__(self, mapResolution: float = 1.0, parent=None) -> None:
        super().__init__(parent=parent)
        self.radius: float = 6 * mapResolution

    def boundingRect(self) -> QRectF:
        return QRectF(
            -self.radius - 2, -self.radius - 2,
            (self.radius + 2) * 2, (self.radius + 2) * 2
        )

    def paint(self, painter: QPainter, option: QStyleOptionGraphicsItem, widget: QWidget | None = None) -> None:
        pen: QPen = QPen(QColor("black"))
        pen.setWidth(2)
        painter.setPen(pen)
        painter.setBrush(QBrush(QColor("#0096ff")))
        painter.drawEllipse(QPointF(0, 0), self.radius, self.radius)

__all__ = ["GoalItem"]
