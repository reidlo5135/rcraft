from PyQt5.QtCore import QObject, pyqtSignal
from PyQt5.QtGui import QImage
from PyQt5.QtWidgets import QFileDialog

from rcraft_gui.model.map.map_model import MapModel


class MapViewModel(QObject):
    map_updated: pyqtSignal = pyqtSignal(QImage)

    def __init__(self):
        super().__init__()
        self.model: MapModel = MapModel()

    def select_map_file(self) -> None:
        file_path, _ = QFileDialog.getOpenFileName(
            caption="Select Map Image",
            filter="Map Image Files (*.bmp *.png *.jpg *.jpeg *.pgm)"
        )

        if not file_path:
            return

        if not self.model.load_map_from_path(file_path):
            print("[ERROR][MapViewModel] Failed to load map")
            return

        qimage: QImage = self.model.get_qimage_from_numpy()

        if qimage is None:
            print("[ERROR][MapViewModel] Failed to convert to Image")
        else:
            self.map_updated.emit(qimage)


__all__: list[str] = ["MapViewModel"]