#include "request.hh"

#include <cstddef>
#include <stdexcept>

#include "uart.hh"

namespace monitoring_system {
namespace logs {

MessageFrame
TotalTimeRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kTotalTimeResponseBytes{8};
  const std::string msg{'T'};
  unsigned char msg_buffer[] = {0x54};
  auto res = uart.write_data(msg_buffer, 1);
  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }
  return MessageFrame{res, kTotalTimeResponseBytes, msg};
}

MessageFrame
EventsRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kEventResponseBytes{10000};
  const std::string msg{'L'};
  unsigned char msg_buffer[] = {0x4C};
  auto res = uart.write_data(msg_buffer, 1);
  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }
  return MessageFrame{res, kEventResponseBytes, msg};
}

} // namespace logs
} // namespace monitoring_system
