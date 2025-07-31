from PyQt5.QtWidgets import (
    QGraphicsView, QGraphicsScene, QGraphicsPixmapItem,
    QSizePolicy, QPushButton
)
from PyQt5.QtGui import QPixmap, QPainter, QImage, QResizeEvent, QMouseEvent, QPen, QBrush
from PyQt5.QtCore import Qt, QPointF, QRectF

from rcraft_gui.scene.goal.goal_item import GoalItem
from rcraft_gui.scene.robot.robot_item import RobotItem
from rcraft_gui.viewmodel.map.map_viewmodel import MapViewModel


class MapView(QGraphicsView):

    def __init__(self, viewmodel: MapViewModel, parent=None) -> None:
        super().__init__(parent=parent)
        self.viewmodel: MapViewModel = viewmodel
        self._setScene()

        self.pixMapItem: QGraphicsPixmapItem | None = None

        self.mapLoadButton = QPushButton("Load Map", self)
        self.mapLoadButton.setFixedSize(120, 30)
        self.mapLoadButton.setStyleSheet("background-color: red; color: white;")
        self.mapLoadButton.clicked.connect(self.viewmodel.handleMapSelect)

        self.mapResolution: float = 1.0
        self.mapLoaded: bool = False
        self.mapSize: tuple[int, int] = (0, 0)

        self.robotItem: RobotItem = RobotItem(self.mapResolution)
        self.robotItem.setRotation(270)
        self.robotItem.setPos(0, 0)
        self.robotItem.setZValue(1)
        self.scene.addItem(self.robotItem)

        self.goalItem: GoalItem | None = None
        self.goalItemPos: tuple[int, int] = (0, 0)

        self.setMouseTracking(True)


    def _setScene(self) -> None:
        self.scene: QGraphicsScene = QGraphicsScene()
        self.setScene(self.scene)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setRenderHints(QPainter.Antialiasing | QPainter.SmoothPixmapTransform)
        self.setMinimumSize(1600, 800)
        self.setBackgroundBrush(Qt.black)

    def onResolutionChanged(self, resolution: float) -> None:
        self.mapResolution = resolution

    def resizeEvent(self, event: QResizeEvent | None) -> None:
        if self.mapLoaded:
            self.fitInView(self.scene.sceneRect(), Qt.KeepAspectRatio)
        self.mapLoadButton.move(self.width() - self.mapLoadButton.width() - 20, 20)
        super().resizeEvent(event)

    def _updateButtonPosition(self) -> None:
        marginX: int = 20
        marginY: int = 20
        self.mapLoadButton.move(
            self.width() - self.mapLoadButton.width() - marginX,
            marginY
        )

    def onMapUpdated(self, qImage: QImage) -> None:
        if qImage is None or qImage.isNull():
            return

        try:
            self.scene.removeItem(self.robotItem)
        except Exception as e:
            pass

        pixMap: QPixmap = QPixmap.fromImage(qImage)
        self.scene.clear()

        self.pixMapItem = QGraphicsPixmapItem(pixMap)
        self.pixMapItem.setScale(self.mapResolution)
        self.pixMapItem.setZValue(0)
        self.scene.addItem(self.pixMapItem)

        widthM: int = int(qImage.width() * self.mapResolution)
        heightM: int = int(qImage.height() * self.mapResolution)
        self.mapSize = (widthM, heightM)
        self.scene.setSceneRect(0, 0, widthM, heightM)
        print(f"[INFO][V] onMapUpdated : {qImage.size()}, {qImage.format()}, {widthM}, {heightM}, {self.mapResolution}")

        self.mapLoaded = True

        borderPen: QPen = QPen(Qt.gray)
        borderPen.setWidth(2)
        rect: QRectF = self.scene.sceneRect()
        self.scene.addRect(rect, borderPen, QBrush(Qt.NoBrush))

        self.fitInView(self.scene.sceneRect(), Qt.KeepAspectRatioByExpanding)

        self.robotItem.setPos(0, abs(0 - heightM))
        self.robotItem.setZValue(1)
        self.scene.addItem(self.robotItem)

    def mouseMoveEvent(self, event: QMouseEvent | None) -> None:
        if not self.mapLoaded:
            super().mouseMoveEvent(event)
            return

        scenePos: QPointF = self.mapToScene(event.pos())
        x: int = int(scenePos.x() / self.mapResolution)
        y: int = int(scenePos.y() / self.mapResolution)

        if x >= 0 and y >= 0:
            self.goalItemPos = (x, y)
        else:
            pass

        super().mouseMoveEvent(event)
        
    def mousePressEvent(self, event: QMouseEvent | None) -> None:
        if not self.mapLoaded:
            super().mousePressEvent(event)
            return

        x: int = self.goalItemPos[0]
        y: int = self.goalItemPos[1]

        width: int = self.mapSize[0]
        height: int = self.mapSize[1]
        
        if event.button() == Qt.RightButton:
            x = max(0, min(x, width))
            y = max(0, min(y, height))

            if not hasattr(self, "goalItem") or self.goalItem is None:
                self.goalItem = GoalItem(mapResolution=self.mapResolution)
                self.scene.addItem(self.goalItem)

            self.goalItem.setPos(x, y)
            self.goalItem.setZValue(1)
            self.goalItem.update()
            self.scene.update()

            print(f"[INFO][V] ready for path finding\n\t"
                  f"map path : {self.viewmodel.mapPath}\n\t"
                  f"robot x, y : {self.robotItem.x(), self.robotItem.y()}\n\t"
                  f"goal x, y : {self.goalItem.x(), self.goalItem.y()}")
        else:
            super().mousePressEvent(event)


__all__: list[str] = ["MapView"]