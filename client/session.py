from commnctn import Communication
from mbedtls import pk, hmac, hashlib, cipher

class Session:
    RSA_SIZE = 256
    EXPONENT = 65537
    secret_key = b"Fj2-;wu3Ur=ARl2!Tqi6IuKM3nG]8z1+"

    CLOSE = 0
    GET_TEMP = 1
    TOGGLE_RELAY = 3

    STATUS_OKAY = 0
    STATUS_ERROR = 1
    STATUS_EXPIRED = 2
    STATUS_HASH_ERROR = 3
    STATUS_BAD_REQUEST = 4
    STATUS_INVALID_SESSION = 5
    STATUS_CONNECTION_ERROR = 6

    def __init__(self, port, baudrate):
        self.communication = Communication(port, baudrate) 
        self.relay_state = False  
        self.session_id = bytes([0 * 8])  

        if not self.communication.communication_open():
            raise Exception("Failed to connect")

        self.hmac_ctx = hashlib.sha256()  
        self.hmac_ctx.update(Session.secret_key) 
         
        self.hmac_ctx = self.hmac_ctx.digest()
        self.hmac_ctx = hmac.new(self.hmac_ctx, digestmod="SHA256")

        self.client_public_rsa = pk.RSA()
        self.client_public_rsa.generate(Session.RSA_SIZE * 8, Session.EXPONENT)  
       
        print("\n  Public Key =", self.client_public_rsa.export_public_key().hex())  # REMOVE THIS LATER
        
        if not self.communication.communication_send(self.client_public_rsa.export_public_key()):
            raise Exception("Failed to send client's public key.")

        
       # buffer = self.communication.communication_read(1024)      
        #print(f"\nReceived Buffer Length: {len(buffer)} (Expected: {2 * Session.RSA_SIZE})")
        #print(f"Received Buffer Hex: {buffer.hex()}")   # REMOVE THIS LATER

    def toggle_relay(self):
        try:
            command = self.TOGGLE_RELAY.to_bytes(1, 'big')
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
            command = self.GET_TEMP.to_bytes(1, 'big')
            self.communication.communication_send(command)

            response = self.communication.communication_read(10)
            if response:
                temperature = response.decode().strip()
                return self.STATUS_OKAY, f"Temperature: {temperature}°C"
            else:
                return self.STATUS_ERROR, "Failed to read temperature"
        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"