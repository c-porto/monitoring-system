/*
 *  The external_lib namespace basically encapsulate the code from 
 *  https://github.com/nehlsen/AHT10-IDF/
*/

#include "include/aht10.hpp"
#include <driver/i2c.h>
#include <esp_log.h>
#include <cstring>

namespace
{
static const char *TAG = "__aht10";
}

#define CONFIG_AHT10_I2C_CLOCK_SPEED 100000

#define AHT10_INIT_CMD 0xE1 //initialization command for __aht10
#define AHT10_START_MEASURMENT_CMD 0xAC //start measurment command
#define AHT10_NORMAL_CMD \
	0xA8 //normal cycle mode command, no info in datasheet!!!
#define AHT10_SOFT_RESET_CMD 0xBA //soft reset command

#define AHT10_INIT_NORMAL_MODE 0x00 //enable normal mode
#define AHT10_INIT_CYCLE_MODE 0x20 //enable cycle mode
#define AHT10_INIT_CMD_MODE 0x40 //enable command mode
#define AHT10_INIT_CAL_ENABLE 0x08 //load factory calibration coeff

#define AHT10_DATA_MEASURMENT_CMD \
	0x33 //no info in datasheet!!! my guess it is DAC resolution, saw someone send 0x00 instead
#define AHT10_DATA_NOP 0x00 //no info in datasheet!!!

#define AHT10_MEASURMENT_DELAY 80 //at least 75 milliseconds
#define AHT10_POWER_ON_DELAY 40 //at least 20..40 milliseconds
#define AHT10_CMD_DELAY 350 //at least 300 milliseconds, no info in datasheet!!!
#define AHT10_SOFT_RESET_DELAY 20 //less than 20 milliseconds

#if defined(CONFIG_AHT10_I2C_PORT_NUMBER_1)
static const i2c_port_t I2CPortNumber = I2C_NUM_0;
#else
static const i2c_port_t I2CPortNumber = I2C_NUM_1;
#endif

#if defined(CONFIG_AHT10_I2C_ADDRESS_0X39)
static const uint8_t I2CAddress = 0x39;
#else
static const uint8_t I2CAddress = 0x38;
#endif

namespace sensor
{
namespace external_lib
{
__aht10::__aht10(int i2cPinSda, int i2cPinScl)
{
	memset(&m_rawData, 0, 6 * sizeof(uint8_t));

	initBus(i2cPinSda, i2cPinScl);
	activateOneTimeMode();
	readStatusByte();
	loadFactoryCalibration();
}

void __aht10::initBus(int i2cPinSda, int i2cPinScl)
{
	ESP_LOGV(TAG, "initBus(pindSda:%d,pinScl:%d)", i2cPinSda, i2cPinScl);

	i2c_config_t config;
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = static_cast<gpio_num_t>(i2cPinSda);
	config.sda_pullup_en = GPIO_PULLUP_ENABLE;
	config.scl_io_num = static_cast<gpio_num_t>(i2cPinScl);
	config.scl_pullup_en = GPIO_PULLUP_ENABLE;
	config.master.clk_speed = CONFIG_AHT10_I2C_CLOCK_SPEED;
	config.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_param_config(I2CPortNumber, &config));
	ESP_ERROR_CHECK_WITHOUT_ABORT(
		i2c_driver_install(I2CPortNumber, config.mode, 0, 0, 0));
	vTaskDelay(pdMS_TO_TICKS(AHT10_POWER_ON_DELAY));
}

bool __aht10::activateOneTimeMode()
{
	ESP_LOGV(TAG, "activateOneTimeMode()");
	return write3byte(AHT10_NORMAL_CMD, AHT10_DATA_NOP, AHT10_DATA_NOP);
}

bool __aht10::activateContinuousMode()
{
	ESP_LOGV(TAG, "activateContinuousMode()");
	return write3byte(AHT10_INIT_CMD,
			  AHT10_INIT_CYCLE_MODE | AHT10_INIT_CAL_ENABLE,
			  AHT10_DATA_NOP);
}

bool __aht10::loadFactoryCalibration()
{
	ESP_LOGV(TAG, "loadFactoryCalibration()");
	return write3byte(
		       AHT10_INIT_CMD, // FIXME this seems to be the "real" init
		       AHT10_INIT_CAL_ENABLE, AHT10_DATA_NOP) &&
	       isCalibrated();
}

void __aht10::softReset()
{
	uint8_t bytes = AHT10_SOFT_RESET_CMD;
	writeBytes(&bytes, 1);
	vTaskDelay(pdMS_TO_TICKS(AHT10_SOFT_RESET_DELAY));
	activateOneTimeMode();
	loadFactoryCalibration();
}

bool __aht10::write3byte(uint8_t byte0, uint8_t byte1, uint8_t byte2)
{
	uint8_t bytes[] = { byte0, byte1, byte2 };
	return writeBytes(&bytes[0], 3);
}

bool __aht10::writeBytes(uint8_t *bytes, uint8_t numberOfBytes)
{
	ESP_LOGV(TAG, "writeBytes(), numberOfBytes:%d", numberOfBytes);

	i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
	if (commandHandle == nullptr) {
		ESP_LOGE(TAG,
			 "writeBytes(), failed to acquire i2c command handle");
		return false;
	}

	i2c_master_start(commandHandle);
	i2c_master_write_byte(commandHandle,
			      (I2CAddress << 1) | I2C_MASTER_WRITE, true);
	for (uint8_t n = 0; n < numberOfBytes; ++n) {
		i2c_master_write_byte(commandHandle, *(bytes + n), true);
	}
	i2c_master_stop(commandHandle);
	bool i2cCommandResult =
		i2c_master_cmd_begin(I2CPortNumber, commandHandle,
				     pdMS_TO_TICKS(1000)) == ESP_OK;
	i2c_cmd_link_delete(commandHandle);
	vTaskDelay(pdMS_TO_TICKS(AHT10_CMD_DELAY));

	ESP_LOGD(TAG, "writeBytes(), Result: %s",
		 (i2cCommandResult ? "OK" : "FAIL"));
	return i2cCommandResult;
}

bool __aht10::readRawData()
{
	ESP_LOGV(TAG, "readRawData()");

	if (!write3byte(AHT10_START_MEASURMENT_CMD, AHT10_DATA_MEASURMENT_CMD,
			AHT10_DATA_NOP)) {
		return false;
	}

	if (!isCalibrated()) {
		ESP_LOGW(TAG, "readRawData(), NOT calibrated");
		//        return false;
	}
	if (isBusy()) {
		vTaskDelay(pdMS_TO_TICKS(AHT10_MEASURMENT_DELAY));
		ESP_LOGD(TAG, "readRawData(), isBusy!, now? %s / status:%d",
			 (isBusy() ? "Y" : "N"), m_statusByte);
	}

	i2c_cmd_handle_t CommandHandle = i2c_cmd_link_create();
	if (CommandHandle == nullptr) {
		ESP_LOGE(TAG,
			 "readRawData(), failed to acquire i2c command handle");
		return false;
	}

	uint8_t data[6];
	i2c_master_start(CommandHandle);
	i2c_master_write_byte(CommandHandle,
			      (I2CAddress << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(CommandHandle, &data[0], I2C_MASTER_ACK);
	i2c_master_read_byte(CommandHandle, &data[1], I2C_MASTER_ACK);
	i2c_master_read_byte(CommandHandle, &data[2], I2C_MASTER_ACK);
	i2c_master_read_byte(CommandHandle, &data[3], I2C_MASTER_ACK);
	i2c_master_read_byte(CommandHandle, &data[4], I2C_MASTER_ACK);
	i2c_master_read_byte(CommandHandle, &data[5], I2C_MASTER_NACK);
	i2c_master_stop(CommandHandle);

	bool Result = i2c_master_cmd_begin(I2CPortNumber, CommandHandle,
					   pdMS_TO_TICKS(1000)) == ESP_OK;
	i2c_cmd_link_delete(CommandHandle);

	ESP_LOGD(TAG, "readRawData(), READ Result: %s",
		 (Result ? "OK" : "FAIL"));

	if (Result) {
		memcpy(m_rawData, data, 6);
	}

	return Result;
}

bool __aht10::readStatusByte()
{
	ESP_LOGV(TAG, "readStatusByte()");

	i2c_cmd_handle_t commandHandle = i2c_cmd_link_create();
	if (commandHandle == nullptr) {
		ESP_LOGE(
			TAG,
			"readStatusByte(), failed to acquire i2c command handle");
		return false;
	}

	i2c_master_start(commandHandle);
	i2c_master_write_byte(commandHandle,
			      (I2CAddress << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(commandHandle, &m_statusByte, I2C_MASTER_ACK);
	i2c_master_stop(commandHandle);
	bool i2cCommandResult =
		i2c_master_cmd_begin(I2CPortNumber, commandHandle,
				     pdMS_TO_TICKS(1000)) == ESP_OK;
	i2c_cmd_link_delete(commandHandle);

	ESP_LOGD(TAG, "readStatusByte(), READ Result: %s, statusByte: %d",
		 (i2cCommandResult ? "OK" : "FAIL"), m_statusByte);
	return i2cCommandResult;
}

bool __aht10::hasStatus() const
{
	return m_statusByte != 0x0;
}

bool __aht10::isCalibrated(bool forceFresh)
{
	if (forceFresh || !hasStatus())
		readStatusByte();

	uint8_t mask = 0b00001000;
	return m_statusByte & mask;
}

bool __aht10::isBusy(bool forceFresh)
{
	if (forceFresh || !hasStatus())
		readStatusByte();

	uint8_t mask = 0b10000000;
	return m_statusByte & mask;
}

float __aht10::getTemperature()
{
	uint32_t temperature = ((uint32_t)(m_rawData[3] & 0x0F) << 16) |
			       ((uint16_t)m_rawData[4] << 8) |
			       m_rawData[5]; //20-bit raw temperature data

	return (float)temperature * 0.000191 - 50;
}

float __aht10::getHumidity()
{
	uint32_t rawData = (((uint32_t)m_rawData[1] << 16) |
			    ((uint16_t)m_rawData[2] << 8) | (m_rawData[3])) >>
			   4; //20-bit raw humidity data

	float humidity = (float)rawData * 0.000095;

	if (humidity < 0)
		return 0;
	if (humidity > 100)
		return 100;
	return humidity;
}
}

void Aht10::init()
{
	temp_ = 0.0;
	hm_ = 0.0;
}

void Aht10::read(MeasureP data)
{
	temp_ = static_cast<double>(dev_.getTemperature());
	hm_ = static_cast<double>(dev_.getHumidity());
	ESP_LOGI(::TAG, "The temperature read was %f\n", temp_);
	ESP_LOGI(::TAG, "The humidity read was %f\n", hm_);
	data->hm = hm_;
	data->temp = temp_;
}
}
