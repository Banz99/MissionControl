/*
 * Copyright (c) 2020-2021 ndeadly
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "bluetooth_types.hpp"
#include "common_structures.hpp"

namespace ams::io_conversions {

    void ParseBoolean(const char *value, bool *out);
    void ParseInt(const char *value, int32_t *out);
    void ParsePollingRate(const char *value, uint32_t *out);
    void ParseBrightness(const char *value, uint32_t *out);
    void ParseFloat(const char *value, float *out);
    void ParseDeadzone(const char *value, float *out);
    void ParseRGBstring(const char* value, controller::RGBColour *out);
    Result BluetoothAddressToString(const bluetooth::Address *address, char *out, size_t out_size);
    void ParseBluetoothAddress(const char *value, bluetooth::Address *out);

}
