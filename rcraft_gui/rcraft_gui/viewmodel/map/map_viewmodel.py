import platform
from PyQt5.QtCore import QObject, pyqtSignal
from PyQt5.QtGui import QImage
from PyQt5.QtWidgets import QFileDialog
from rcraft_gui.model.map.map_model import MapModel


class MapViewModel(QObject):
    mapUpdated: pyqtSignal = pyqtSignal(QImage)
    resolutionChanged: pyqtSignal = pyqtSignal(float)
    robotPoseChanged: pyqtSignal = pyqtSignal(int, int)

    def __init__(self):
        super().__init__()
        self.model: MapModel = MapModel()
        self.mapPath: str = ""

    def handleMapSelect(self) -> None:
        selectFolderPath: str = ""
        osType: str = platform.system()

        if osType == "Windows":
            selectFolderPath = "C:\\"
        else:
            selectFolderPath = "~"

        path, _ = QFileDialog.getOpenFileName(
            None,
            "Select Map Image or YAML",
            selectFolderPath,
            "Map Files (*.pgm *.png *.bmp *.jpg *.yaml);;All Files (*)"
        )

        if not path:
            return
        else:
            print(f"[INFO][VM] selected Map Path : {path}")

        success: bool = self.model.loadMapFromPath(path)
        if not success:
            print("[ERROR][VM] Failed to load map")
            return

        self.mapPath = path
        qImage: QImage = self.model.getQImageFromNumPy()

        if qImage:
            self.resolutionChanged.emit(self.model.mapResolution)
            self.mapUpdated.emit(qImage)


__all__: list[str] = ["MapViewModel"]