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

namespace ams::profiles {

    struct ControllerProfileConfig {
         struct {
            bool enable_rumble;
            bool enable_motion;
        } general;

        controller::SwitchControllerColours colours;

        struct {
            bool use_western_layout;
            uint32_t sony_led_brightness;
            uint32_t dualshock_pollingrate_divisor;
            bool swap_dpad_lstick;
            bool invert_lstick_xaxis;
            bool invert_lstick_yaxis;
            bool invert_rstick_xaxis;
            bool invert_rstick_yaxis;
            float lstick_deadzone;
            float rstick_deadzone;
            bool disable_home_button;
        } misc;
    };

    constexpr ControllerProfileConfig g_cp_global_config = {
        .general = {
            .enable_rumble = true,
            .enable_motion = true
        },
        .colours = controller::g_default_colours_pro_controller,
        .misc = {
            .use_western_layout = false,
            .swap_dpad_lstick = false,
            .invert_lstick_xaxis = false,
            .invert_lstick_yaxis = false,
            .invert_rstick_xaxis = false,
            .invert_rstick_yaxis = false,
            .lstick_deadzone = 0.0f,
            .rstick_deadzone = 0.0f,
            .disable_home_button = false
        }
    };

    void SetProfilesDisabled(bool disable);
    Result GetCustomIniConfig(const bluetooth::Address *address, ControllerProfileConfig *config);

}
