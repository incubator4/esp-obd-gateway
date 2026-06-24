#pragma once

/** Gateway TWAI ↔ SN65HVD230 默认引脚（4D GEN4-S3 FFC 扩展口） */
#define GW_CAN_TX_PIN 17
#define GW_CAN_RX_PIN 18
#define GW_CAN_RS_PIN -1

/** OBD-II 默认 500 kbps；老车可改为 250000 / 125000 */
#define GW_CAN_BITRATE 500000

/** OBD PID 轮询间隔（ms） */
#define GW_OBD_POLL_MS 100

/** 无 OBD 响应超过此时间视为断开（ms） */
#define GW_OBD_STALE_MS 3000

/** ESP-NOW Wi-Fi 信道（须与 display 端一致） */
#define GW_ESPNOW_CHANNEL 1
