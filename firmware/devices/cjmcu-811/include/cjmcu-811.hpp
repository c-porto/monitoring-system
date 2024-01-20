#pragma once
#include "i2c.hpp"
#include "utils.hpp"
#include <sys/_stdint.h>

// External lib for ccs811 sensor encapsulated in Cjmcu811 class
// Lib from github.com/Gustbel
namespace sensor
{
namespace __cjmcu811
{
namespace external_lib
{

extern "C"
{
#include <stdbool.h>

#define TICK_DELAY 10

#define I2C_PORT_NUMBER 0

#define CCS811_I2C_ADDRESS_2 0x5A  /* Default Device Address/Identifier for CCS811*/
#define CCS811_I2C_ADDRESS_1 0x5A  // Could be 0x5B
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

/* CCS811 register addresses */
#define CCS811_REG_STATUS 0x00
#define CCS811_REG_MEAS_MODE 0x01
#define CCS811_REG_ALG_RESULT_DATA 0x02
#define CCS811_REG_RAW_DATA 0x03
#define CCS811_REG_ENV_DATA 0x05
#define CCS811_REG_NTC 0x06
#define CCS811_REG_THRESHOLDS 0x10
#define CCS811_REG_BASELINE 0x11

#define CCS811_REG_HW_ID 0x20
#define CCS811_REG_HW_VER 0x21
#define CCS811_REG_FW_BOOT_VER 0x23
#define CCS811_REG_FW_APP_VER 0x24

#define CCS811_REG_ERROR_ID 0xe0

#define CCS811_REG_APP_ERASE 0xf1
#define CCS811_REG_APP_DATA 0xf2
#define CCS811_REG_APP_VERIFY 0xf3
#define CCS811_REG_APP_START 0xf4
#define CCS811_REG_SW_RESET 0xff

// status register bits
#define CCS811_STATUS_ERROR 0x01     // error, details in CCS811_REG_ERROR
#define CCS811_STATUS_DATA_RDY 0x08  // new data sample in ALG_RESULT_DATA
#define CCS811_STATUS_APP_VALID 0x10 // valid application firmware loaded
#define CCS811_STATUS_FW_MODE 0x80   // firmware is in application mode

// error register bits
#define CCS811_ERR_WRITE_REG_INV 0x01  // invalid register address on write
#define CCS811_ERR_READ_REG_INV 0x02   // invalid register address on read
#define CCS811_ERR_MEASMODE_INV 0x04   // invalid requested measurement mode
#define CCS811_ERR_MAX_RESISTANCE 0x08 // maximum sensor resistance exceeded
#define CCS811_ERR_HEATER_FAULT 0x10   // heater current not in range
#define CCS811_ERR_HEATER_SUPPLY 0x20  // heater voltage not applied correctly

    // Struct data
    typedef struct
    {
        uint16_t dataCO2, dataVOC;
    } ccs811_data;

    // int8_t slave_read_byte(uint8_t);
    // bool slave_write_byte(uint8_t, uint8_t);
    bool ccs811_write_byte(uint8_t, const uint8_t, const uint8_t);
    uint8_t ccs811_read_byte(uint8_t);
    ccs811_data ccs811_sensor_data(uint8_t);
    bool init_ccs811();
    uint16_t get_ccs811(int);
}
} // namespace external_lib
} // namespace __cjmcu811

class Cjmcu811 final : public Sensor
{
  public:
    Cjmcu811()
    {
        this->id = CJMCU811_ID;
    }
    void init() override;
    void read(MeasureP ms) override;

  private:
    double C02_ppm_;
    const uint32_t kSensorBurnInTime_{1200000UL};
};

} // namespace sensor
