#include "request.hh"

#include <cstddef>
#include <stdexcept>

#include "uart.hh"

namespace monitoring_system {
namespace logs {

MessageFrame TotalTimeRequest::emb_sys_log_request(
    uart::UartInterface const &uart) {
  constexpr std::size_t kTotalTimeResponseBytes{8};

  const std::string msg{"T"};

  auto res =
      uart.write_data(reinterpret_cast<const unsigned char *>(msg.c_str()), 1);

  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }

  return MessageFrame{res, kTotalTimeResponseBytes, msg};
}

MessageFrame EventsRequest::emb_sys_log_request(
    uart::UartInterface const &uart) {
  constexpr std::size_t kEventResponseBytes{10000};

  const std::string msg{"L"};

  auto res =
      uart.write_data(reinterpret_cast<const unsigned char *>(msg.c_str()), 1);

  if (res != 1) {
    throw std::runtime_error("Erroneous message was sent");
  }

  return MessageFrame{res, kEventResponseBytes, msg};
}

}  // namespace logs
}  // namespace monitoring_system
