from PyQt5.QtCore import QSize
from PyQt5.QtWidgets import QWidget, QVBoxLayout
from PyQt5.QtGui import QGuiApplication, QScreen, QResizeEvent

from rcraft_viz.view.map.map_view import MapView
from rcraft_viz.viewmodel.map.map_viewmodel import MapViewModel


class OverlayView(QWidget):

    def __init__(self, parent=None) -> None:
        super().__init__(parent)

        screen: QScreen | None = QGuiApplication.primaryScreen()
        screenSize: QSize = screen.size()

        baseWidth: int = 1600
        baseHeight: int = 800
        scaledWidth: int = int(screenSize.width() * (baseWidth / 1920))
        scaledHeight: int = int(screenSize.height() * (baseHeight / 1080))
        self.setMinimumSize(scaledWidth, scaledHeight)

        self.mapViewModel: MapViewModel = MapViewModel()
        self.mapView: MapView = MapView(viewmodel=self.mapViewModel)
        self.mapViewModel.mapUpdated.connect(self.mapView.onMapUpdated)
        self.mapViewModel.resolutionChanged.connect(self.mapView.onResolutionChanged)

        layout: QVBoxLayout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.mapView)

    def resizeEvent(self, event: QResizeEvent) -> None:
        super().resizeEvent(event)


__all__: list[str] = ["OverlayView"]