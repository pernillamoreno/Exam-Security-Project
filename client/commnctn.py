import serial

class Communication:
    __port: str
    __speed: int

    def __init__(self, info: str) -> None:
        addr = info.split(" ")
        Communication.__port = addr[0]
        Communication.__speed = int(addr[1])
        self.__con = serial.Serial(Communication.__port, Communication.__speed)

    def connect(self) -> bool:
        status = False
        try:
            if not self.__con.is_open:
                self.__con.open()
            status = self.__con.is_open
        except:
            pass
        return status

    def disconnect(self):
        try:
            self.__con.close()
        except:
            pass

    def send(self, buf: bytes) -> bool:
        status = False
        try:
            if self.__con.is_open:
                self.__con.reset_output_buffer()
                status = (len(buf) == self.__con.write(buf))
        except:
            pass
        return status

    def receive(self, size: int) -> bytes:
        buffer = b""
        try:
            if self.__con.is_open:
                self.__con.reset_input_buffer()
                buffer = self.__con.read(size)
        except:
            pass
        return buffer

    def __del__(self):
        self.disconnect()
  
