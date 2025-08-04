import os
import ctypes
import sys

OS_TYPE_WINDOWS: str = "win32"
OS_TYPE_LINUX: str = "linux"
OS_TYPE_MAC: str = "darwin"

class LibLoader:

    def __init__(self) -> None:
        self.base_dir: str = os.path.dirname(os.path.abspath(__file__))
        self.extension: str = ""
        self.os_type: str = sys.platform

        if self.os_type.startswith(OS_TYPE_WINDOWS):
            self.extension = ".dll"
        elif self.os_type.startswith(OS_TYPE_LINUX):
            self.extension = ".so"
        elif self.os_type.startswith(OS_TYPE_MAC):
            self.extension = ".dylib"
        else:
            raise RuntimeError(f"Unsupported platform: {self.os_type}")

        print(f"Detected OS: {self.os_type}, extension: {self.extension}")


    def load_library(self, package_name: str, lib_name: str) -> ctypes.CDLL:
        rel_path: str = os.path.join(self.base_dir, f"../{package_name}/build/lib{lib_name}{self.extension}")
        lib_path: str = os.path.normpath(rel_path)

        if not os.path.exists(lib_path):
            raise FileNotFoundError(f"Cannot find DLL at: {lib_path}")

        lib: ctypes.CDLL = ctypes.CDLL(lib_path)

        return lib


__all__: list[str] = ["LibLoader"]