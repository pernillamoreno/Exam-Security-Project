import serial
import time

BAUDRATE = 115200

class Communication:
    def __init__(self, port, baudrate=BAUDRATE):
        self.ser = serial.Serial(port, baudrate, timeout=2)

    def communication_open(self) -> bool:
        status = False
        try:
            if not self.ser.is_open:
                self.ser.open()
            status = self.ser.is_open
        except:
            pass
        return status

    def communication_close(self):
        try:
            self.ser.close()
        except:
            pass

    def communication_send(self, buffer: bytes) -> bool:
        status = False
        try:
            if self.ser.is_open:
                self.ser.reset_output_buffer()
                status = (len(buffer) == self.ser.write(buffer))
        except:
            pass
        return status

    def communication_read(self, size: int) -> bytes:
        buffer = b""
        try:
            if self.ser.is_open:
                self.ser.reset_input_buffer()
                buffer = self.ser.read(size)
        except:
            pass
        return buffer

    # Destructor to close serial port when object is deleted
    def __del__(self):
        self.communication_close()
  
if __name__ == "__main__":
    print("Communication module is running successfully!")
            
  

  
