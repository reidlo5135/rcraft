import os
import cv2
import numpy as np
from typing import Optional, Any
from PIL import Image
from PyQt5.QtGui import QImage


class MapModel:

    def __init__(self) -> None:
        self.mapImage: Optional[Image.Image] = None
        self.mapImagePath: Optional[str] = None
        self.mapResolution: float = 1.0
        self.mapOrigin: tuple[float, float] = (0.0, 0.0)

    def loadMapFromPath(self, path: str) -> bool:
        if not os.path.exists(path):
            print(f"[ERROR][M] File does not exist : {path}")
            return False

        mapExtension: str = os.path.splitext(path)[1].lower()
        print(f"[INFO][M] Loaded Map Image : {path}, {mapExtension}")

        try:
            if mapExtension == ".pgm":
                grayImage: cv2.Mat | np.ndarray[Any, np.dtype] = cv2.imread(path, cv2.IMREAD_GRAYSCALE)

                if grayImage is None:
                    raise IOError("OpenCV failed to load PGM file")

                self.mapImage = Image.fromarray(grayImage).convert("RGB")
                print(f"[INFO][M] Loaded PGM map via OpenCV: {path}")
            else:
                self.mapImage = Image.open(path).convert("RGB")
                print(f"[INFO][M] Loaded map: {path}")

            self.mapImagePath = path
            print(f"[INFO][M] Loaded map from {path}")
            return True
        except Exception as e:
            print(f"[ERROR][M] Failed to load image : {e}")
            return False

    def getQImageFromNumPy(self) -> Optional[QImage]:
        if self.mapImage is None:
            return None

        rgbImage: Image = self.mapImage.convert("RGB")
        width, height = rgbImage.size
        rawData: bytes = rgbImage.tobytes("raw", "RGB")
        bytesPerLine: int = 3 * width

        qImage: QImage = QImage(rawData, width, height, bytesPerLine, QImage.Format_RGB888).copy()

        print(f"[INFO][M] getQImageFromNumPy : {qImage.size()}, {qImage.format()}")

        return qImage


__all__: list[str] = ["MapModel"]