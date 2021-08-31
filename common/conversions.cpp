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
#include "conversions.hpp"

namespace ams::io_conversions {

    void ParseBoolean(const char *value, bool *out) {
        if (strcasecmp(value, "true") == 0)
            *out = true;
        else if (strcasecmp(value, "false") == 0)
            *out = false;
    }

    void ParseInt(const char *value, int32_t *out) {
        *out = atoi(value);
    }

    void ParsePollingRate(const char *value, uint32_t *out) {
        int32_t temp=8;
        ParseInt(value, &temp);
        if(temp >= 0 && temp <= 16)
            *out = temp;
    }

    void ParseBrightness(const char *value, uint32_t *out) {
        int32_t temp=8;
        ParseInt(value, &temp);
        if(temp >= 0 && temp <= 63)
            *out = temp;
    }

    void ParseFloat(const char *value, float *out) {
        uint32_t val_len = strlen(value);
        float temp = 0.0f;
        bool multiply = true;
        uint32_t divide = 10;
        for (uint32_t i = 0; i < val_len; ++i) {
            if(value[i] >= '0' && value[i] <= '9') {
                temp = multiply ? temp * 10 : temp;
                temp += multiply ? value[i] - '0' : static_cast<float>(value[i] - '0') / divide;
                if(!multiply){
                    divide *= 10;
                }
            }
            else if(value[i] == '.') {
                multiply = false;
            }
            else return;
        }
        *out = temp;
    }

    void ParseDeadzone(const char *value, float *out) {
        float temp=0.0f;
        ParseFloat(value, &temp);
        temp /= 100;
        if (temp > 0.0f && temp < 1.0f)
            *out = temp;
    }

    void ParseRGBstring(const char* value, controller::RGBColour *out) {
        uint8_t temp[3];
        if (std::strncmp(value, "rgb(", 4) == 0) {
             value+=4;
             uint32_t i = 0, k=0;
             uint32_t str_len = strlen(value);
             temp[0] = 0;
             temp[1] = 0;
             temp[2] = 0;
             while (i < str_len && value[i] !=')' && k <= 2){
                if (value[i] >= '0' && value[i] <= '9') {
                     temp[k] *= 10;
                     temp[k] += value[i] - '0';

                }
                else if (value[i] == ','){
                    k++;
                }
                else if (value[i] != ' ') //Ignore spaces if found
                    return;
                i++;
             }
             if (k == 2){
                out->r = temp[0];
                out->g = temp[1];
                out->b = temp[2];
             }
        }
        else if (value[0] == '#') {
            char buf[2 + 1];
            if (strlen(value) >= 7) {
                std::memcpy(buf, value + 1, 2);
                temp[0] = static_cast<uint8_t>(std::strtoul(buf, nullptr, 16));
                std::memcpy(buf, value + 3, 2);
                temp[1] = static_cast<uint8_t>(std::strtoul(buf, nullptr, 16));
                std::memcpy(buf, value + 5, 2);
                temp[2] = static_cast<uint8_t>(std::strtoul(buf, nullptr, 16));
                out->r = temp[0];
                out->g = temp[1];
                out->b = temp[2];
            }
            return;
        }
        return;
    }

    Result BluetoothAddressToString(const bluetooth::Address *address, char *out, size_t out_size) {
        if (out_size < 2*sizeof(bluetooth::Address) + 1)
            return -1;

        char ch;
        for (uint32_t i = 0; i < sizeof(bluetooth::Address); ++i) {
            ch = address->address[i] >> 4;
            *out++ = ch + (ch <= 9 ? '0' : 'a' - 0xa);
            ch = address->address[i] & 0x0f;
            *out++ = ch + (ch <= 9 ? '0' : 'a' - 0xa);
        }
        *out = '\0';

        return ams::ResultSuccess();
    }

    void ParseBluetoothAddress(const char *value, bluetooth::Address *out) {
        // Check length of address string is correct
        if (std::strlen(value) != 3*sizeof(bluetooth::Address) - 1) return;

        // Parse bluetooth mac address
        char buf[2 + 1];
        bluetooth::Address address = {};
        for (uint32_t i = 0; i < sizeof(bluetooth::Address); ++i) {
            // Convert hex pair to number
            std::memcpy(buf, &value[i*3], 2);
            address.address[i] = static_cast<uint8_t>(std::strtoul(buf, nullptr, 16));

            // Check for colon separator
            if ((i < sizeof(bluetooth::Address) - 1) && (value[i*3 + 2] != ':'))
                return;
        }

        *out = address;
    }

}
