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
#include "utils_stuff.hpp"

namespace ams::utils {

    void ParseBoolean(const char *value, bool *out) {
            if (strcasecmp(value, "true") == 0)
                *out = true;
            else if (strcasecmp(value, "false") == 0)
                *out = false; 
        }

        void ParseInt(const char *value, int *out, int min=INT_MIN, int max=INT_MAX) {
            int tmp = std::strtol(value, nullptr, 10);
            if ((tmp >= min) && (tmp <= max)) {
                *out = tmp;
            }
        }


}
