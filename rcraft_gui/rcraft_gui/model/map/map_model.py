import os
import cv2
import numpy as np
from typing import Optional, Any
from PIL import Image
from PyQt5.QtGui import QImage


class MapModel:

    def __init__(self) -> None:
        self.map_image: Optional[Image.Image] = None
        self._np_buffer: Optional[np.ndarray] = None
        self.map_path: Optional[str] = None

    def load_map_from_path(self, path: str) -> bool:
        if not os.path.exists(path):
            print(f"[ERROR][MapModel] File does not exist : {path}")
            return False

        ext: str = os.path.splitext(path)[1].lower()
        print(f"[INFO][MapModel] Loaded Map Image : {path}, {ext}")

        try:
            if ext == ".pgm":
                gray_img: cv2.Mat | np.ndarray[Any, np.dtype] = cv2.imread(path, cv2.IMREAD_GRAYSCALE)

                if gray_img is None:
                    raise IOError("OpenCV failed to load PGM file")

                self.map_image = Image.fromarray(gray_img).convert("RGB")
                print(f"[INFO][MapModel] Loaded PGM map via OpenCV: {path}")
            else:
                self.map_image = Image.open(path).convert("RGB")
                print(f"[INFO][MapModel] Loaded map: {path}")

            self.map_path = path
            print(f"[INFO][MapModel] Loaded map from {path}")
            return True
        except Exception as e:
            print(f"[ERROR][MapModel] Failed to load image : {e}")
            return False

    def get_qimage_from_numpy(self) -> Optional[QImage]:
        if self.map_image is None:
            return None

        rgb_image: Image = self.map_image.convert("RGB")
        width, height = rgb_image.size
        raw_data: bytes = rgb_image.tobytes("raw", "RGB")
        bytes_per_line: int = 3 * width

        qimage: QImage = QImage(raw_data, width, height, bytes_per_line, QImage.Format_RGB888)

        print(f"[M] image : {qimage.size()}, {qimage.format()}")

        return qimage.copy()


__all__: list[str] = ["MapModel"]