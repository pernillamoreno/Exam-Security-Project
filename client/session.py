from commnctn import Communication
from mbedtls import pk, hmac, hashlib, cipher

class Session:from commnctn import Communication
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
        self.relay_state = False  # Track the relay state

    def toggle_relay(self):
        """Toggle the relay state."""
        try:
            # Determine the command based on the current relay state
            command = self.__TOGGLE_RELAY.to_bytes(1, 'big')
            self.communication.communication_send(command)

            # Wait for response
            response = self.communication.communication_read(1)
            if response == b'\x01':  # Assuming 0x01 indicates LED_ON
                self.relay_state = True
                return self.STATUS_OKAY, "Relay State: On"
            elif response == b'\x00':  # Assuming 0x00 indicates LED_OFF
                self.relay_state = False
                return self.STATUS_OKAY, "Relay State: Off"
            else:
                return self.STATUS_ERROR, "Unexpected response from device"

        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"

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
        self.relay_state = False  # Track the relay state

    def toggle_relay(self):
        """Toggle the relay state."""
        try:
            # Determine the command based on the current relay state
            command = self.__TOGGLE_RELAY.to_bytes(1, 'big')
            self.communication.communication_send(command)

            # Wait for response
            response = self.communication.communication_read(1)
            if response == b'\x01':  # Assuming 0x01 indicates LED_ON
                self.relay_state = True
                return self.STATUS_OKAY, "Relay State: On"
            elif response == b'\x00':  # Assuming 0x00 indicates LED_OFF
                self.relay_state = False
                return self.STATUS_OKAY, "Relay State: Off"
            else:
                return self.STATUS_ERROR, "Unexpected response from device"

        except Exception as e:
            return self.STATUS_CONNECTION_ERROR, f"Error: {e}"


    