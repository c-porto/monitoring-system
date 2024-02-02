#include "include/i2c.hpp"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

extern "C" {

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ /*!< I2C master read */
#define ACK_CHECK_EN 0x1 /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0 /*!< I2C ack value */
#define NACK_VAL 0x1 /*!< I2C nack value */

static const gpio_num_t i2c_gpio_sda = GPIO_NUM_21;
static const gpio_num_t i2c_gpio_scl = GPIO_NUM_22;
static const uint32_t i2c_frequency = 100000;
static const i2c_port_t i2c_port = I2C_NUM_0;

static esp_err_t i2c_master_driver_initialize(void)
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = i2c_gpio_sda,
		.scl_io_num = i2c_gpio_scl,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		//      .master.clk_speed = i2c_frequency,
		//.clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_*
		// flags to choose i2c source clock here. */
	};
	conf.master.clk_speed = i2c_frequency;
	return i2c_param_config(i2c_port, &conf);
}

void init_i2c()
{
	i2c_driver_install(i2c_port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE,
			   I2C_MASTER_TX_BUF_DISABLE, 0);
	if (i2c_master_driver_initialize() != ESP_OK) {
		ESP_LOGI("I2C EXTERNAL DRIVER", "Error Initializing i2c");
	}
}
}
