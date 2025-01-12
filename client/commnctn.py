import serial

BAUDRATE = 115200

class Communication:
     def __init__(self, port, baudrate=BAUDRATE):
        self.ser = serial.Serial(port, baudrate)
        
     def communication_send(self, buffer: bytes) -> int:
        if not self.ser.is_open:
            raise Exception("Serial connection is not open.")
        return self.ser.write(buffer)
    
     def communication_read(self, size: int) -> bytes:
        if not self.ser.is_open:
            raise Exception("Serial connection is not open.")
        return self.ser.read(size)
    
    
     def communication_open(self) -> bool:
        if not self.ser.is_open:
            self.ser.open()
        return self.ser.is_open
    
    
     def communication_close(self):   
        if self.ser.is_open:
            self.ser.close()
            
     # Test Code
if __name__ == "__main__":
    print("Communication module is running successfully!")
            
  
