#pragma once

#include "utils.hpp"

namespace sensor
{
namespace external_lib
{
class __aht10 {
    public:
	explicit __aht10(int i2cPinSda, int i2cPinScl);

	bool readRawData();
	float getTemperature();
	float getHumidity();

    private:
	void initBus(int i2cPinSda, int i2cPinScl);
	bool activateOneTimeMode();
	bool activateContinuousMode();
	bool loadFactoryCalibration();
	void softReset();

	bool write3byte(uint8_t byte0, uint8_t byte1, uint8_t byte2);
	bool writeBytes(uint8_t *bytes, uint8_t numberOfBytes);

	uint8_t m_rawData[6];

	bool readStatusByte();
	// 7:busy, 6: cmd mode, 5: cyclic mode, 4: reserved, 3: calibrated, 2-0: reserved
	uint8_t m_statusByte = 0x0;
	bool hasStatus() const;
	bool isCalibrated(bool forceFresh = false);
	bool isBusy(bool forceFresh = true);
};
}
class Aht10 final : public Sensor {
    public:
	void read(MeasureP data);
	void init();

	explicit Aht10(int i2cPinSda, int i2cPinScl)
		: dev_{ i2cPinSda, i2cPinScl }
	{
	}

    private:
	external_lib::__aht10 dev_;
    double temp_;
    double hm_;
};
}
