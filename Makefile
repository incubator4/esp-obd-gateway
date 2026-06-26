# PlatformIO shortcuts for esp-obd-gateway
# Usage: make [target] [ENV=gateway] [BOARD=c6_147] [PORT=/dev/cu.usbserial-xxx]

PIO   ?= pio
ENV   ?= gateway
BOARD ?= c6_13
BAUD  ?= 115200
PORT  ?=

PORT_FLAG := $(if $(PORT),--port $(PORT),)

.PHONY: release package-release

release: package-release ## 编译 release 固件并打包到 dist/

package-release: ## 将已编译的 firmware.bin 打包（需先 pio run）
	pio run -e gateway -e display_c6_13 -e display_c6_147 -e display_s3_169
	python3 scripts/package_release.py --version "$$(git describe --tags --always 2>/dev/null | sed 's/^v//' || echo dev)"

.PHONY: gateway gateway-upload gateway-flash gateway-fake gateway-fake-upload gateway-fake-flash
.PHONY: display-c6-13 display-c6-13-upload display-c6-13-flash
.PHONY: display-c6-147 display-c6-147-upload display-c6-147-flash
.PHONY: display-s3-169 display-s3-169-upload display-s3-169-flash
.PHONY: flash-display build-display

help: ## 显示可用命令
	@echo "esp-obd-gateway — PlatformIO shortcuts"
	@echo ""
	@echo "通用（默认 ENV=gateway，可用 ENV=display_c6_147 等覆盖）:"
	@echo "  make build              编译"
	@echo "  make upload             烧录"
	@echo "  make monitor            串口监视 (115200)"
	@echo "  make flash              烧录后打开串口监视"
	@echo "  make clean              清理构建产物"
	@echo "  make ports              列出串口设备"
	@echo ""
	@echo "显示端统一入口（按芯片族 × 屏尺寸，touch 自动探测）:"
	@echo "  make build-display BOARD=c6_13|c6_147|s3_169"
	@echo "  make flash-display BOARD=c6_147 PORT=/dev/cu.usbmodem101"
	@echo ""
	@echo "网关 (4D GEN4-S3):"
	@echo "  make gateway            编译 gateway"
	@echo "  make gateway-upload     烧录 gateway"
	@echo "  make gateway-flash      烧录 + 监视"
	@echo "  make gateway-fake       编译 gateway（fake OBD 数据）"
	@echo "  make gateway-fake-flash 烧录 fake gateway + 监视"
	@echo ""
	@echo "显示端 C6 1.3\" (ESP32-C6-LCD-1.3):"
	@echo "  make display-c6-13         编译 display_c6_13"
	@echo "  make display-c6-13-flash   烧录 + 监视"
	@echo ""
	@echo "显示端 C6 1.47\" (ESP32-C6-LCD/Touch-LCD-1.47):"
	@echo "  make display-c6-147        编译 display_c6_147"
	@echo "  make display-c6-147-flash  烧录 + 监视"
	@echo ""
	@echo "显示端 S3 1.69\" (ESP32-S3-LCD/Touch-LCD-1.69):"
	@echo "  make display-s3-169        编译 display_s3_169"
	@echo "  make display-s3-169-flash  烧录 + 监视"

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

build-display:
	$(PIO) run -e display_$(BOARD)

flash-display:
	$(PIO) run -e display_$(BOARD) -t upload -t monitor $(PORT_FLAG)

# --- gateway ---
gateway:
	$(MAKE) build ENV=gateway

gateway-upload:
	$(MAKE) upload ENV=gateway PORT=$(PORT)

gateway-flash:
	$(MAKE) flash ENV=gateway PORT=$(PORT)

gateway-fake:
	$(MAKE) build ENV=gateway_fake

gateway-fake-upload:
	$(MAKE) upload ENV=gateway_fake PORT=$(PORT)

gateway-fake-flash:
	$(MAKE) flash ENV=gateway_fake PORT=$(PORT)

# --- display_c6_13 ---
display-c6-13:
	$(MAKE) build ENV=display_c6_13

display-c6-13-upload:
	$(MAKE) upload ENV=display_c6_13 PORT=$(PORT)

display-c6-13-flash:
	$(MAKE) flash ENV=display_c6_13 PORT=$(PORT)

# --- display_c6_147 ---
display-c6-147:
	$(MAKE) build ENV=display_c6_147

display-c6-147-upload:
	$(MAKE) upload ENV=display_c6_147 PORT=$(PORT)

display-c6-147-flash:
	$(MAKE) flash ENV=display_c6_147 PORT=$(PORT)

# --- display_s3_169 ---
display-s3-169:
	$(MAKE) build ENV=display_s3_169

display-s3-169-upload:
	$(MAKE) upload ENV=display_s3_169 PORT=$(PORT)

display-s3-169-flash:
	$(MAKE) flash ENV=display_s3_169 PORT=$(PORT)
