//*****************************************************************************
//
//! @file am_devices_mb85rs1mt.h
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

#ifndef AM_DEVICES_MB85RS1MT_H
#define AM_DEVICES_MB85RS1MT_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions for the commands
//
//*****************************************************************************
#define AM_DEVICES_MB85RS1MT_WRITE_ENABLE       0x06
#define AM_DEVICES_MB85RS1MT_WRITE_DISABLE      0x04
#define AM_DEVICES_MB85RS1MT_READ_STATUS        0x05
#define AM_DEVICES_MB85RS1MT_WRITE_STATUS       0x01
#define AM_DEVICES_MB85RS1MT_READ               0x03
#define AM_DEVICES_MB85RS1MT_WRITE              0x02
#define AM_DEVICES_MB85RS1MT_READ_DEVICE_ID     0x9F

//*****************************************************************************
//
// Global definitions for the status register
//
//*****************************************************************************
#define AM_DEVICES_MB85RS1MT_WPEN        0x80        // Write pending status
#define AM_DEVICES_MB85RS1MT_WEL         0x02        // Write enable latch

//*****************************************************************************
//
// Global definitions for the device id.
//
//*****************************************************************************
#define AM_DEVICES_MB85RS1MT_ID         0x03277F04  //0x047F2703

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
typedef enum
{
    AM_DEVICES_MB85RS1MT_STATUS_SUCCESS,
    AM_DEVICES_MB85RS1MT_STATUS_ERROR
} am_devices_MB85RS1MT_status_t;

#define AM_DEVICES_MB85RS1MT_CMD_WREN   AM_DEVICES_MB85RS1MT_WRITE_ENABLE
#define AM_DEVICES_MB85RS1MT_CMD_WRDI   AM_DEVICES_MB85RS1MT_WRITE_DISABLE

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern uint32_t am_devices_mb85rs1mt_init(uint32_t ui32Module, am_hal_iom_config_t *psIOMSettings, void **ppIomHandle);
extern uint32_t am_devices_mb85rs1mt_term(uint32_t ui32Module);

extern uint32_t am_devices_mb85rs1mt_read_id(uint32_t *pDeviceID);

extern uint32_t am_devices_mb85rs1mt_status_get(uint32_t *pStatus);

extern uint32_t am_devices_mb85rs1mt_command_send(uint32_t ui32Cmd);

extern uint32_t am_devices_mb85rs1mt_blocking_write(uint8_t *ui8TxBuffer,
                                                    uint32_t ui32WriteAddress,
                                                    uint32_t ui32NumBytes);

extern uint32_t am_devices_mb85rs1mt_nonblocking_write(uint8_t *ui8TxBuffer,
                                                       uint32_t ui32WriteAddress,
                                                       uint32_t ui32NumBytes,
                                                       am_hal_iom_callback_t pfnCallback,
                                                       void *pCallbackCtxt);
extern uint32_t am_devices_mb85rs1mt_nonblocking_write_adv(uint8_t *pui8TxBuffer,
                                       uint32_t ui32WriteAddress,
                                       uint32_t ui32NumBytes,
                                       uint32_t ui32PauseCondition,
                                       uint32_t ui32StatusSetClr,
                                       am_hal_iom_callback_t pfnCallback,
                                       void *pCallbackCtxt);

extern uint32_t am_devices_mb85rs1mt_blocking_read(uint8_t *pui8RxBuffer,
                                                   uint32_t ui32ReadAddress,
                                                   uint32_t ui32NumBytes);

extern uint32_t am_devices_mb85rs1mt_nonblocking_read(uint8_t *pui8RxBuffer,
                                                      uint32_t ui32ReadAddress,
                                                      uint32_t ui32NumBytes,
                                                      am_hal_iom_callback_t pfnCallback,
                                                      void *pCallbackCtxt);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_MB85RS1MT_H

