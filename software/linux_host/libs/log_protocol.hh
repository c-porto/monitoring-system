#ifndef LOG_PROTOCOL_HH_
#define LOG_PROTOCOL_HH_

#include <cstdint>
#include <memory>
#include <ostream>

#include "queue.hh"
#include "request.hh"
namespace monitoring_system {
namespace logs {

using log_queue = ds::Queue<std::string> *;

class Protocol {
public:
  virtual log_queue deserialize_data(MessageFrame &,
                                     uart::UartInterface &) const = 0;
  virtual ~Protocol() = default;
};

class EventProtocol : public Protocol {
public:
  EventProtocol() = default;
  log_queue deserialize_data(MessageFrame &,
                             uart::UartInterface &) const override;
};

class TotalTimeProtocol : public Protocol {
public:
  TotalTimeProtocol() = default;
  log_queue deserialize_data(MessageFrame &,
                             uart::UartInterface &) const override;
};

} // namespace logs
} // namespace monitoring_system
#endif // !LOG_PROTOCOL_HH_
