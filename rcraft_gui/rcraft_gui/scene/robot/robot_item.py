from PyQt5.QtCore import QSize
from PyQt5.QtWidgets import QGraphicsEllipseItem
from PyQt5.QtGui import QBrush, QColor

class RobotItem(QGraphicsEllipseItem):
    def __init__(self):
        super().__init__()
        self.setBrush(QBrush(QColor(255, 0, 0)))

    def set_size(self, size) -> None:
        self.setRect(-size/2, -size/2, size, size)

    def move_to(self, x: float, y: float):
        self.setPos(x, y)
