/****************************************************************************
 * Copyright (c) 2015 Mark Charlebois. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @file
 * The declarations in this file are released to DspAL users and are used to
 * make file I/O call's for access to slave devices on a SPI bus.  Many of the
 * data structures are used in the parameter of the IOCTL functions to define the
 * behavior of the particular IOCTL.
 *
 * Sample source files are referenced to demonstrate how a particular IOCTL
 * or data structure is used in the context of the POSIX standard file I/O functions
 * (open/close/read/write/ioctl).
 *
 * @par Reading SPI Data
 * To read data that has accumulated since the last call to read (see the rx_func_ptr_t to define an
 * optional receive data callback) the buffer parameter of the read function must reference a buffer
 * large enough to contain all of the accumulated data.  If the buffer is not large enough, some portion of the
 * accumulated will be copied to the buffer.  The actual length of the data copied to the caller's buffer is
 * specified in the return value of the read function.
 *
 * @par Writing UART Data
 * To write data to the SPI bus slave device a buffer parameter containing the data to be transmitted
 * must be passed to the write function.  After the data is queued for transmit, the write function will
 * return immediately to the caller.
 *
 * @note [Not Yet Implemented]
 * The tx_data_callback member of the dspal_serial_open_options structure can be used to be receive
 * notification of when all queued data has been transmitted.  This can be used as an alternative to
 * setting the is_tx_data_synchronous member to true.
 *
 * @note [Not Yet Implemented]
 * If the is_tx_data_synchronous member of the dspal_spi_ioctl_set_options structure is set
 * to true, the transmit function will only return when all data in the transmit queue has
 * been transmitted.
 *
 * @par
 * Sample source code for read/write data to a SPI slave device is included below:
 * @include spi_test_imp.c
 */

/**
 * @brief
 * List of IOCTL's used for setting SPI options and requesting certain SPI operations that
 * are not performed by the read/write calls.
 */
enum DSPAL_SPI_IOCTLS
{
   SPI_IOCTL_INVALID = -1,   /**< invalid IOCTL code, used to return an error */
   SPI_IOCTL_SET_OPTIONS,    /**< used to configure certain options for communicating on the SPI bus */
   SPI_IOCTL_LOOPBACK_TEST,  /**< activate the internal loopback test mode of the spi bus */
   SPI_IOCTL_RDWR,           /**< used to initiate a write/read batch transfer */
   SPI_IOCTL_SET_BUS_FREQUENCY_IN_HZ,  /**< use this to set the SPI bus speed in HZ */
   SPI_IOCTL_SET_GPIO_INTERRUPT,  /**< enable/disable data ready interrupt for SPI device. */
   SPI_IOCTL_MAX_NUM,        /**< number of valid IOCTL codes defined for the I2C bus */
};

/**
 * @brief
 * List of valid loopback states.
 */
enum DSPAL_SPI_LOOPBACK_TEST_STATE
{
   SPI_LOOPBACK_STATE_UNCONFIGURED, /**< initial loopback state indicating that it is neither enabled, nor disabled */
   SPI_LOOPBACK_STATE_DISABLED,     /**< specifies that the loopback state should be disabled */
   SPI_LOOPBACK_STATE_ENABLED,      /**< specifies that the loopback state should be enabled */
};

/**
 * Callback function indicating that new data has been received and is ready to be read.
 * @param event
 * Reserved for future use.
 * @param
 * Reserved for future use.
 */
typedef void (*spi_rx_func_ptr_t)(int event, void *);

/**
 * Callback function used to indicate that the transmission of all enqueued data is
 * completed.
 * @param event
 * Reserved for future use.
 * @param
 * Reserved for future use.
 */
typedef void (*spi_tx_func_ptr_t)(int event, void *);


/**
 * Structure passed to the SPI_IOCTL_SET_BUS_FREQUENCY_IN_HZ IOCTL call.  Specifies the
 * speed of the SPI bus communcations to the slave device.
 */
struct dspal_spi_ioctl_set_bus_frequency
{
   uint32_t bus_frequency_in_hz;  /**< the maximum speed of the bus for high speed data transfers */
};

/**
 * Structure passed to the SPI_IOCTL_SET_OPTIONS IOCTL call.  Specifies certain SPI bus options and capabilities.
 *
 * TODO: Add a void* parameter to this structure to allow the caller to specify the value
 * of the void* passed in the tx_data_callback and rx_data_callback.
 */
struct dspal_spi_ioctl_set_options
{
   uint32_t slave_address;  /**< the address of the slave device to communicate with */
   int is_tx_data_synchronous; /**< not yet supported, should the transmit data callback be called to indicate when data is fully transmitted */
   spi_tx_func_ptr_t tx_data_callback; /**< optional, not yet supported, called when transmit transfer is complete */
   spi_rx_func_ptr_t rx_data_callback; /**< optional, not yet supported, called when new data is ready to be read */
};

/**
 * Structure passed to the SPI_IOCTL_RDWR IOCTL call.  Specifies the address and length of the
 * read and write buffers used in a combined read/write operation on the SPI bus.
 */
struct dspal_spi_ioctl_read_write
{
   void *read_buffer;  		/**< the address of the buffer used for data read from the slave device. */
   uint32_t read_buffer_length; /**< the length of the buffer referenced by the read_buffer parameter. */
   void *write_buffer; 		/**< the address of the buffer containing the data to write to the slave device. */
   uint32_t write_buffer_length;/**< the length of the buffer referenced by the write_buffer paarameter. */
};

/**
 * Structure passed to the SPI_IOCTL_LOOPBACK_TEST call. Specifies the desired state of the loopback
 * test mode.
 */
struct dspal_spi_ioctl_loopback
{
   enum DSPAL_SPI_LOOPBACK_TEST_STATE state; /**< the state indicating if loopback mode is enabled or disabled. */
};

/**
 * gpio interrupt callback function pointer
 * @param context
 * the parameter passed to the callback when gpio interrupt fires
 */
typedef void (*spi_gpio_int_func_ptr_t)(void* context);

/**
 * Structure passed to the SPI_IOCTL_SET_GPIO_INTERRUPT call. Configure the
 * GPIO interrupt or disable the interrupt.
 *
 * TODO: This is actually for interrupt configuration on GPIO device. We
 * borrow the spi sys_ioctl for now. When GPIO Dspal wrapper is available,
 * we may need to consolidate this logic into GPIO Dspal wrapper.
 */
struct dspal_spi_ioctl_set_gpio_interrupt
{
   bool enable_gpio_interrupt;  /**< indicate whether this ioctl op should enable or disable the gpio interrupt */
   int gpio_int_dev_id; /**< id of GPIO device used for interrupt */
   spi_gpio_int_func_ptr_t gpio_int_callback;  /*< interrupt callback functtion pointer. if enable_gpio_interrupt is false, this variable is DON'T CARE */
   void* user_context;
};
