import sys
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QApplication, QMainWindow
from rcraft_gui.view.main.overlay_view import OverlayView

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Rcraft")
        self.setGeometry(100, 100, 1920, 1080)
        self.setWindowIcon(QIcon("resource/image/favicon.ico"))

        self.overlay_view = OverlayView()
        self.setCentralWidget(self.overlay_view)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
