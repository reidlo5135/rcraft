from PyQt5.QtCore import QSize, Qt
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QStackedLayout
from PyQt5.QtGui import QGuiApplication, QScreen, QResizeEvent
from rcraft_gui.view.map.map_view import MapView

from rcraft_gui.viewmodel.map.map_viewmodel import MapViewModel


class OverlayView(QWidget):

    def __init__(self, parent=None) -> None:
        super().__init__(parent)

        screen: QScreen | None = QGuiApplication.primaryScreen()
        screen_size: QSize = screen.size()

        base_width: int = 1600
        base_height: int = 800
        scaled_width: int = int(screen_size.width() * (base_width / 1920))
        scaled_height: int = int(screen_size.height() * (base_height / 1080))
        self.setMinimumSize(scaled_width, scaled_height)

        self.map_viewmodel: MapViewModel = MapViewModel()
        self.map_view: MapView = MapView(viewmodel=self.map_viewmodel)
        self.map_viewmodel.map_updated.connect(self.map_view.update_map_from_qimage)

        # Layout
        layout: QVBoxLayout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.map_view)

    def resizeEvent(self, event: QResizeEvent) -> None:
        super().resizeEvent(event)


__all__: list[str] = ["OverlayView"]