"""
    Client gui
"""

import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QTextEdit, QVBoxLayout, QHBoxLayout, QWidget, QLabel
)
from PyQt6.QtCore import QThread, pyqtSignal
import serial.tools.list_ports
from session import Session




class GetTemperatureThread(QThread):
    result = pyqtSignal(int, str)

    def __init__(self, session):
        super().__init__()
        self.session = session

    def run(self):
        status, message = self.session.get_temperature()
        self.result.emit(status, message)


class ToggleRelayThread(QThread):
    result = pyqtSignal(int, str)

    def __init__(self, session):
        super().__init__()
        self.session = session

    def run(self):
        status, message = self.session.toggle_relay()
        self.result.emit(status, message)


class ClientGui(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Client")
        self.setFixedSize(500, 400)

        # Initialize session
        self.session = Session("/dev/ttyUSB0")


        # Main layout
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.main_layout = QVBoxLayout()
        self.main_layout.setContentsMargins(10, 10, 10, 10)
        self.main_layout.setSpacing(10)

        self.button_layout = QHBoxLayout()

        # Buttons
        self.close_session_button = QPushButton("Close Session")
        self.get_temp_button = QPushButton("Get Temperature")
        self.toggle_relay_button = QPushButton("Toggle Relay")
        self.clear_button = QLabel("<a href='#'>Clear</a>")
        self.clear_button.setStyleSheet("color: blue; text-decoration: underline;")
        self.clear_button.setOpenExternalLinks(False)

        # Add buttons to horizontal layout
        self.button_layout.addWidget(self.close_session_button)
        self.button_layout.addWidget(self.get_temp_button)
        self.button_layout.addWidget(self.toggle_relay_button)
        self.button_layout.addStretch()
        self.button_layout.addWidget(self.clear_button)

        # Text area
        self.log_area = QTextEdit()
        self.log_area.setReadOnly(True)
        self.log_area.setStyleSheet("background-color: black; color: white; font-size: 12px;")

        # Add layouts to the main
        self.main_layout.addLayout(self.button_layout)
        self.main_layout.addWidget(self.log_area)

        self.central_widget.setLayout(self.main_layout)

        # Connect button clicks
        self.close_session_button.clicked.connect(self.close_session)
        self.get_temp_button.clicked.connect(self.handle_get_temperature)
        self.toggle_relay_button.clicked.connect(self.handle_toggle_relay)
        self.clear_button.linkActivated.connect(self.clear_log)

    def handle_get_temperature(self):
        """Handle the Get Temperature button click."""
        self.temp_thread = GetTemperatureThread(self.session)
        self.temp_thread.result.connect(self.display_message)
        self.temp_thread.start()

    def handle_toggle_relay(self):
        """Handle the Toggle Relay button click."""
        self.toggle_thread = ToggleRelayThread(self.session)
        self.toggle_thread.result.connect(self.display_message)
        self.toggle_thread.start()

    def display_message(self, status, message):
        """Display messages in the log area."""
        self.log_area.append(message)

    def close_session(self):
        """Handle the Close Session button click."""
        self.log_area.append("Session Closed")

    def clear_log(self):
        """Clear the log area."""
        self.log_area.clear()


def main():
    app = QApplication(sys.argv)
    window = ClientGui()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main() 