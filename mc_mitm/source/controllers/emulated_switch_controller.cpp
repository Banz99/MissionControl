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
#include "emulated_switch_controller.hpp"
#include "../mcmitm_config.hpp"
#include <memory>

namespace ams::controller {

    namespace {

        // Frequency in Hz rounded to nearest int
        // https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/rumble_data_table.md#frequency-table
        const uint16_t rumble_freq_lut[] = {
            0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031,
            0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0039, 0x003a, 0x003b,
            0x003c, 0x003e, 0x003f, 0x0040, 0x0042, 0x0043, 0x0045, 0x0046, 0x0048,
            0x0049, 0x004b, 0x004d, 0x004e, 0x0050, 0x0052, 0x0054, 0x0055, 0x0057,
            0x0059, 0x005b, 0x005d, 0x005f, 0x0061, 0x0063, 0x0066, 0x0068, 0x006a,
            0x006c, 0x006f, 0x0071, 0x0074, 0x0076, 0x0079, 0x007b, 0x007e, 0x0081,
            0x0084, 0x0087, 0x0089, 0x008d, 0x0090, 0x0093, 0x0096, 0x0099, 0x009d,
            0x00a0, 0x00a4, 0x00a7, 0x00ab, 0x00ae, 0x00b2, 0x00b6, 0x00ba, 0x00be,
            0x00c2, 0x00c7, 0x00cb, 0x00cf, 0x00d4, 0x00d9, 0x00dd, 0x00e2, 0x00e7,
            0x00ec, 0x00f1, 0x00f7, 0x00fc, 0x0102, 0x0107, 0x010d, 0x0113, 0x0119,
            0x011f, 0x0125, 0x012c, 0x0132, 0x0139, 0x0140, 0x0147, 0x014e, 0x0155,
            0x015d, 0x0165, 0x016c, 0x0174, 0x017d, 0x0185, 0x018d, 0x0196, 0x019f,
            0x01a8, 0x01b1, 0x01bb, 0x01c5, 0x01ce, 0x01d9, 0x01e3, 0x01ee, 0x01f8,
            0x0203, 0x020f, 0x021a, 0x0226, 0x0232, 0x023e, 0x024b, 0x0258, 0x0265,
            0x0272, 0x0280, 0x028e, 0x029c, 0x02ab, 0x02ba, 0x02c9, 0x02d9, 0x02e9,
            0x02f9, 0x030a, 0x031b, 0x032c, 0x033e, 0x0350, 0x0363, 0x0376, 0x0389,
            0x039d, 0x03b1, 0x03c6, 0x03db, 0x03f1, 0x0407, 0x041d, 0x0434, 0x044c,
            0x0464, 0x047d, 0x0496, 0x04af, 0x04ca, 0x04e5
        };

        // Floats from dekunukem repo normalised and scaled by function used by yuzu
        // https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/rumble_data_table.md#amplitude-table
        // https://github.com/yuzu-emu/yuzu/blob/d3a4a192fe26e251f521f0311b2d712f5db9918e/src/input_common/sdl/sdl_impl.cpp#L429
        const float rumble_amp_lut_f[] = {
            0.000000, 0.120576, 0.137846, 0.146006, 0.154745, 0.164139, 0.174246,
            0.185147, 0.196927, 0.209703, 0.223587, 0.238723, 0.255268, 0.273420,
            0.293398, 0.315462, 0.321338, 0.327367, 0.333557, 0.339913, 0.346441,
            0.353145, 0.360034, 0.367112, 0.374389, 0.381870, 0.389564, 0.397476,
            0.405618, 0.413996, 0.422620, 0.431501, 0.436038, 0.440644, 0.445318,
            0.450062, 0.454875, 0.459764, 0.464726, 0.469763, 0.474876, 0.480068,
            0.485342, 0.490694, 0.496130, 0.501649, 0.507256, 0.512950, 0.518734,
            0.524609, 0.530577, 0.536639, 0.542797, 0.549055, 0.555413, 0.561872,
            0.568436, 0.575106, 0.581886, 0.588775, 0.595776, 0.602892, 0.610127,
            0.617482, 0.624957, 0.632556, 0.640283, 0.648139, 0.656126, 0.664248,
            0.672507, 0.680906, 0.689447, 0.698135, 0.706971, 0.715957, 0.725098,
            0.734398, 0.743857, 0.753481, 0.763273, 0.773235, 0.783370, 0.793684,
            0.804178, 0.814858, 0.825726, 0.836787, 0.848044, 0.859502, 0.871165,
            0.883035, 0.895119, 0.907420, 0.919943, 0.932693, 0.945673, 0.958889,
            0.972345, 0.986048, 1.000000

        };

        inline void DecodeRumbleValues(const uint8_t enc[], SwitchRumbleData *dec) {
            uint8_t hi_freq_ind = 0x20 + (enc[0] >> 2) + ((enc[1] & 0x01) * 0x40) - 1;
            uint8_t hi_amp_ind  = (enc[1] & 0xfe) >> 1;
            uint8_t lo_freq_ind = (enc[2] & 0x7f) - 1;;
            uint8_t lo_amp_ind  = ((enc[3] - 0x40) << 1) + ((enc[2] & 0x80) >> 7);

            dec->high_band_freq = float(rumble_freq_lut[hi_freq_ind]);
            dec->high_band_amp  = rumble_amp_lut_f[hi_amp_ind];
            dec->low_band_freq  = float(rumble_freq_lut[lo_freq_ind]);
            dec->low_band_amp   = rumble_amp_lut_f[lo_amp_ind];
        }

    }

    EmulatedSwitchController::EmulatedSwitchController(const bluetooth::Address *address)
    : SwitchController(address)
    , m_charging(false)
    , m_battery(BATTERY_MAX) {
        this->ClearControllerState();

        m_colours.body       = {0x32, 0x32, 0x32};
        m_colours.buttons    = {0xe6, 0xe6, 0xe6};
        m_colours.left_grip  = {0x46, 0x46, 0x46};
        m_colours.right_grip = {0x46, 0x46, 0x46};

        mitm::ControllerProfileConfig config;
        mitm::GetCustomIniConfig(address,&config);

        m_enable_rumble = config.general.enable_rumble;
    };

    void EmulatedSwitchController::ClearControllerState(void) {
        std::memset(&m_buttons, 0, sizeof(m_buttons));
        m_left_stick.SetData(STICK_ZERO, STICK_ZERO);
        m_right_stick.SetData(STICK_ZERO, STICK_ZERO);
        std::memset(&m_motion_data, 0, sizeof(m_motion_data));
    }

    Result EmulatedSwitchController::HandleIncomingReport(const bluetooth::HidReport *report) {
        this->UpdateControllerState(report);

        // Prepare Switch report
        s_input_report.size = sizeof(SwitchInputReport0x30) + 1;
        auto switch_report = reinterpret_cast<SwitchReportData *>(s_input_report.data);
        switch_report->id = 0x30;
        switch_report->input0x30.conn_info      = 0;
        switch_report->input0x30.battery        = m_battery | m_charging;
        switch_report->input0x30.buttons        = m_buttons;
        switch_report->input0x30.left_stick     = m_left_stick;
        switch_report->input0x30.right_stick    = m_right_stick;
        std::memcpy(&switch_report->input0x30.motion, &m_motion_data, sizeof(m_motion_data));

        this->ApplyButtonCombos(&switch_report->input0x30.buttons);

        switch_report->input0x30.timer = os::ConvertToTimeSpan(os::GetSystemTick()).GetMilliSeconds() & 0xff;
        return bluetooth::hid::report::WriteHidReportBuffer(&m_address, &s_input_report);
    }

    Result EmulatedSwitchController::HandleOutgoingReport(const bluetooth::HidReport *report) {
        uint8_t cmdId = report->data[0];
        switch (cmdId) {
            case 0x01:
                R_TRY(this->HandleSubCmdReport(report));
                break;
            case 0x10:
                R_TRY(this->HandleRumbleReport(report));
                break;
            default:
                break;
        }

        return ams::ResultSuccess();
    }

    Result EmulatedSwitchController::HandleSubCmdReport(const bluetooth::HidReport *report) {
        auto switch_report = reinterpret_cast<const SwitchReportData *>(&report->data);

        switch (switch_report->output0x01.subcmd.id) {
            case SubCmd_RequestDeviceInfo:
                R_TRY(this->SubCmdRequestDeviceInfo(report));
                break;
            case SubCmd_SpiFlashRead:
                R_TRY(this->SubCmdSpiFlashRead(report));
                break;
            case SubCmd_SpiFlashWrite:
                R_TRY(this->SubCmdSpiFlashWrite(report));
                break;
            case SubCmd_SpiSectorErase:
                R_TRY(this->SubCmdSpiSectorErase(report));
                break;
            case SubCmd_SetInputReportMode:
                R_TRY(this->SubCmdSetInputReportMode(report));
                break;
            case SubCmd_TriggersElapsedTime:
                R_TRY(this->SubCmdTriggersElapsedTime(report));
                break;
            case SubCmd_SetShipPowerState:
                R_TRY(this->SubCmdSetShipPowerState(report));
                break;
            case SubCmd_SetMcuConfig:
                R_TRY(this->SubCmdSetMcuConfig(report));
                break;
            case SubCmd_SetMcuState:
                R_TRY(this->SubCmdSetMcuState(report));
                break;
            case SubCmd_SetPlayerLeds:
                R_TRY(this->SubCmdSetPlayerLeds(report));
                break;
            case SubCmd_SetHomeLed:
                R_TRY(this->SubCmdSetHomeLed(report));
                break;
            case SubCmd_EnableImu:
                R_TRY(this->SubCmdEnableImu(report));
                break;
            case SubCmd_EnableVibration:
                R_TRY(this->SubCmdEnableVibration(report));
                break;
            default:
                break;
        }

        return ams::ResultSuccess();
    }

    Result EmulatedSwitchController::HandleRumbleReport(const bluetooth::HidReport *report) {
        R_SUCCEED_IF(!m_enable_rumble);

        auto report_data = reinterpret_cast<const SwitchReportData *>(report->data);

        SwitchRumbleData rumble_data;
        DecodeRumbleValues(report_data->output0x10.left_motor, &rumble_data);

        return this->SetVibration(&rumble_data);
    }

    Result EmulatedSwitchController::SubCmdRequestDeviceInfo(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x82,
            .id = SubCmd_RequestDeviceInfo,
            .device_info = {
                .fw_ver = {
                    .major = 0x03,
                    .minor = 0x48
                },
                .type = 0x03,
                ._unk0 = 0x02,
                .address = m_address,
                ._unk1 = 0x01,
                ._unk2 = 0x02
            }
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSpiFlashRead(const bluetooth::HidReport *report) {
        // These are read from official Pro Controller
        // @ 0x00006000: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff                            <= Serial
        // @ 0x00006050: 32 32 32 ff ff ff ff ff ff ff ff ff                                        <= RGB colours (body, buttons, left grip, right grip)
        // @ 0x00006080: 50 fd 00 00 c6 0f 0f 30 61 ae 90 d9 d4 14 54 41 15 54 c7 79 9c 33 36 63    <= Factory Sensor and Stick device parameters
        // @ 0x00006098: 0f 30 61 ae 90 d9 d4 14 54 41 15 54 c7 79 9c 33 36 63                      <= Stick device parameters 2. Normally the same with 1, even in Pro Contr.
        // @ 0x00008010: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    <= User Analog sticks calibration
        // @ 0x0000603d: e6 a5 67 1a 58 78 50 56 60 1a f8 7f 20 c6 63 d5 15 5e ff 32 32 32 ff ff ff <= Left analog stick calibration
        // @ 0x00006020: 64 ff 33 00 b8 01 00 40 00 40 00 40 17 00 d7 ff bd ff 3b 34 3b 34 3b 34    <= 6-Axis motion sensor Factory calibration

        auto switch_report = reinterpret_cast<const SwitchReportData *>(&report->data);
        uint32_t read_addr = switch_report->output0x01.subcmd.spi_flash_read.address;
        uint8_t  read_size = switch_report->output0x01.subcmd.spi_flash_read.size;

        SwitchSubcommandResponse response = {
            .ack = 0x90,
            .id = SubCmd_SpiFlashRead,
            .spi_flash_read = {
                .address = read_addr,
                .size = read_size
            }
        };

        if (read_addr == 0x6050) {
            std::memcpy(response.spi_flash_read.data, &m_colours, sizeof(m_colours)); // Set controller colours
        }
        else {
            std::memset(response.spi_flash_read.data, 0xff, read_size); // Console doesn't seem to mind if response is uninitialised data (0xff)
        }

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSpiFlashWrite(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SpiFlashWrite,
            .spi_flash_write = {
                .status = 0x01
            }
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSpiSectorErase(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SpiSectorErase,
            .spi_flash_write = {
                .status = 0x01
            }
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetInputReportMode(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SetInputReportMode
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdTriggersElapsedTime(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x83,
            .id = SubCmd_TriggersElapsedTime
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetShipPowerState(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SetShipPowerState,
            .set_ship_power_state = {
                .enabled = false
            }
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetMcuConfig(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0xa0,
            .id = SubCmd_SetMcuConfig,
            .data = {0x01, 0x00, 0xff, 0x00, 0x03, 0x00, 0x05, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x5c}
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetMcuState(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SetMcuState
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetPlayerLeds(const bluetooth::HidReport *report) {
        const uint8_t *subCmd = &report->data[10];
        uint8_t led_mask = subCmd[1];
        R_TRY(this->SetPlayerLed(led_mask));

        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SetPlayerLeds
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdSetHomeLed(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_SetHomeLed
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdEnableImu(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_EnableImu
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::SubCmdEnableVibration(const bluetooth::HidReport *report) {
        const SwitchSubcommandResponse response = {
            .ack = 0x80,
            .id = SubCmd_EnableVibration
        };

        return this->FakeSubCmdResponse(&response);
    }

    Result EmulatedSwitchController::FakeSubCmdResponse(const SwitchSubcommandResponse *response) {
        s_input_report.size = sizeof(SwitchInputReport0x21) + 1;
        auto report_data = reinterpret_cast<SwitchReportData *>(s_input_report.data);
        report_data->id = 0x21;
        report_data->input0x21.conn_info   = 0;
        report_data->input0x21.battery     = m_battery | m_charging;
        report_data->input0x21.buttons     = m_buttons;
        report_data->input0x21.left_stick  = m_left_stick;
        report_data->input0x21.right_stick = m_right_stick;
        report_data->input0x21.vibrator    = 0;
        std::memcpy(&report_data->input0x21.response, response, sizeof(SwitchSubcommandResponse));
        report_data->input0x21.timer = os::ConvertToTimeSpan(os::GetSystemTick()).GetMilliSeconds() & 0xff;

        //Write a fake response into the report buffer
        return bluetooth::hid::report::WriteHidReportBuffer(&m_address, &s_input_report);
    }

}
