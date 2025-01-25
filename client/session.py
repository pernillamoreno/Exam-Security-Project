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

    def __init__(self, port):
        self.communication = Communication(port)
        self.relay_state = False  
        self.__SESSION_ID = bytes([0 * 8])

        if not self.communication.communication_open():
            raise Exception("Failed to connect")

        self.__HMAC_KEY = hashlib.sha256()
        self.__HMAC_KEY.update(Session.__SECRET_KEY)
        self.__HMAC_KEY = self.__HMAC_KEY.digest()
        self.__HMAC_KEY = hmac.new(self.__HMAC_KEY, digestmod="SHA256")
        print("      HMAC Key =", self.__HMAC_KEY.digest().hex())  # REMOVE THIS LATER

        self.client_public_rsa = pk.RSA()
        self.client_public_rsa.generate(Session.__RSA_SIZE * 8, Session.__EXPONENT)
        print("\n  Public Key =", self.client_public_rsa.export_public_key().hex())  # REMOVE THIS LATER
        
        if not self.communication.communication_send(self.client_public_rsa.export_public_key()):
            raise Exception("Failed to send client's public key.")

        
        #buffer = self.communication.communication_read(1024)      
        #print(f"\nReceived Buffer Length: {len(buffer)} (Expected: {2 * Session.__RSA_SIZE})")
        #print(f"Received Buffer Hex: {buffer.hex()}")   # REMOVE THIS LATER
        
        

    def toggle_relay(self):
        try:
            command = self.__TOGGLE_RELAY.to_bytes(1, 'big')
            self.communication.communication_send(command)

            response = self.communication.communication_read(1)
            if response == b'\x01':
                self.relay_state = True
                return self.STATUS_OKAY, "Relay State: On"
            elif response == b'\x00':
                self.relay_state = False
                return self.STATUS_OKAY, "Relay State: Off"
            else:
                return self.STATUS_ERROR, "Unexpected response from device"
        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"

    def get_temperature(self):
        try:
            command = self.__GET_TEMP.to_bytes(1, 'big')
            self.communication.communication_send(command)

            response = self.communication.communication_read(10)
            if response:
                try:
                    temperature = response.decode().strip()  # Decode and clean string
                    print(f"Received Temperature: {temperature}°C")  # Debugging
                    return self.STATUS_OKAY, f"Temperature: {temperature}°C"
                except UnicodeDecodeError:
                    return self.STATUS_ERROR, "Error decoding temperature response"
            else:
                return self.STATUS_ERROR, "Failed to read temperature"
    
        except Exception as e:
           return self.STATUS_CONNECTION_ERROR, f"Error: {e}"