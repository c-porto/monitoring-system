#include "request.hh"

#include <cstddef>
#include <stdexcept>

#include "uart.hh"

namespace monitoring_system {
namespace logs {

MessageFrame
TotalTimeRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kTotalTimeResponseBytes{8};

  const char msg_buffer[1] = {'T'};

  auto res = uart.write_data(msg_buffer, sizeof(msg_buffer));

  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }

  return MessageFrame{res, kTotalTimeResponseBytes, {msg_buffer[0]}};
}

MessageFrame
EventsRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kEventResponseBytes{10000};

  const char msg_buffer[1] = {'L'};

  auto res = uart.write_data(msg_buffer, sizeof(msg_buffer));

  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }

  return MessageFrame{res, kEventResponseBytes, {msg_buffer[0]}};
}

} // namespace logs
} // namespace monitoring_system
