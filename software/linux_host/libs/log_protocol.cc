#include "log_protocol.hh"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include "queue.hh"
#include "uart.hh"

namespace monitoring_system {
namespace logs {
log_queue TotalTimeProtocol::deserialize_data(
    MessageFrame const &mf, uart::UartInterface const &uart) const {
  log_queue q = std::make_shared<ds::Queue<std::string>>(1);
  auto rx_buffer = std::make_unique<unsigned char[]>(mf.rx_msg_len_bytes);
  auto res = uart.write_data(rx_buffer.get(), mf.rx_msg_len_bytes);
  uint64_t seconds_on{0};

  for (std::size_t i{0}; i < res; ++i) {
    seconds_on |= (rx_buffer[i] << (8U * i));
  }
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::seconds(seconds_on));
  auto hrs = std::chrono::duration_cast<std::chrono::hours>(duration);
  duration -= hrs;
  auto min = std::chrono::duration_cast<std::chrono::minutes>(duration);
  duration -= min;

  const std::string kTotalTimeOn = std::to_string(hrs.count()) + " hours, " +
                                   std::to_string(min.count()) + " minutes, " +
                                   std::to_string(duration.count()) + " On";
  q->enqueue(kTotalTimeOn);

  return q;
}
log_queue EventProtocol::deserialize_data(
    MessageFrame const &mf, uart::UartInterface const &uart) const {
  log_queue q =
      std::make_shared<ds::Queue<std::string>>(mf.rx_msg_len_bytes / 100);
  auto rx_buffer = std::make_unique<unsigned char[]>(mf.rx_msg_len_bytes);
  // TODO
  return q;
}
}  // namespace logs
}  // namespace monitoring_system
