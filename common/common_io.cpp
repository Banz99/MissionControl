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
#include <stratosphere.hpp>
#include "common_io.hpp"
#include "conversions.hpp"

namespace ams::io {

    namespace {

        constexpr const char controller_base_path[] = "sdmc:/config/MissionControl/controllers/";
    }

    Result GetSpecificControllerFolderPath(const bluetooth::Address *address, char* path) {
            // Check if directory for this controller exists and create it if not
            bool dir_exists;
            std::strcat(path, controller_base_path);
            io_conversions::BluetoothAddressToString(address, path+std::strlen(path), sizeof(path)-std::strlen(path));
            R_TRY(fs::HasDirectory(&dir_exists, path));
            if (!dir_exists) {
                R_TRY(fs::CreateDirectory(path));
            }
            std::strcat(path, "/");
            return ams::ResultSuccess();
    }

    Result GetControllersFolderPath(char* path){
        strcpy(path, controller_base_path);
        return ams::ResultSuccess();
    }
}
