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
#pragma once

struct ControllerProfileConfig {
         struct {
            bool enable_rumble;
            bool enable_motion;
        } general;

        struct {
            int dualshock3_led_mode;
            int dualshock4_polling_rate;
            int dualshock4_lightbar_brightness;
            int dualsense_lightbar_brightness;
            bool dualsense_enable_player_leds;
            int dualsense_vibration_intensity;
            int dualsense_adaptive_trigger_travel;
            int analog_trigger_activation_threshold;
            bool use_western_layout;
            bool swap_dpad_lstick;
            bool invert_lstick_xaxis;
            bool invert_lstick_yaxis;
            bool invert_rstick_xaxis;
            bool invert_rstick_yaxis;
            bool disable_home_button;
        } misc;
};

constexpr ControllerProfileConfig g_cp_global_config = {
    .general = {
        .enable_rumble = true,
        .enable_motion = true
    },
    .misc = {
        .dualshock3_led_mode = 0,
        .dualshock4_polling_rate = 8,
        .dualshock4_lightbar_brightness = 5,
        .dualsense_lightbar_brightness = 5,
        .dualsense_enable_player_leds = true,
        .dualsense_vibration_intensity = 4,
        .dualsense_adaptive_trigger_travel = 0,
        .analog_trigger_activation_threshold = 50,
        .use_western_layout = false,
        .swap_dpad_lstick = false,
        .invert_lstick_xaxis = false,
        .invert_lstick_yaxis = false,
        .invert_rstick_xaxis = false,
        .invert_rstick_yaxis = false,
        .disable_home_button = false
    }
};