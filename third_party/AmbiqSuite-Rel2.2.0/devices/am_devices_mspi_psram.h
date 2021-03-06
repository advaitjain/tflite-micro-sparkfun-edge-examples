//*****************************************************************************
//
//! @file am_devices_mspi_psram.h
//!
//! @brief Micron Serial SPI PSRAM driver.
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

#ifndef AM_DEVICES_MSPI_PSRAM_H
#define AM_DEVICES_MSPI_PSRAM_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global definitions for psram commands
//
//*****************************************************************************
#define AM_DEVICES_MSPI_PSRAM_WRITE             0x02
#define AM_DEVICES_MSPI_PSRAM_READ              0x03
#define AM_DEVICES_MSPI_PSRAM_FAST_READ         0x0B
#define AM_DEVICES_MSPI_PSRAM_QUAD_MODE_ENTER   0x35
#define AM_DEVICES_MSPI_PSRAM_QUAD_WRITE        0x38
#define AM_DEVICES_MSPI_PSRAM_RESET_ENABLE      0x66
#define AM_DEVICES_MSPI_PSRAM_RESET_MEMORY      0x99
#define AM_DEVICES_MSPI_PSRAM_READ_ID           0x9F
#define AM_DEVICES_MSPI_PSRAM_HALF_SLEEP_ENTER  0xC0
#define AM_DEVICES_MSPI_PSRAM_QUAD_READ         0xEB
#define AM_DEVICES_MSPI_PSRAM_QUAD_MODE_EXIT    0xF5

//
// The following definitions are typically specific to a multibit spi psram device.
// They should be tailored
//

#if defined (APS6404L)
//*****************************************************************************
//
// Device specific identification.
//
//*****************************************************************************
#define AM_DEVICES_MSPI_PSRAM_KGD_PASS          0x5D0D
#define AM_DEVICES_MSPI_PSRAM_KGD_FAIL          0x550D

// Page size - limits the bust write/read
#define AM_DEVICES_MSPI_PSRAM_PAGE_SIZE         1024
//#define AM_DEVICES_MSPI_PSRAM_TEST_BLOCK_SIZE   64*1024
#define AM_DEVICES_MSPI_PSRAM_TEST_BLOCK_SIZE   2*1024

// According to APS6404L tCEM restriction, we define maximum bytes for each speed empirically
#define AM_DEVICES_MSPI_PSRAM_48MHZ_MAX_BYTES   128
#define AM_DEVICES_MSPI_PSRAM_24MHZ_MAX_BYTES   64
#define AM_DEVICES_MSPI_PSRAM_16MHZ_MAX_BYTES   32
#define AM_DEVICES_MSPI_PSRAM_12MHZ_MAX_BYTES   16
#define AM_DEVICES_MSPI_PSRAM_8MHZ_MAX_BYTES    8

#endif

//*****************************************************************************
//
// Global definitions for the MSPI instance to use.
//
//*****************************************************************************
#define AM_DEVICES_MSPI_PSRAM_MSPI_INSTANCE     0


//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************
#if 1
  typedef enum
{
    AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS,
    AM_DEVICES_MSPI_PSRAM_STATUS_ERROR
} am_devices_mspi_psram_status_t;
#endif

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern volatile uint32_t g_MSPIInterruptStatus;

extern uint32_t am_devices_mspi_psram_init(uint32_t ui32Module,
                                           am_hal_mspi_dev_config_t *psMSPISettings,
                                           void **pHandle);

extern uint32_t am_devices_mspi_psram_deinit(uint32_t ui32Module, am_hal_mspi_dev_config_t *psMSPISettings);

extern uint32_t am_devices_mspi_psram_id(void);

extern uint32_t am_devices_mspi_psram_reset(void);

extern uint32_t am_devices_mspi_psram_read(uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           bool bWaitForCompletion);
extern uint32_t am_devices_mspi_psram_read_adv(uint8_t *pui8RxBuffer,
                                           uint32_t ui32ReadAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_mspi_callback_t pfnCallback,
                                           void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_write(uint8_t *ui8TxBuffer,
                                            uint32_t ui32WriteAddress,
                                            uint32_t ui32NumBytes,
                                            bool bWaitForCompletion);

extern uint32_t am_devices_mspi_psram_write_adv(uint8_t *puiTxBuffer,
                                           uint32_t ui32WriteAddress,
                                           uint32_t ui32NumBytes,
                                           uint32_t ui32PauseCondition,
                                           uint32_t ui32StatusSetClr,
                                           am_hal_mspi_callback_t pfnCallback,
                                           void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_enable_xip(void);

extern uint32_t am_devices_mspi_psram_disable_xip(void);

extern uint32_t am_devices_mspi_psram_enable_scrambling(void);

extern uint32_t am_devices_mspi_psram_disable_scrambling(void);

extern uint32_t am_devices_mspi_psram_read_hiprio(uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);
extern uint32_t am_devices_mspi_psram_nonblocking_read(uint8_t *pui8RxBuffer,
                           uint32_t ui32ReadAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

extern uint32_t am_devices_mspi_psram_write_hiprio(uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);
extern uint32_t am_devices_mspi_psram_nonblocking_write(uint8_t *pui8TxBuffer,
                           uint32_t ui32WriteAddress,
                           uint32_t ui32NumBytes,
                           am_hal_mspi_callback_t pfnCallback,
                           void *pCallbackCtxt);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_MSPI_PSRAM_H

