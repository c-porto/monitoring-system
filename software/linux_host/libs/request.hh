#ifndef REQUEST_HH_
#define REQUEST_HH_

#include <cstddef>
#include <string>

#include "uart.hh"

namespace monitoring_system {
namespace logs {

enum class RequestTypes {
  kTotalTime,
  kEvents,
};

enum class EventDisplayOptions {
  kAllEvents,
  kTimeWindowEvents,
};

/* Struct used to make the request API portable */
struct MessageFrame {
  MessageFrame(std::size_t tx, std::size_t rx, std::string c)
      : tx_msg_len_bytes{tx}, rx_msg_len_bytes{rx}, cmd{c} {}
  std::size_t tx_msg_len_bytes;
  std::size_t rx_msg_len_bytes;
  std::string cmd;
};

/* Base class used as interface to request data from the embedded system*/
class Request {
 public:
  virtual MessageFrame emb_sys_log_request(uart::UartInterface const &) = 0;
  virtual ~Request() = default;
};

/* Derived class used to request Time data from the embedded system*/
class TotalTimeRequest : public Request {
 public:
  TotalTimeRequest() = default;
  MessageFrame emb_sys_log_request(uart::UartInterface const &) override;
};

/* Derived class used to request Event data from the embedded system*/
class EventsRequest : public Request {
 public:
  EventsRequest() = default;
  MessageFrame emb_sys_log_request(uart::UartInterface const &) override;
};
}  // namespace logs
}  // namespace monitoring_system
#endif  // !REQUEST_HH_
