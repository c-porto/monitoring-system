#include "include/cjmcu-811.hpp"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/i2c_types.h"
#include "i2c.hpp"
#include "utils.hpp"
#include <stdexcept>

#define DEBUG

namespace
{
const char *TAG = "CJMCU-811";
}

namespace sensor
{
namespace __cjmcu811
{
namespace external_lib
{
extern "C" {
#define CCS811_MODE 1

short value;
i2c_cmd_handle_t cmd;

bool init_ccs811()
{
	const uint8_t app_start = 0;

	if (ccs811_read_byte(CCS811_REG_HW_ID) != 0x81)
		return false;

	ccs811_write_byte(CCS811_REG_APP_START, app_start, 0);

	vTaskDelay(100 / portTICK_PERIOD_MS);

	// Define Mode
	const uint8_t mode = CCS811_MODE << 4;
	ccs811_write_byte(
		CCS811_REG_MEAS_MODE, mode,
		8); // Put 'No data' (0) in APP_START register for init sensor
	vTaskDelay(100 / portTICK_PERIOD_MS);

	return true;
}

uint16_t get_ccs811(int a)
{
	ccs811_data d = ccs811_sensor_data(CCS811_REG_ALG_RESULT_DATA);
	switch (a) {
	case 0: /* CO2 */
		return d.dataCO2;
	case 1: /* VOC */
		return d.dataVOC;
	}
	return 0;
}

ccs811_data ccs811_sensor_data(uint8_t addr)
{
	ccs811_data data;
	uint8_t dataCO2_HB, dataCO2_LB, dataVOC_HB, dataVOC_LB;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create(); // Command link Create
	i2c_master_start(cmd); // Start bit
	i2c_master_write_byte(cmd, CCS811_I2C_ADDRESS_1 << 1 | WRITE_BIT,
			      ACK_CHECK_EN); // Write an single byte address
	i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
	i2c_master_stop(cmd); // Stop bit
	i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUMBER, cmd, TICK_DELAY);
	i2c_cmd_link_delete(cmd);
	vTaskDelay(pdMS_TO_TICKS(30));
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, CCS811_I2C_ADDRESS_1 << 1 | READ_BIT,
			      ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &dataCO2_HB, (i2c_ack_type_t)ACK_VAL);
	i2c_master_read_byte(cmd, &dataCO2_LB, (i2c_ack_type_t)ACK_VAL);
	i2c_master_read_byte(cmd, &dataVOC_HB, (i2c_ack_type_t)ACK_VAL);
	i2c_master_read_byte(cmd, &dataVOC_LB, (i2c_ack_type_t)ACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUMBER, cmd, TICK_DELAY);
	i2c_cmd_link_delete(cmd);

	data.dataCO2 = (dataCO2_HB << 8) | dataCO2_LB;
	data.dataVOC = (dataVOC_HB << 8) | dataVOC_LB;

	return data;
}

bool ccs811_write_byte(uint8_t reg_addr, const uint8_t data, const uint8_t len)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, CCS811_I2C_ADDRESS_1 << 1 | WRITE_BIT,
			      ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
	i2c_master_write(cmd, &data, len, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	int ret = i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUMBER, cmd,
				       TICK_DELAY);
	i2c_cmd_link_delete(cmd);
	if (ret == ESP_FAIL) {
		return false;
	}
	return true;
}

uint8_t ccs811_read_byte(uint8_t addr)
{
	uint8_t data;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create(); // Command link Create
	i2c_master_start(cmd); // Start bit
	i2c_master_write_byte(cmd, CCS811_I2C_ADDRESS_1 << 1 | WRITE_BIT,
			      ACK_CHECK_EN); // Write an single byte address
	i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
	i2c_master_stop(cmd); // Stop bit
	i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUMBER, cmd, TICK_DELAY);
	i2c_cmd_link_delete(cmd);
	vTaskDelay(pdMS_TO_TICKS(30));
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, CCS811_I2C_ADDRESS_1 << 1 | READ_BIT,
			      ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data, (i2c_ack_type_t)ACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUMBER, cmd, TICK_DELAY);
	i2c_cmd_link_delete(cmd);

	return data;
}
}
} // namespace external_lib
} // namespace __cjmcu811

void Cjmcu811::init()
{
	if (!(__cjmcu811::external_lib::init_ccs811())) {
		ESP_LOGI(::TAG, "Error Initializing Cjmcu811");
		throw std::runtime_error("Could not initialize Cjmcu");
	}
#ifndef DEBUG
	vTaskDelay(pdMS_TO_TICKS(this->kSensorBurnInTime_));
#endif // DEBUG
}
void Cjmcu811::read(MeasureP ms)
{
	ESP_LOGI(::TAG, "Trying to read Cjmcu811");
	C02_ppm_ =
		static_cast<double>(__cjmcu811::external_lib::get_ccs811(0U));
	ESP_LOGI(::TAG, "Measure was %f ppm", static_cast<float>(C02_ppm_));
	ms->air = C02_ppm_;
}

} // namespace sensor
