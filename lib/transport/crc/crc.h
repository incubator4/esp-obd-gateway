#pragma once

#include <cstddef>
#include <cstdint>

namespace obd {

uint8_t crc8(const uint8_t* data, size_t len);

}  // namespace obd
