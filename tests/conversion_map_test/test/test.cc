#include <gtest/gtest.h>
#include <map.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>

const std::unordered_map<sensor::sensor_id, LogHandlerMock> kLogConversion{
    /* Dht11 handler for single read enqueue */
    {sensor::DHT11_ID,
     [](ds::Queue<double> &fila, logs::MeasureMock &sample) {
       fila.enqueue(sample.a);
       fila.enqueue(sample.b);
     }},
    /* Gyml8511 handler for single read enqueue */
    {sensor::GYML8511_ID,
     [](ds::Queue<double> &fila, logs::MeasureMock &sample) {
       fila.enqueue(sample.c);
     }},
    /* Cjmcu811  handler for single read enqueue */
    {sensor::CJMCU811_ID,
     [](ds::Queue<double> &fila, logs::MeasureMock &sample) {
       fila.enqueue(sample.d);
     }}};

/* Static Global Variables*/
namespace {
ds::Queue<double> q{10};
logs::MeasureMock m;
std::shared_ptr<sensor::sensor_id[]> sptr{new sensor::sensor_id[3]};
constexpr double output[]{1.0, 2.0, 3.0, 4.0};
}  // namespace

/* Testing finding correct key, i.e finding correct sensor_id */
TEST(FunctionMapTest, FindKeyTest) {
  sptr[0] = sensor::DHT11_ID;
  sptr[1] = sensor::GYML8511_ID;
  sptr[2] = sensor::CJMCU811_ID;
  for (std::size_t i{0}; i < 3; ++i) {
    auto j{kLogConversion.find(sptr[i])};
    EXPECT_EQ(j->first, sptr[i]) << "Failed in finding key stored in map";
  }
}

/* Testing executing handler function for the correct sensor_id associated */
TEST(FunctionMapTest, ExecutingHandlerFunctionTest) {
  sptr[0] = sensor::DHT11_ID;
  sptr[1] = sensor::GYML8511_ID;
  sptr[2] = sensor::CJMCU811_ID;
  for (std::size_t i{0}; i < 3; ++i) {
    auto j{kLogConversion.find(sptr[i])};
    j->second(::q, ::m);
  }
  for (std::size_t i{0}; i < 4; ++i) {
    EXPECT_EQ(::q.dequeue(), ::output[i]);
  }
}
