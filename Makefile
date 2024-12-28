
SPEED = 115200
PORT = /dev/ttyUSB0

clean:
    @rm -rf server/.pio server/.vscode client/__pycache__
    
client:
    @python3 client/main.py $(PORT):$(SPEED)
    
server:
    @cd server; \
    export PLATFORMIO_BUILD_FLAGS="-DSPEED=$(SPEED)"; \
    pio run -t upload
    
.PHONY: clean client server  