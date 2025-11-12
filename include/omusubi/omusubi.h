#pragma once

#include "core/string_view.h"
#include "core/fixed_string.hpp"
#include "core/fixed_buffer.hpp"
#include "core/types.h"

#include "interface/readable.h"
#include "interface/writable.h"
#include "interface/connectable.h"
#include "interface/scannable.h"
#include "interface/pressable.h"
#include "interface/measurable.h"
#include "interface/displayable.h"

#include "device/serial_communication.h"
#include "device/bluetooth_communication.h"
#include "device/wifi_communication.h"
#include "device/ble_communication.h"

#include "system_context.h"
