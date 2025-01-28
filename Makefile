SPEED = 115200
PORT = /dev/ttyUSB0

client:
	@python3 client/main.py $(PORT) $(SPEED)

server:
	@cd server; \
	export PLATFORMIO_BUILD_FLAGS="-DSPEED=$(SPEED)"; \
	pio run -t upload

clean:
	@rm -rf server/.pio server/.vscode client/__pycache__

.PHONY: clean client server

