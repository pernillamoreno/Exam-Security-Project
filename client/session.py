from communication import Communication
from mbedtls import pk, hmac, hashlib, cipher

class Session:
    __RSA_SIZE = 256
    __EXPONENT = 65537
    __SECRET_KEY = b"Fj2-;wu3Ur=ARl2!Tqi6IuKM3nG]8z1+"

    OKAY = 0
    ERROR = 1
    EXPIRED = 2

    __TERMINATE = 0

    def __init__(self, port: str, speed: int) -> None:
        self.__SESSION_ID = bytes(8 * [0])
        self.__com = Communication(port, speed)

        if not self.__com.connect():
            raise Exception("Failed to connect")
        
        self.__hmac = hashlib.sha256()
        self.__hmac.update(Session.__SECRET_KEY)
        self.__hmac = self.__hmac.digest()
        self.__hmac = hmac.new(self.__hmac, digestmod="SHA256")

        self.__clientRSA = pk.RSA()
        self.__clientRSA.generate(Session.__RSA_SIZE * 8, Session.__EXPONENT)

        if not self.__send(self.__clientRSA.export_public_key()):
            raise Exception("1) Failed to exchange keys")
            
        buffer = self.__receive(2 * Session.__RSA_SIZE)
        print("Received server key length:", len(buffer))
        if 0 == len(buffer):
            raise Exception("2) Failed to exchange keys")
        
        self.__serverRSA = self.__clientRSA.decrypt(buffer[0:Session.__RSA_SIZE])
        self.__serverRSA += self.__clientRSA.decrypt(buffer[Session.__RSA_SIZE:2*Session.__RSA_SIZE])
        self.__serverRSA = pk.RSA().from_DER(self.__serverRSA)

        del self.__clientRSA
        self.__clientRSA = pk.RSA()
        self.__clientRSA.generate(Session.__RSA_SIZE * 8, Session.__EXPONENT)

        buffer = self.__clientRSA.export_public_key() + self.__clientRSA.sign(self.__SECRET_KEY, "SHA256")
        buffer = self.__serverRSA.encrypt(buffer[0:184]) + self.__serverRSA.encrypt(buffer[184:368]) + self.__serverRSA.encrypt(buffer[368:550])

        if not self.__send(buffer):
            raise Exception("3) Failed to exchange keys")
        
        buffer = self.__receive(Session.__RSA_SIZE)
        if 0 == len(buffer):
            raise Exception("4) Failed to exchange keys")
        
        if b"DONE" != self.__clientRSA.decrypt(buffer):
            raise Exception("5) Failed to exchange keys")

    def __receive(self, length: int) -> bytes:
        buffer = self.__com.receive(length + self.__hmac.digest_size)
        self.__hmac.update(buffer[0:length])
        if buffer[length:length + self.__hmac.digest_size] == self.__hmac.digest():
            buffer = buffer[0:length]
        else:
            buffer = b''
        return buffer


    def __send(self, buf: bytes) -> bool:
        self.__hmac.update(buf)
        buf += self.__hmac.digest()
        return self.__com.send(buf)
    
    def __bool__(self) -> bool:
        return (0 == int.from_bytes(self.__SESSION_ID, 'little'))
    
    def establish(self) -> bool:
        status = False
        self.__SESSION_ID = bytes(8 * [0])

        try:
            buffer = self.__clientRSA.sign(self.__SECRET_KEY, "SHA256")
            buffer = self.__serverRSA.encrypt(buffer[0:Session.__RSA_SIZE//2]) + self.__serverRSA.encrypt(buffer[Session.__RSA_SIZE//2:Session.__RSA_SIZE])

            if self.__send(buffer):
               buffer = self.__receive(Session.__RSA_SIZE)
               if 0 < len(buffer):
                   buffer = self.__clientRSA.decrypt(buffer)
                   self.__SESSION_ID = buffer[0:8]
                   if 0 != int.from_bytes(self.__SESSION_ID, 'little'):
                       self.__AES = cipher.AES.new(buffer[8:40], buffer[40:56])
                       status = True
        except Exception as e:
            print(f"Error during session establishment: {e}")

        return status
    
    def request(self, req: int, res: bytearray) -> int:
        status = Session.ERROR

        if 0 != int.from_bytes(self.__SESSION_ID, 'little'):
            buffer = bytes([req]) + self.__SESSION_ID
            plen = cipher.AES.block_size - (len(buffer) % cipher.AES.block_size)
            buffer += bytes(plen * [len(buffer)])
            buffer = self.__AES.encrypt(buffer)

            if self.__send(buffer):
                buffer = self.__receive(cipher.AES.block_size)
                if 0 < len(buffer):
                    buffer = self.__AES.decrypt(buffer)
                    if 0 < len(buffer):
                        status = buffer[0]
                        res[:] = buffer[1:]
        return status
    
    def terminate(self) -> bool:
        return (Session.OKAY == self.request(Session.__TERMINATE, bytearray()))
    
    def __del__(self):
        self.terminate()
    
        