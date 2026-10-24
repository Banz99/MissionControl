/*
 * Copyright (c) 2020-2026 ndeadly
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
#include "utils_bluetooth_address.hpp"

namespace ams::utils {

    bool BluetoothAddressCompare(bluetooth::Address addr1, bluetooth::Address addr2) {
        return std::memcmp(&addr1, &addr2, sizeof(bluetooth::Address)) == 0;
    }

    bluetooth::Address BluetoothAddressReverse(bluetooth::Address address) {
        u64 tmp = 0;
        std::memcpy(&tmp, &address, sizeof(address));
        tmp = util::SwapEndian(tmp) >> 16;

        bluetooth::Address out;
        std::memcpy(&out, &tmp, sizeof(address));
        return out;
    }

    Result BluetoothAddressToString(bluetooth::Address address, char *out, size_t out_size) {
        if (out_size < 2*sizeof(bluetooth::Address) + 1) {
            return -1;
        }

        char ch;
        for (u32 i = 0; i < sizeof(bluetooth::Address); ++i) {
            ch = address.address[i] >> 4;
            *out++ = ch + (ch <= 9 ? '0' : 'a' - 0xa);
            ch = address.address[i] & 0x0f;
            *out++ = ch + (ch <= 9 ? '0' : 'a' - 0xa);
        }
        *out = '\0';

        R_SUCCEED();
    }

    void ParseBluetoothAddress(const char *value, bluetooth::Address *out) {
        // Check length of address string is correct
        if (std::strlen(value) != 3*sizeof(bluetooth::Address) - 1) {
            return;
        }

        // Parse bluetooth mac address
        char buf[2 + 1];
        bluetooth::Address address = {};
        for (u32 i = 0; i < sizeof(bluetooth::Address); ++i) {
            // Convert hex pair to number
            std::memcpy(buf, &value[i*3], 2);
            address.address[i] = static_cast<u8>(std::strtoul(buf, nullptr, 16));

            // Check for colon separator
            if ((i < sizeof(bluetooth::Address) - 1) && (value[i*3 + 2] != ':')) {
                return;
            }
        }

        *out = address;
    }

}
