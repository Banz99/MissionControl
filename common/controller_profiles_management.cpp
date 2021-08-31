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
#include "conversions.hpp"
#include "common_io.hpp"
#include "controller_profiles_management.hpp"

namespace ams::profiles {

    namespace {
        constexpr const char cp_default_name[] = "default.ini";
        constexpr const char cp_controller_name[] = "profile.ini";

        bool g_disable_custom_profiles = false;

        int ControllerProfileIniHandler(void *user, const char *section, const char *name, const char *value) {
                auto config = reinterpret_cast<ControllerProfileConfig *>(user);

                if (strcasecmp(section, "general") == 0) {
                    if (strcasecmp(name, "enable_rumble") == 0)
                        io_conversions::ParseBoolean(value, &config->general.enable_rumble);
                    else if (strcasecmp(name, "enable_motion") == 0)
                        io_conversions::ParseBoolean(value, &config->general.enable_motion);
                }
                else if (strcasecmp(section, "colours") == 0) {
                    if (strcasecmp(name, "body") == 0)
                        io_conversions::ParseRGBstring(value, &config->colours.body);
                    else if (strcasecmp(name, "buttons") == 0)
                        io_conversions::ParseRGBstring(value, &config->colours.buttons);
                    else if (strcasecmp(name, "left_grip") == 0)
                        io_conversions::ParseRGBstring(value, &config->colours.left_grip);
                    else if (strcasecmp(name, "right_grip") == 0)
                        io_conversions::ParseRGBstring(value, &config->colours.right_grip);
                }
                else if (strcasecmp(section, "misc") == 0) {
                    if (strcasecmp(name, "use_western_layout") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.use_western_layout);
                    else if (strcasecmp(name, "sony_led_brightness") == 0)
                        io_conversions::ParseBrightness(value, &config->misc.sony_led_brightness);
                    else if (strcasecmp(name, "swap_dpad_lstick") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.swap_dpad_lstick);
                    else if (strcasecmp(name, "dualshock_pollingrate_divisor") == 0)
                        io_conversions::ParsePollingRate(value, &config->misc.dualshock_pollingrate_divisor);
                    else if (strcasecmp(name, "invert_lstick_xaxis") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.invert_lstick_xaxis);
                    else if (strcasecmp(name, "invert_lstick_yaxis") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.invert_lstick_yaxis);
                    else if (strcasecmp(name, "invert_rstick_xaxis") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.invert_rstick_xaxis);
                    else if (strcasecmp(name, "invert_rstick_yaxis") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.invert_rstick_yaxis);
                    else if (strcasecmp(name, "lstick_deadzone") == 0)
                        io_conversions::ParseDeadzone(value, &config->misc.lstick_deadzone);
                    else if (strcasecmp(name, "rstick_deadzone") == 0)
                        io_conversions::ParseDeadzone(value, &config->misc.rstick_deadzone);
                    else if (strcasecmp(name, "disable_home_button") == 0)
                        io_conversions::ParseBoolean(value, &config->misc.disable_home_button);
                }
                else {
                    return 0;
                }

                return 1;
        }

        Result CopyFile(const char *dst_path, const char *src_path) {
            fs::FileHandle src_file, dst_file;

            /* Open the source file and get its size. */
            R_TRY(fs::OpenFile(std::addressof(src_file), src_path, fs::OpenMode_Read));
            ON_SCOPE_EXIT { fs::CloseFile(src_file); };

            s64 file_size;
            R_TRY(fs::GetFileSize(std::addressof(file_size), src_file));

            /* Create the destination file. */
            R_TRY(fs::CreateFile(dst_path, file_size));

            /* Open the destination file. */
            R_TRY(fs::OpenFile(std::addressof(dst_file), dst_path, fs::OpenMode_Write));
            ON_SCOPE_EXIT { fs::CloseFile(dst_file); };

            /* Allocate a buffer with which to copy. */
            constexpr size_t BufferSize = 4_KB;
            void* buffer;
            buffer  = std::malloc(BufferSize);

            /* Repeatedly read until we've copied all the data. */
            s64 offset = 0;
            while (offset < file_size) {
                const size_t read_size = std::min(static_cast<size_t>(file_size - offset),BufferSize);
                R_TRY(fs::ReadFile(src_file, offset, buffer, read_size));
                R_TRY(fs::WriteFile(dst_file, offset, buffer, read_size, fs::WriteOption::None));
                offset += read_size;
            }

            /* Flush the destination file. */
            R_TRY(fs::FlushFile(dst_file));
            free(buffer);
            return ResultSuccess();
        }
    }

    void SetProfilesDisabled(bool disable) {
        g_disable_custom_profiles = disable;
    }

    Result GetCustomIniConfig(const bluetooth::Address *address, ControllerProfileConfig *config) {
        if (g_disable_custom_profiles) {
            return 1;
        }
        char custom_config_path[100] = "";
        io::GetSpecificControllerFolderPath(address, custom_config_path);
        std::strcat(custom_config_path, cp_controller_name);

        bool has_file = false;
        R_TRY(fs::HasFile(&has_file, custom_config_path));
        if (!has_file) {
            char cp_default_location[100] = "";
            io::GetControllersFolderPath(cp_default_location);
            std::strcat(cp_default_location, cp_default_name);
            CopyFile(custom_config_path, cp_default_location);
        }
        /* Open the file. */
        fs::FileHandle file;
        {
            if (R_FAILED(fs::OpenFile(std::addressof(file), custom_config_path, fs::OpenMode_Read))) {
                return 1;
            }
        }
        ON_SCOPE_EXIT { fs::CloseFile(file); };
        util::ini::ParseFile(file, config, ControllerProfileIniHandler);
        return ams::ResultSuccess();
    }
}
