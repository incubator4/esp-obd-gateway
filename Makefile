# PlatformIO shortcuts for esp-obd-gateway
# Usage: make [target] [ENV=gateway] [PORT=/dev/cu.usbserial-xxx]

PIO   ?= pio
ENV   ?= gateway
BAUD  ?= 115200
PORT  ?=

PORT_FLAG := $(if $(PORT),--port $(PORT),)

.PHONY: help build upload monitor flash clean ports
.PHONY: gateway gateway-upload gateway-flash
.PHONY: display-c6 display-c6-upload display-c6-flash
.PHONY: display-s3 display-s3-upload display-s3-flash

help: ## 显示可用命令
	@echo "esp-obd-gateway — PlatformIO shortcuts"
	@echo ""
	@echo "通用（默认 ENV=gateway，可用 ENV=display_c6 等覆盖）:"
	@echo "  make build              编译"
	@echo "  make upload             烧录"
	@echo "  make monitor            串口监视 (115200)"
	@echo "  make flash              烧录后打开串口监视"
	@echo "  make clean              清理构建产物"
	@echo "  make ports              列出串口设备"
	@echo ""
	@echo "网关 (4D GEN4-S3):"
	@echo "  make gateway            编译 gateway"
	@echo "  make gateway-upload     烧录 gateway"
	@echo "  make gateway-flash      烧录 + 监视"
	@echo ""
	@echo "显示端 C6 (Waveshare ESP32-C6-LCD-1.3):"
	@echo "  make display-c6         编译 display_c6"
	@echo "  make display-c6-upload  烧录 display_c6"
	@echo "  make display-c6-flash   烧录 + 监视"
	@echo ""
	@echo "显示端 S3 (Waveshare ESP32-S3-Touch-LCD-1.69):"
	@echo "  make display-s3         编译 display_s3"
	@echo "  make display-s3-upload  烧录 display_s3"
	@echo "  make display-s3-flash   烧录 + 监视"
	@echo ""
	@echo "示例:"
	@echo "  make gateway-flash PORT=/dev/cu.usbmodem101"
	@echo "  make upload ENV=display_c6"

build: ## 编译当前环境
	$(PIO) run -e $(ENV)

upload: ## 烧录当前环境
	$(PIO) run -e $(ENV) -t upload $(PORT_FLAG)

monitor: ## 串口监视
	$(PIO) device monitor -b $(BAUD) $(PORT_FLAG)

flash: ## 烧录后串口监视
	$(PIO) run -e $(ENV) -t upload -t monitor $(PORT_FLAG)

clean: ## 清理构建产物
	$(PIO) run -e $(ENV) -t clean

ports: ## 列出可用串口
	$(PIO) device list

# --- gateway ---
gateway:
	$(MAKE) build ENV=gateway

gateway-upload:
	$(MAKE) upload ENV=gateway PORT=$(PORT)

gateway-flash:
	$(MAKE) flash ENV=gateway PORT=$(PORT)

# --- display_c6 ---
display-c6:
	$(MAKE) build ENV=display_c6

display-c6-upload:
	$(MAKE) upload ENV=display_c6 PORT=$(PORT)

display-c6-flash:
	$(MAKE) flash ENV=display_c6 PORT=$(PORT)

# --- display_s3 ---
display-s3:
	$(MAKE) build ENV=display_s3

display-s3-upload:
	$(MAKE) upload ENV=display_s3 PORT=$(PORT)

display-s3-flash:
	$(MAKE) flash ENV=display_s3 PORT=$(PORT)
