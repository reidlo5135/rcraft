from PyQt5.QtCore import QRectF
from PyQt5.QtGui import QColor, QBrush, QPainter
from PyQt5.QtWidgets import QGraphicsItem, QStyleOptionGraphicsItem, QWidget

ROBOT_REAL_WIDTH: float = 20.0
ROBOT_REAL_HEIGHT: float = 10.0

class RobotItem(QGraphicsItem):

    def __init__(self, mapResolution: float, parent=None) -> None:
        super().__init__(parent=parent)
        self.robotWidthPx: float = ROBOT_REAL_WIDTH / mapResolution
        self.robotHeightPx: float = ROBOT_REAL_HEIGHT / mapResolution

    def boundingRect(self) -> QRectF:
        margin: int = 4

        return QRectF(
            -self.robotWidthPx / 2 - margin,
            -self.robotHeightPx / 2 - self.robotHeightPx / 6 - margin,
            self.robotWidthPx + 2 * margin,
            self.robotHeightPx + self.robotHeightPx / 3 + 2 * margin
        )

    def paint(self, painter: QPainter, option: QStyleOptionGraphicsItem, widget: QWidget | None = None) -> None:
        bodyRect: QRectF = QRectF(
            -self.robotWidthPx / 2,
            -self.robotHeightPx / 2,
            self.robotWidthPx,
            self.robotHeightPx
        )
        painter.setBrush(QBrush(QColor(255, 132, 40)))
        painter.setPen(QColor(0,0,0))
        painter.drawRect(bodyRect)

        leftWheelRect: QRectF = QRectF(
            -self.robotWidthPx / 2,
            -self.robotHeightPx / 2 - self.robotHeightPx / 6,
            self.robotWidthPx,
            self.robotHeightPx / 6
        )
        painter.setBrush(QBrush(QColor(160, 160, 160)))
        painter.setPen(QColor(0,0,0))
        painter.drawRect(leftWheelRect)

        rightWheelRect: QRectF = QRectF(
            -self.robotWidthPx / 2,
            self.robotHeightPx / 2,
            self.robotWidthPx,
            self.robotHeightPx / 6
        )
        painter.setBrush(QBrush(QColor(160, 160, 160)))
        painter.setPen(QColor(0,0,0))
        painter.drawRect(rightWheelRect)


__all__: list[str] = ["RobotItem"]