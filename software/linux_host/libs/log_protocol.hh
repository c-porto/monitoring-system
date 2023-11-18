#ifndef LOG_PROTOCOL_HH_
#define LOG_PROTOCOL_HH_

#include "queue.hh"
#include <cstdint>
#include <memory>
#include <ostream>
namespace monitoring_system {
namespace logs {

using log_queue = std::shared_ptr<ds::Queue<std::string>>;

template <typename Tp> class Protocol {
public:
  virtual log_queue serialize_data(std::ostream &, Tp) = 0;
};

template <typename Tp> class EventProtocol : public Protocol<Tp> {
  virtual log_queue serialize_data(std::ostream &, Tp) = 0;
};

template <typename Tp> class TotalTimeProtocol : public Protocol<Tp> {
  virtual log_queue serialize_data(std::ostream &, Tp) = 0;
};

} // namespace logs
} // namespace monitoring_system
#endif // !LOG_PROTOCOL_HH_
