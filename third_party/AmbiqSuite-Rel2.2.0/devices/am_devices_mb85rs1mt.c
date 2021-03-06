//*****************************************************************************
//
//! @file am_devices_mb85rs1mt.c
//!
//! @brief Fujitsu 64K SPI FRAM driver.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Ambiq Micro
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision v2.2.0-7-g63f7c2ba1 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_devices_mb85rs1mt.h"
#include "am_bsp.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
static void *g_pMB85RS1MTIOMHandle;
static uint32_t g_MB85RS1MTCS;

//*****************************************************************************
//
// Generic Command Write function.
//
//*****************************************************************************
static uint32_t
am_device_command_write(uint32_t ui32InstrLen, uint32_t ui32Instr,
                        uint32_t *pData, uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;

    //
    // Create the transaction.
    //
    Transaction.ui32InstrLen    = ui32InstrLen;
    Transaction.ui32Instr       = ui32Instr;
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32TxBuffer   = pData;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

static uint32_t
am_device_command_read(uint32_t ui32InstrLen, uint32_t ui32Instr,
                       uint32_t *pData, uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;

    //
    // Create the transaction.
    //
    Transaction.ui32InstrLen    = ui32InstrLen;
    Transaction.ui32Instr       = ui32Instr;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = pData;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Initialize the mb85rs1mt driver.
//!
//! @param ui32Module     - IOM Module#
//! @param psIOMSettings  - IOM device structure describing the target spiflash.
//!
//! This function should be called before any other am_devices_mb85rs1mt
//! functions. It is used to set tell the other functions how to communicate
//! with the external spiflash hardware.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_init(uint32_t ui32Module, am_hal_iom_config_t *psIOMSettings, void **ppIomHandle)
{
  uint32_t g_CS[AM_REG_IOM_NUM_MODULES] =
  {
      AM_BSP_IOM0_CS_CHNL,
      AM_BSP_IOM1_CS_CHNL,
  #ifndef APOLLO3_FPGA
      AM_BSP_IOM2_CS_CHNL,
      AM_BSP_IOM3_CS_CHNL,
      AM_BSP_IOM4_CS_CHNL,
      AM_BSP_IOM5_CS_CHNL,
  #endif
  };

    if ( ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Configure the IOM pins.
    //
    am_bsp_iom_pins_enable(ui32Module, AM_HAL_IOM_SPI_MODE);

    //
    // Enable fault detection.
    //
#if AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_FAULT_CAPTURE_ENABLE, 0);
#else // AM_APOLLO3_MCUCTRL
    am_hal_mcuctrl_fault_capture_enable();
#endif // AM_APOLLO3_MCUCTRL

    //
    // Initialize the IOM instance.
    // Enable power to the IOM instance.
    // Configure the IOM for Serial operation during initialization.
    // Enable the IOM.
    // HAL Success return is 0
    //
    if (am_hal_iom_initialize(ui32Module, &g_pMB85RS1MTIOMHandle) ||
        am_hal_iom_power_ctrl(g_pMB85RS1MTIOMHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(g_pMB85RS1MTIOMHandle, psIOMSettings) ||
        am_hal_iom_enable(g_pMB85RS1MTIOMHandle))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    else
    {
        *ppIomHandle = g_pMB85RS1MTIOMHandle;
        g_MB85RS1MTCS = g_CS[ui32Module];
        //
        // Return the status.
        //
        return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
    }
}


//*****************************************************************************
//
//! @brief De-Initialize the mb85rs1mt driver.
//!
//! @param ui32Module     - IOM Module#
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_term(uint32_t ui32Module)
{
    if ( ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    // Disable the pins
    am_bsp_iom_pins_disable(ui32Module, AM_HAL_IOM_SPI_MODE);

    //
    // Disable the IOM.
    //
    am_hal_iom_disable(g_pMB85RS1MTIOMHandle);

    //
    // Disable power to and uninitialize the IOM instance.
    //
    am_hal_iom_power_ctrl(g_pMB85RS1MTIOMHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);

    am_hal_iom_uninitialize(g_pMB85RS1MTIOMHandle);

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}


//*****************************************************************************
//
//! @brief Reads the ID of the external flash and returns the value.
//!
//! @param pDeviceID - Pointer to the return buffer for the Device ID.
//!
//! This function reads the device ID register of the external flash, and returns
//! the result as an 32-bit unsigned integer value.
//!
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_read_id(uint32_t *pDeviceID)
{
    //
    // Send the command sequence to read the Device ID.
    //
    if (am_device_command_read(1, AM_DEVICES_MB85RS1MT_READ_DEVICE_ID, pDeviceID, 4))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;

}

//*****************************************************************************
//
//! @brief Reads the current status of the external flash
//!
//! This function reads the status register of the external flash, and returns
//! the result as an 8-bit unsigned integer value. The processor will block
//! during the data transfer process, but will return as soon as the status
//! register had been read.
//!
//! Macro definitions for interpreting the contents of the status register are
//! included in the header file.
//!
//! @return 8-bit status register contents
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_status_get(uint32_t *pStatus)
{
    //
    // Send the command sequence to read the device status.
    //
    if (am_device_command_read(1, AM_DEVICES_MB85RS1MT_READ_STATUS, pStatus, 1))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Sends a specific command to the device (blocking).
//!
//! ui32Cmd = AM_DEVICES_CMD_WREN
//!           AM_DEVICES_CMD_WRDI
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_command_send(uint32_t ui32Cmd)
{
    uint32_t Dummy;

    if ( ui32Cmd == AM_DEVICES_MB85RS1MT_CMD_WREN )
    {
        //
        // Send the command to enable writing.
        //
        if (am_device_command_write(1, AM_DEVICES_MB85RS1MT_WRITE_ENABLE, &Dummy, 0))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }

    }
    else if ( ui32Cmd == AM_DEVICES_MB85RS1MT_CMD_WRDI )
    {
        //
        // Send the command to enable writing.
        //
        if (am_device_command_write(1, AM_DEVICES_MB85RS1MT_WRITE_DISABLE, &Dummy, 0))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }
    }
    else
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;

}

//*****************************************************************************
//
//! @brief Programs the given range of flash addresses.
//!
//! @param ui32DeviceNumber - Device number of the external flash
//! @param pui8TxBuffer - Buffer to write the external flash data from
//! @param ui32WriteAddress - Address to write to in the external flash
//! @param ui32NumBytes - Number of bytes to write to the external flash
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external flash at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target flash
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_blocking_write(uint8_t *pui8TxBuffer,
                                    uint32_t ui32WriteAddress,
                                    uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    uint32_t              Dummy;

    //
    // Send the WRITE ENABLE command to enable writing.
    //
    if (am_device_command_write(1, AM_DEVICES_MB85RS1MT_WRITE_ENABLE, &Dummy, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Write the command to the device.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE;
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = true;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Write the offset (24 bits) to the device.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 3;
    Transaction.ui32Instr       = ui32WriteAddress & 0x00FFFFFF;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Send the command to disable writing.
    //
    if (am_device_command_write(1, AM_DEVICES_MB85RS1MT_WRITE_DISABLE, &Dummy, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Programs the given range of flash addresses.
//!
//! @param ui32DeviceNumber - Device number of the external flash
//! @param pui8TxBuffer - Buffer to write the external flash data from
//! @param ui32WriteAddress - Address to write to in the external flash
//! @param ui32NumBytes - Number of bytes to write to the external flash
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external flash at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target flash
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_write_adv(uint8_t *pui8TxBuffer,
                                           uint32_t ui32WriteAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_iom_callback_t pfnCallback,
                                           void *pCallbackCtxt)
{
    am_hal_iom_transfer_t Transaction;

    //
    // Common transaction parameters
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = ui32PauseCondition;

    //
    // Set up the IOM transaction to send the WREN device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_ENABLE;
    Transaction.ui32NumBytes    = 0;        // WREN CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;
    Transaction.ui32StatusSetClr = 0;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }


    //
    // Set up the IOM transaction to write the FRAM command to the device.
    // This one needs to keep CE asserted (via continue).
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE;
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.bContinue       = true;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    if (ui32NumBytes)
    {
        //
        // Set up the IOM transaction to write the offset (address) and data.
        //
        Transaction.eDirection      = AM_HAL_IOM_TX;
        Transaction.ui32InstrLen    = 3;
        Transaction.ui32Instr       = ui32WriteAddress & 0x00FFFFFF;
        Transaction.ui32NumBytes    = ui32NumBytes;
        Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;
        Transaction.bContinue       = false;
        Transaction.ui32PauseCondition = 0;
        // Now set the post processing condition
        Transaction.ui32StatusSetClr = ui32StatusSetClr;

        //
        // Start the transaction
        //
        if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, pfnCallback, pCallbackCtxt))
        {
            return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
        }
    }
    // No Need to issue AM_DEVICES_MB85RS1MT_WRITE_DISABLE, as WEL gets deasserted
    // following the WRITE transaction

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Programs the given range of flash addresses.
//!
//! @param ui32DeviceNumber - Device number of the external flash
//! @param pui8TxBuffer - Buffer to write the external flash data from
//! @param ui32WriteAddress - Address to write to in the external flash
//! @param ui32NumBytes - Number of bytes to write to the external flash
//!
//! This function uses the data in the provided pui8TxBuffer and copies it to
//! the external flash at the address given by ui32WriteAddress. It will copy
//! exactly ui32NumBytes of data from the original pui8TxBuffer pointer. The
//! user is responsible for ensuring that they do not overflow the target flash
//! memory or underflow the pui8TxBuffer array
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_write(uint8_t *pui8TxBuffer,
                                       uint32_t ui32WriteAddress,
                                       uint32_t ui32NumBytes,
                                       am_hal_iom_callback_t pfnCallback,
                                       void *pCallbackCtxt)
{
#if 0
    am_hal_iom_transfer_t Transaction;

    //
    // Common transaction parameters
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition       = 0;
    Transaction.ui32StatusSetClr = 0;

#if 0
    //
    // Set up the IOM transaction to send the WREN device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_ENABLE;
    Transaction.ui32NumBytes    = 0;        // WREN CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }
#endif

    // Do the transaction (no callback).
    if (am_devices_mb85rs1mt_nonblocking_write_adv(pui8TxBuffer,
                                                   ui32WriteAddress,
                                                   ui32NumBytes,
                                                   0, 0, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction to send the WRDI (write disable) device command.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 1;        // Sending 1 offset byte
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_WRITE_DISABLE;
    Transaction.ui32NumBytes    = 0;        // WRDI CMD is sent as the offset
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer; // Not used for this CMD
    Transaction.bContinue       = false;

    //
    // Start the transaction, sending the callback.
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
#else
    return (am_devices_mb85rs1mt_nonblocking_write_adv(pui8TxBuffer,
                                                   ui32WriteAddress,
                                                   ui32NumBytes,
                                                   0, 0, pfnCallback, pCallbackCtxt));
#endif
}

//*****************************************************************************
//
//! @brief Reads the contents of the fram into a buffer.
//!
//! @param pui8RxBuffer - Buffer to store the received data from the flash
//! @param ui32ReadAddress - Address of desired data in external flash
//! @param ui32NumBytes - Number of bytes to read from external flash
//!
//! This function reads the external flash at the provided address and stores
//! the received data into the provided buffer location. This function will
//! only store ui32NumBytes worth of data.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_blocking_read(uint8_t *pui8RxBuffer,
                                   uint32_t ui32ReadAddress,
                                   uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition       = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Write the command to the device.
    //

    // Corvette RX HW bug, CORVETTE-874. 0-byte xfer must be TX.
    // This HW bug should be covered in the HAL, so test the HAL fix here by
    // attempting to do a 0-byte RX transfer.
//  Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_READ;
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = true;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction to write to the device with offset address.
    //
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 3;
    Transaction.ui32Instr       = ui32ReadAddress & 0x00FFFFFF;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Reads the contents of the fram into a buffer.
//!
//! @param pui8RxBuffer - Buffer to store the received data from the flash
//! @param ui32ReadAddress - Address of desired data in external flash
//! @param ui32NumBytes - Number of bytes to read from external flash
//!
//! This function reads the external flash at the provided address and stores
//! the received data into the provided buffer location. This function will
//! only store ui32NumBytes worth of data.
//
//! @return 32-bit status
//
//*****************************************************************************
uint32_t
am_devices_mb85rs1mt_nonblocking_read(uint8_t *pui8RxBuffer,
                                      uint32_t ui32ReadAddress,
                                      uint32_t ui32NumBytes,
                                      am_hal_iom_callback_t pfnCallback,
                                      void *pCallbackCtxt)
{
    am_hal_iom_transfer_t      Transaction;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Set up the IOM transaction to write the FRAM command to the device.
    // This one needs to keep CE asserted (via continue).
    //

    // Corvette RX HW bug, CORVETTE-874. 0-byte xfer must be TX.
    // This HW bug should be covered in the HAL, so test the HAL fix here by
    // attempting to do a 0-byte RX transfer.
//  Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = AM_DEVICES_MB85RS1MT_READ;
    Transaction.ui32NumBytes    = 0;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = true;

    //
    // Start the transaction (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, 0, 0))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Set up the IOM transaction.
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 3;
    Transaction.ui32Instr       = ui32ReadAddress & 0x00FFFFFF;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32SpiChipSelect = g_MB85RS1MTCS;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RS1MTIOMHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RS1MT_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RS1MT_STATUS_SUCCESS;
}

