import struct
from commnctn import Communication
from mbedtls import pk, hmac, hashlib, cipher

class Session:
    __RSA_SIZE = 256
    __EXPONENT = 65537
    __SECRET_KEY = b"Fj2-;wu3Ur=ARl2!Tqi6IuKM3nG]8z1+"

    __CLOSE = 0
    __GET_TEMP = 1
    __TOGGLE_RELAY = 3

    STATUS_OKAY = 0
    STATUS_ERROR = 1
    STATUS_EXPIRED = 2
    STATUS_HASH_ERROR = 3
    STATUS_BAD_REQUEST = 4
    STATUS_INVALID_SESSION = 5
    STATUS_CONNECTION_ERROR = 6

    def __init__(self, info: str) -> None:
        self.__SESSION_ID = bytes([0] * 8)
        self.__com = Communication(info)

        if not self.__com.connect():
            raise Exception("Failed to connect")

        self.__HMAC_KEY = hashlib.sha256()
        self.__HMAC_KEY.update(Session.__SECRET_KEY)
        self.__HMAC_KEY = self.__HMAC_KEY.digest()
        self.__HMAC_KEY = hmac.new(self.__HMAC_KEY, digestmod="SHA256")
        print("HMAC Key =", self.__HMAC_KEY.digest().hex())

        self.client_public_rsa = pk.RSA()
        self.client_public_rsa.generate(Session.__RSA_SIZE * 8, Session.__EXPONENT)
        print("\nPublic Key =", self.client_public_rsa.export_public_key().hex())

        if not self.send(self.client_public_rsa.export_public_key()):
            raise Exception("Failed exchange key.")

    def send(self, buf: bytes) -> bool:
        return self.__com.send(buf)

    def receive(self, size: int) -> bytes:
        return self.__com.receive(size)

    def toggle_relay(self):
        """Toggle the relay state."""
        try:
            command = self.__TOGGLE_RELAY.to_bytes(1, 'big')
            self.send(command)
            response = self.receive(1)

            if response == b'\x01':  # Assuming 0x01 indicates LED_ON
                return self.STATUS_OKAY, "Relay State: On"
            elif response == b'\x00':  # Assuming 0x00 indicates LED_OFF
                return self.STATUS_OKAY, "Relay State: Off"
            else:
                return self.STATUS_ERROR, "Unexpected response from device"
        
        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"

    def get_temperature(self):
        """Retrieve ESP32's internal temperature sensor value."""
        try:
            command = self.__GET_TEMP.to_bytes(1, 'big')
            self.send(command)
            response = self.receive(4)

            if len(response) == 4:
                temperature = struct.unpack("<f", response)[0]  # `<f` = Little-endian float
                return self.STATUS_OKAY, f"ESP32 Temperature: {temperature:.2f}°C"
            else:
                return self.STATUS_ERROR, "Failed to read temperature (wrong size)"

        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"

    def close(self):
        """Close the communication session."""
        self.__com.disconnect()

    def __del__(self):
        self.close()