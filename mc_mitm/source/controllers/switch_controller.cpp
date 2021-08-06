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
#include "switch_controller.hpp"

namespace ams::controller {

    namespace {

        const uint8_t led_player_mappings[] = {
            SwitchPlayerNumber_Unknown, //0000
            SwitchPlayerNumber_One,     //0001
            SwitchPlayerNumber_Unknown, //0010
            SwitchPlayerNumber_Two,     //0011
            SwitchPlayerNumber_Unknown, //0100
            SwitchPlayerNumber_Six,     //0101
            SwitchPlayerNumber_Eight,   //0110
            SwitchPlayerNumber_Three,   //0111
            SwitchPlayerNumber_One,     //1000
            SwitchPlayerNumber_Five,    //1001
            SwitchPlayerNumber_Six,     //1010
            SwitchPlayerNumber_Seven,   //1011
            SwitchPlayerNumber_Two,     //1100
            SwitchPlayerNumber_Seven,   //1101
            SwitchPlayerNumber_Three,   //1110
            SwitchPlayerNumber_Four,    //1111
        };

    }

    Result LedsMaskToPlayerNumber(uint8_t led_mask, uint8_t *player_number) {
        *player_number = led_player_mappings[led_mask & 0xf];
        if (*player_number == SwitchPlayerNumber_Unknown)
            return -1;

        return ams::ResultSuccess();
    }

    bluetooth::HidReport SwitchController::s_input_report;
    bluetooth::HidReport SwitchController::s_output_report;

    Result SwitchController::HandleIncomingReport(const bluetooth::HidReport *report) {
        s_input_report.size = report->size;
	    std::memcpy(s_input_report.data, report->data, report->size);

        auto switch_report = reinterpret_cast<SwitchReportData *>(s_input_report.data);
        if (switch_report->id == 0x30) {
            this->ApplyButtonCombos(&switch_report->input0x30.buttons);
            /* TODO: Apply those when there are profiles working here as well
            this->ApplyButtonHoldandTurboMask(&switch_report->input0x30.buttons);
            this->ApplyButtonInversionMask(&switch_report->input0x30.buttons);
            */
        }

        return bluetooth::hid::report::WriteHidReportBuffer(&m_address, &s_input_report);
    }

    Result SwitchController::HandleOutgoingReport(const bluetooth::HidReport *report) {
        return bluetooth::hid::report::SendHidReport(&m_address, report);
    }

    void SwitchController::ApplyButtonCombos(SwitchButtonData *buttons) {
        // Home combo = MINUS + DPAD_DOWN
        if (buttons->minus && buttons->dpad_down) {
            buttons->home = 1;
            buttons->minus = 0;
            buttons->dpad_down = 0;
        }

        // Capture combo = MINUS + DPAD_UP
        if (buttons->minus && buttons->dpad_up) {
            buttons->capture = 1;
            buttons->minus = 0;
            buttons->dpad_up = 0;
        }
    }

    void SwitchController::ApplyButtonInversionMask(SwitchButtonData *buttons) {
        uint32_t buttonsbits = (*reinterpret_cast<uint32_t*>(buttons)) & 0xffffff;
        buttonsbits = buttonsbits ^ m_inversion_enable_mask;
        *buttons = *(reinterpret_cast<SwitchButtonData*>(&buttonsbits));
    }

    void SwitchController::ApplyButtonHoldandTurboMask(SwitchButtonData *buttons) {
        uint32_t buttonsbits = (*reinterpret_cast<uint32_t*>(buttons)) & 0xffffff;
        uint32_t hold_state = m_button_holding_state;
        m_button_holding_state = buttonsbits;
        buttonsbits = (~m_hold_enable_mask & ~m_turbo_enable_mask & buttonsbits) | (~m_hold_enable_mask & ~m_previous_button_state_2 & buttonsbits) | (m_hold_enable_mask & m_previous_button_state & ~buttonsbits) | (m_hold_enable_mask & m_previous_button_state & hold_state) | (~m_turbo_enable_mask & ~m_previous_button_state & ~hold_state & buttonsbits); //http://www.32x8.com/sop6_____A-B-C-D-E-F_____m_1-3-5-7-9-11-13-15-17-20-22-23-25-28-30-31-33-35-37-39_____d_48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63_____option-0_____989781966478857295708 (A = turbo B = hold C = prev_state_2 D = prev_state E = holding_btns F = input)
        m_previous_button_state_2 = m_previous_button_state;
        m_previous_button_state = buttonsbits;
        *buttons = *(reinterpret_cast<SwitchButtonData*>(&buttonsbits));
    }

}
