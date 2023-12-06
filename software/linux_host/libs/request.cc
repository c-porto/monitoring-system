#include "request.hh"

#include <cstddef>
#include <stdexcept>

#include "uart.hh"

namespace monitoring_system {
namespace logs {

MessageFrame
TotalTimeRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kTotalTimeResponseBytes{8};

  constexpr char msg{'T'};

  return MessageFrame{1, kTotalTimeResponseBytes, msg};
}

MessageFrame
EventsRequest::emb_sys_log_request(uart::UartInterface const &uart) {
  constexpr std::size_t kEventResponseBytes{10000};

  constexpr char msg{'L'};

  return MessageFrame{1, kEventResponseBytes, msg};
}

} // namespace logs
} // namespace monitoring_system
