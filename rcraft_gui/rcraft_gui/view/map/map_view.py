from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsPixmapItem, QSizePolicy, QPushButton
from PyQt5.QtGui import QPixmap, QPainter, QImage, QResizeEvent
from PyQt5.QtCore import Qt, QSize

from rcraft_gui.scene.robot.robot_item import RobotItem
from rcraft_gui.viewmodel.map.map_viewmodel import MapViewModel


class MapView(QGraphicsView):

    def __init__(self, viewmodel: MapViewModel, parent=None) -> None:
        super().__init__(parent=parent)
        self.viewmodel: MapViewModel = viewmodel
        self.scene = QGraphicsScene()
        self.setScene(self.scene)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setRenderHints(QPainter.Antialiasing | QPainter.SmoothPixmapTransform)
        self.setMinimumSize(1600, 800)
        self.setBackgroundBrush(Qt.black)

        self.pixmap_item: QGraphicsPixmapItem | None = None

        self.load_button = QPushButton("Load Map", self)
        self.load_button.setFixedSize(120, 30)
        self.load_button.setStyleSheet("background-color: red; color: white;")
        self.load_button.clicked.connect(self.viewmodel.select_map_file)

        self.robot_item: RobotItem = RobotItem()
        self.robot_item.set_size(2)
        self.scene.addItem(self.robot_item)


    def resizeEvent(self, event: QResizeEvent | None) -> None:
        self.fitInView(self.scene.sceneRect(), Qt.KeepAspectRatio)
        self.update_button_position()
        super().resizeEvent(event)

    def update_button_position(self) -> None:
        margin_x: int = 20
        margin_y: int = 20
        self.load_button.move(
            self.width() - self.load_button.width() - margin_x,
            margin_y
        )

    def update_map_from_qimage(self, qimage: QImage) -> None:
        print("[V] UPDATE")
        if qimage is None or qimage.isNull():
            print("[ERROR][MapView] Invalid QImage")
            return

        print(f"[V] Image : {qimage.size()}, {qimage.format()}")

        pixmap: QPixmap = QPixmap.fromImage(qimage)
        self.scene.clear()

        item: QGraphicsPixmapItem = QGraphicsPixmapItem(pixmap)
        self.scene.addItem(item)
        self.scene.setSceneRect(item.boundingRect())
        self.fitInView(self.scene.sceneRect(), Qt.KeepAspectRatio)


__all__: list[str] = ["MapView"]