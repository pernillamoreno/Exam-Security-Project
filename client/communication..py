import serial
import time

BAUDRATE = 115200

import serial

class Communication:
    port: str
    speed: int

    def init(self, port: str, speed: int) -> None:
        Communication.port = port
        Communication.speed = speed
        self.con = serial.Serial(Communication.port, Communication.speed)

    def connect(self) -> bool:
        status = False
        try:
            if not self.con.is_open:
                self.con.open()
            status = self.con.is_open
        except:
            pass
        return status

    def disconnect(self):
        try:
            self.con.close()
        except:
            pass

    def send(self, buf: bytes) -> bool:
        status = False
        try:
            if self.con.is_open:
                self.con.reset_output_buffer()
                status = (len(buf) == self.con.write(buf))
        except:
            pass
        return status

    def receive(self, size: int) -> bytes:
        buffer = b''
        try:
            if self.con.is_open:
                self.con.reset_input_buffer()
                buffer = self.con.read(size)
        except:
            pass
        return buffer

    def del__(self):
        self.disconnect()