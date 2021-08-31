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
#include "virtual_spi_management.hpp"
#include "../../../common/common_io.hpp"
#include "../../../common/common_structures.hpp"


namespace ams::controller {

    namespace {

        constexpr const char spi_name[] = "spi_flash.bin";
        constexpr const auto spi_flash_size = 0x10000;

        Result InitializeVirtualSpiFlash(const char *path, SwitchAnalogStickFactoryCalibration *lstick_factory_calib, SwitchAnalogStickFactoryCalibration *rstick_factory_calib) {
            fs::FileHandle file;

            // Open the file for write
            R_TRY(fs::OpenFile(std::addressof(file), path, fs::OpenMode_Write));
            ON_SCOPE_EXIT { fs::CloseFile(file); };

            // Fill the file with 0xff
            uint8_t buff[64];
            std::memset(buff, 0xff, sizeof(buff));

            unsigned int offset = 0;
            while (offset < spi_flash_size) {
                size_t write_size = std::min(static_cast<size_t>(spi_flash_size - offset), sizeof(buff));
                R_TRY(fs::WriteFile(file, offset, buff, write_size, fs::WriteOption::None));
                offset += write_size;
            }

            // Write default values for data that the console attempts to read in practice
            const struct {
                SwitchAnalogStickFactoryCalibration lstick_factory_calib;
                SwitchAnalogStickFactoryCalibration rstick_factory_calib;
            } data1 = { *lstick_factory_calib, *rstick_factory_calib };
            R_TRY(fs::WriteFile(file, 0x603d, &data1, sizeof(data1), fs::WriteOption::None));

            R_TRY(fs::WriteFile(file, 0x6050, &g_default_colours_pro_controller, sizeof(g_default_colours_pro_controller), fs::WriteOption::None));

            R_TRY(fs::FlushFile(file));

            return ams::ResultSuccess();
        }

    }

    Result OpenSPIFile(const bluetooth::Address *address, fs::FileHandle *spi_flash_file, SwitchAnalogStickFactoryCalibration *lstick_factory_calib, SwitchAnalogStickFactoryCalibration *rstick_factory_calib) {
        char spi_path[100] = "";
        io::GetSpecificControllerFolderPath(address,spi_path);
        std::strcat(spi_path, spi_name);

        bool file_exists;
        R_TRY(fs::HasFile(&file_exists, spi_path));
        if (!file_exists) {
            // Create file representing first 64KB of SPI flash
            R_TRY(fs::CreateFile(spi_path, spi_flash_size));

            // Initialise the spi flash data
            R_TRY(InitializeVirtualSpiFlash(spi_path, lstick_factory_calib, rstick_factory_calib));
        }

        // Open the virtual spi flash file for read and write
        R_TRY(fs::OpenFile(std::addressof(*spi_flash_file), spi_path, fs::OpenMode_ReadWrite));
        return ams::ResultSuccess();
    }

    Result CloseSPIFile(fs::FileHandle spi_flash_file) {
        fs::FlushFile(spi_flash_file);
        fs::CloseFile(spi_flash_file);
        return ams::ResultSuccess();
    }

}
