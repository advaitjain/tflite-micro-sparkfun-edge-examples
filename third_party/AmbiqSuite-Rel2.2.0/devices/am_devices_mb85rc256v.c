//*****************************************************************************
//
//! @file am_devices_mb85rc256v.c
//!
//! @brief Fujitsu 256K I2C FRAM driver.
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
#include "am_devices_mb85rc256v.h"
#include "am_bsp.h"

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
static void *g_pMB85RC256VIOMHandle;

//*****************************************************************************
//
// Generic Command Write function.
//
//*****************************************************************************
static uint32_t
am_device_command_write(uint8_t ui8DevAddr, uint32_t ui32InstrLen,
                        uint32_t ui32Instr, bool bCont,
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
    Transaction.uPeerInfo.ui32I2CDevAddr = ui8DevAddr;
    Transaction.bContinue       = bCont;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RC256VIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Generic Command Read function.
//
//*****************************************************************************
static uint32_t
am_device_command_read(uint8_t ui8DevAddr, uint32_t ui32InstrLen, uint32_t ui32Instr,
                       bool bCont, uint32_t *pData, uint32_t ui32NumBytes)
{
    am_hal_iom_transfer_t  Transaction;

    //
    // Create the transaction.
    //
    Transaction.ui32InstrLen    = ui32InstrLen;
    Transaction.ui32Instr       = ui32Instr;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = pData;
    Transaction.uPeerInfo.ui32I2CDevAddr = ui8DevAddr;
    Transaction.bContinue       = bCont;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    //
    // Execute the transction over IOM.
    //
    if (am_hal_iom_blocking_transfer(g_pMB85RC256VIOMHandle, &Transaction))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Initialize the mb85rc256v driver.
//!
//! @param psIOMSettings  - IOM device structure describing the target spiflash.
//! @param pDMACtrlBuffer - DMA Transfer Control Buffer.
//!
//! This function should be called before any other am_devices_mb85rc256v
//! functions. It is used to set tell the other functions how to communicate
//! with the external spiflash hardware.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_mb85rc256v_init(uint32_t ui32Module, am_hal_iom_config_t *psIOMSettings, void **ppIomHandle)
{
    if ( ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

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
    //
    if (am_hal_iom_initialize(ui32Module, &g_pMB85RC256VIOMHandle) ||
        am_hal_iom_power_ctrl(g_pMB85RC256VIOMHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(g_pMB85RC256VIOMHandle, psIOMSettings) ||
        am_hal_iom_enable(g_pMB85RC256VIOMHandle))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }
    else
    {
        //
        // Configure the IOM pins.
        //
        am_bsp_iom_pins_enable(ui32Module, AM_HAL_IOM_I2C_MODE);


        *ppIomHandle = g_pMB85RC256VIOMHandle;
        //
        // Return the status.
        //
        return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
    }
}

//*****************************************************************************
//
//! @brief De-Initialize the mb85rc256v driver.
//!
//! @param ui32Module     - IOM Module#
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
uint32_t
am_devices_mb85rc256v_term(uint32_t ui32Module)
{
    if ( ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Disable the IOM.
    //
    am_hal_iom_disable(g_pMB85RC256VIOMHandle);

    //
    // Disable power to and uninitialize the IOM instance.
    //
    am_hal_iom_power_ctrl(g_pMB85RC256VIOMHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);

    am_hal_iom_uninitialize(g_pMB85RC256VIOMHandle);

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
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
am_devices_mb85rc256v_read_id(uint32_t *pDeviceID)
{
    //
    // Send the command sequence to read the Device ID.
    //
    if (am_device_command_read(AM_DEVICES_MB85RC256V_RSVD_SLAVE_ID, 1,
                           (AM_DEVICES_MB85RC256V_SLAVE_ID << 1),
                           false, pDeviceID, 3))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
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
am_devices_mb85rc256v_blocking_write(uint8_t *pui8TxBuffer,
                                     uint32_t ui32WriteAddress,
                                     uint32_t ui32NumBytes)
{
    //
    // Write the data to the device.
    //
    if (am_device_command_write(AM_DEVICES_MB85RC256V_SLAVE_ID, 2,
                            (ui32WriteAddress & 0x0000FFFF),
                            false, (uint32_t *)pui8TxBuffer, ui32NumBytes))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
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
am_devices_mb85rc256v_nonblocking_write(uint8_t *pui8TxBuffer,
                                        uint32_t ui32WriteAddress,
                                        uint32_t ui32NumBytes,
                                        am_hal_iom_callback_t pfnCallback,
                                        void *pCallbackCtxt)
{
    am_hal_iom_transfer_t         Transaction;

    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.uPeerInfo.ui32I2CDevAddr = AM_DEVICES_MB85RC256V_SLAVE_ID;
    Transaction.bContinue       = false;

    //
    // Set up the IOM transaction.
    //
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32InstrLen    = 2;
    Transaction.ui32Instr       = ui32WriteAddress & 0x0000FFFF;
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32TxBuffer   = (uint32_t *)pui8TxBuffer;

    //
    // Add this transaction to the command queue (no callback).
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RC256VIOMHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
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
am_devices_mb85rc256v_blocking_read(uint8_t *pui8RxBuffer,
                                    uint32_t ui32ReadAddress,
                                    uint32_t ui32NumBytes)
{
    if (am_device_command_read(AM_DEVICES_MB85RC256V_SLAVE_ID, 2,
                           (ui32ReadAddress & 0x0000FFFF),
                           false, (uint32_t *)pui8RxBuffer, ui32NumBytes))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
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
am_devices_mb85rc256v_nonblocking_read(uint8_t *pui8RxBuffer,
                                       uint32_t ui32ReadAddress,
                                       uint32_t ui32NumBytes,
                                       am_hal_iom_callback_t pfnCallback,
                                       void * pCallbackCtxt)
{
    am_hal_iom_transfer_t Transaction;

    //
    // Set up the IOM transaction.
    //
    Transaction.ui8Priority     = 1;        // High priority for now.
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32InstrLen    = 2;
    Transaction.ui32Instr       = (ui32ReadAddress & 0x0000FFFF);
    Transaction.ui32NumBytes    = ui32NumBytes;
    Transaction.pui32RxBuffer   = (uint32_t *)pui8RxBuffer;
    Transaction.uPeerInfo.ui32I2CDevAddr = AM_DEVICES_MB85RC256V_SLAVE_ID;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.bContinue       = false;

    //
    // Start the transaction.
    //
    if (am_hal_iom_nonblocking_transfer(g_pMB85RC256VIOMHandle, &Transaction, pfnCallback, pCallbackCtxt))
    {
        return AM_DEVICES_MB85RC256V_STATUS_ERROR;
    }

    //
    // Return the status.
    //
    return AM_DEVICES_MB85RC256V_STATUS_SUCCESS;
}
