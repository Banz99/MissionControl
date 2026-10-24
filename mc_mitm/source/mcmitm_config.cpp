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
#include <stratosphere.hpp>
#include "mcmitm_config.hpp"
#include "utils.hpp"

namespace ams::mitm {

    namespace {

        constexpr const char config_file_location[] = "sdmc:/config/MissionControl/missioncontrol.ini";

        constinit SetLanguage g_system_language;

        constinit MissionControlConfig g_global_config = {
            .general = {
                .disable_custom_profiles=false
            },
            .misc = {
                .dualshock3_enable_usb_pairing = true,
            }
        };

        int ConfigIniHandler(void *user, const char *section, const char *name, const char *value) {
            auto config = reinterpret_cast<MissionControlConfig *>(user);

            if (strcasecmp(section, "general") == 0) {
                if (strcasecmp(name, "disable_custom_profiles") == 0) {
                    utils::ParseBoolean(value, &config->general.disable_custom_profiles);
				}
            } else if (strcasecmp(section, "bluetooth") == 0) {
                if (strcasecmp(name, "host_name") == 0) {
                    std::strncpy(config->bluetooth.host_name, value, sizeof(config->bluetooth.host_name));
                } else if (strcasecmp(name, "host_address") == 0) {
                    utils::ParseBluetoothAddress(value, &config->bluetooth.host_address);
                }
            } else if (strcasecmp(section, "misc") == 0) {
                if (strcasecmp(name, "dualshock3_enable_usb_pairing") == 0) {
                    utils::ParseBoolean(value, &config->misc.dualshock3_enable_usb_pairing);
                }
            } else {
                return 0;
            }

            return 1;
        }

        void ParseIniConfiguration() {
            fs::FileHandle file;
            {
                if (R_FAILED(fs::OpenFile(std::addressof(file), config_file_location, fs::OpenMode_Read))) {
                    return;
                }
            }
            ON_SCOPE_EXIT { fs::CloseFile(file); };

            util::ini::ParseFile(file, &g_global_config, ConfigIniHandler);
        }

        void ReadSystemLanguage() {
            R_ABORT_UNLESS(setInitialize());
            ON_SCOPE_EXIT { setExit(); };
            u64 language_code = 0;
            R_ABORT_UNLESS(setGetSystemLanguage(&language_code));
            R_ABORT_UNLESS(setMakeLanguage(language_code, &g_system_language));
        }

    }

    void LoadConfiguration() {
        ParseIniConfiguration();
        ReadSystemLanguage();
    }

    MissionControlConfig *GetGlobalConfig() {
        return &g_global_config;
    }

    SetLanguage GetSystemLanguage() {
        return g_system_language;
    }

}
