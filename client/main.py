import sys
import struct
from session import Session
from PyQt6.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QPushButton, QLabel, QWidget, QTextEdit

class ClientWindow(QMainWindow):
    __TEMPERATURE = 1
    __TOGGLE_RELAY = 3

    def __init__(self, port, baud):
        try:
            self.__session = Session(port, baud)
        except Exception as exp:
            print(exp)
            print("Failed to create the session ...")
            sys.exit(1)

        super().__init__()

        self.setWindowTitle("Client")
        self.setFixedSize(500, 500)

        main_layout = QVBoxLayout()

        button_layout = QHBoxLayout()

        self.__session_button = QPushButton("Establish Session", self)
        self.__session_button.setFixedSize(150, 30)
        self.__session_button.clicked.connect(self.__toggleSession)
        button_layout.addWidget(self.__session_button)

        self.__temp_button = QPushButton("Get Temperature", self)
        self.__temp_button.clicked.connect(self.__getTemperature)
        self.__temp_button.setDisabled(True)
        button_layout.addWidget(self.__temp_button)

        self.__relay_button = QPushButton("Toggle Relay", self)
        self.__relay_button.clicked.connect(self.__toggleRelay)
        self.__relay_button.setDisabled(True)
        button_layout.addWidget(self.__relay_button)

        self.clear_log_button = QPushButton("Clear", self)
        self.clear_log_button.setStyleSheet("color: blue; text-decoration: underline;")
        self.clear_log_button.clicked.connect(self.__clearLog)
        button_layout.addWidget(self.clear_log_button)

        main_layout.addLayout(button_layout)

        self.__log = QTextEdit(self)
        self.__log.setReadOnly(True)
        self.__log.setStyleSheet("background-color: black; color: white; border: 1px solid white;")
        self.__log.setCursorWidth(0)
        main_layout.addWidget(self.__log)

        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)

    def __toggleSession(self):
        self.__session_button.setText("Wait ...")

        if self.__session.is_established():
            if self.__session.terminate():
                self.__session_button.setText("Establish Session")
                self.__log.append("Session Close: Done")
                self.__temp_button.setDisabled(True)
                self.__relay_button.setDisabled(True)
                self.__session = Session(self.__session._Session__com._Communication__port,
                                     self.__session._Session__com._Communication__speed)
            else:
                self.__session_button.setText("Close Session")
                self.__log.append("Session Close: Failed")
        else:
            if self.__session.establish():
                self.__session_button.setText("Close Session")
                self.__log.append("Session Establish: Done")
                self.__temp_button.setDisabled(False)
                self.__relay_button.setDisabled(False)
            else:
                self.__session_button.setText("Establish Session")
                self.__log.append("Session Establish: Failed")

    def __getTemperature(self):
        temp = bytearray()
        status = self.__session.request(ClientWindow.__TEMPERATURE, temp)

        if status == Session.OKAY:
            try:
                if len(temp) >= 4:
                    temperature = struct.unpack('<f', temp[:4])[0]
                    self.__log.append(f"Temperature: {temperature:.2f} °C")
                else:
                    self.__log.append("Temperature Error: Not enough data")
            except struct.error as e:
                self.__log.append(f"Temperature Decode Error: {e}")
        elif status == Session.EXPIRED:
            self.__log.append("Session Error: Expired")
            self.__session_button.setText("Establish Session")
            self.__temp_button.setDisabled(True)
            self.__relay_button.setDisabled(True)
            self.__session = Session(self.__session._Session__com._Communication__port,
                                     self.__session._Session__com._Communication__speed)
        else:
            self.__log.append("Temperature: Error")

    def __toggleRelay(self):
        state = bytearray()
        status = self.__session.request(ClientWindow.__TOGGLE_RELAY, state)
        if status == Session.OKAY:
            self.__log.append(f"Relay toggled. New state: {'ON' if state[0] else 'OFF'}")
        elif status == Session.EXPIRED:
            self.__log.append("Session Error: Expired")
            self.__session_button.setText("Establish Session")
            self.__temp_button.setDisabled(True)
            self.__relay_button.setDisabled(True)
            self.__session = Session(self.__session._Session__com._Communication__port,
                                     self.__session._Session__com._Communication__speed)
        else:
            self.__log.append("Toggle Relay: Error")

    def __clearLog(self):
        self.__log.clear()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: main.py <serial_port> <baud_rate>")
        sys.exit(1)

    serial_port = sys.argv[1]
    baud_rate = int(sys.argv[2])

    app = QApplication(sys.argv)
    window = ClientWindow(serial_port, baud_rate)
    window.show()
    sys.exit(app.exec())
