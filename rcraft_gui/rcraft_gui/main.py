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

        self.overlayView: OverlayView = OverlayView()
        self.setCentralWidget(self.overlayView)

if __name__ == "__main__":
    app: QApplication = QApplication(sys.argv)
    window: MainWindow = MainWindow()
    window.show()
    sys.exit(app.exec_())
