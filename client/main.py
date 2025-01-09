"""
    Client gui
"""

import serial  # Import pyserial library
import time
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QTextEdit, QVBoxLayout, QHBoxLayout, QWidget, QLabel
)
import sys
from session import Session

class ClientGui(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Client")
        self.setFixedSize(500, 400)  # fixed size 

        # Initialize serial communication
        self.serial_connection = serial.Serial(port="/dev/ttyUSB0", baudrate=9600, timeout=1)

        # Main layout
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.main_layout = QVBoxLayout()
        self.main_layout.setContentsMargins(10, 10, 10, 10)
        self.main_layout.setSpacing(10)  # Add spacing between widgets

        self.button_layout = QHBoxLayout()

        # Buttons
        self.close_session_button = QPushButton("Close Session")
        self.get_temp_button = QPushButton("Get Temperature")
        self.toggle_relay_button = QPushButton("Toggle Relay")
        self.clear_button = QLabel("<a href='#'>Clear</a>")
        self.clear_button.setStyleSheet("color: blue; text-decoration: underline;")
        self.clear_button.setOpenExternalLinks(False)

        #  buttons to horizontal layout
        self.button_layout.addWidget(self.close_session_button)
        self.button_layout.addWidget(self.get_temp_button)
        self.button_layout.addWidget(self.toggle_relay_button)
        self.button_layout.addStretch()  # Add space to right
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
        self.get_temp_button.clicked.connect(self.get_temperature)
        self.toggle_relay_button.clicked.connect(self.toggle_relay)
        self.clear_button.linkActivated.connect(self.clear_log)

        # Relay state tracking
        self.relay_state = False

    # Button functions
    def close_session(self):
        self.log_area.append("Establish Session: Done")

    def get_temperature(self):
     """
        try:
            # Toggle the relay state
            command = "TOGGLE_ON" if not self.relay_state else "TOGGLE_OFF"
            self.serial_connection.write((command + '\n').encode())  # Send command to ESP32
            time.sleep(0.5)  # Wait for ESP32 to respond

            # Read acknowledgment from ESP32
            response = self.serial_connection.readline().decode().strip()
            if response == "LED_ON":
                self.relay_state = True
                self.log_area.append("Relay State: On")
            elif response == "LED_OFF":
                self.relay_state = False
                self.log_area.append("Relay State: Off")
            else:
                self.log_area.append("Error: No response from ESP32")
        except Exception as e:
            self.log_area.append(f"Error: {e}")

    def clear_log(self):
        self.log_area.clear()"""
        #self.log_area.append("Temperature: Here is my temp")

    def toggle_relay(self):
        try:
            # Toggle the relay state
            command = "TOGGLE_ON" if not self.relay_state else "TOGGLE_OFF"
            self.serial_connection.write((command + '\n').encode())  # Send command to ESP32
            time.sleep(0.5)  # Wait for ESP32 to respond

            # Read acknowledgment from ESP32
            response = self.serial_connection.readline().decode().strip()
            if response == "LED_ON":
                self.relay_state = True
                self.log_area.append("Relay State: On")
            elif response == "LED_OFF":
                self.relay_state = False
                self.log_area.append("Relay State: Off")
            else:
                self.log_area.append("Error: No response from ESP32")
        except Exception as e:
            self.log_area.append(f"Error: {e}")

    def clear_log(self):
        self.log_area.clear()


def main():
    app = QApplication(sys.argv)
    window = ClientGui()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()


