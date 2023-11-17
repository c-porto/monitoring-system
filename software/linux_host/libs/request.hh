#ifndef REQUEST_HH_
#define REQUEST_HH_

#include <cstddef>
#include <string>

namespace monitoring_system {
namespace logs {

struct MessageFrame {
  std::size_t tx_msg_len_bytes;
  std::size_t rx_msg_len_bytes;
  std::string cmd;
};

/* Base class used as interface to request data from the embedded system*/
class Request {
public:
  virtual MessageFrame emb_sys_log_request() = 0;
};

/* Derived class used to request Time data from the embedded system*/
class TotalTimeRequest : public Request {
public:
  MessageFrame emb_sys_log_request() override { return {1U, 8U, "T"}; };
};

/* Derived class used to request Event data from the embedded system*/
class EventsRequest : public Request {
public:
  MessageFrame emb_sys_log_request() override { return {1U, 100U * 50U, "L"}; };
};
} // namespace logs
} // namespace monitoring_system
#endif // !REQUEST_HH_
