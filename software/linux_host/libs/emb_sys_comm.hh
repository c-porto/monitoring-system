#ifndef EMB_SYS_COMM_HH_
#define EMB_SYS_COMM_HH_

#include "cli.hh"
#include "log_protocol.hh"
#include "queue.hh"
#include "request.hh"
#include "uart.hh"
#include <memory>
#include <ostream>
#include <vector>

namespace monitoring_system {
using cli_settings = std::unique_ptr<cli::CliSettings>;
class LinuxHost final : public std::enable_shared_from_this<LinuxHost> {
public:
  void load_settings(cli_settings &&st) { user_settings_ = std::move(st); }
  template <typename Tp>
  logs::log_queue make_request(logs::Request *, logs::Protocol<Tp> *);
  void display_logs(std::ostream &);
  void start_cli_interface(std::ostream&);

private:
  cli_settings user_settings_;
  std::vector<logs::log_queue> stored_logs_;
};

} // namespace monitoring_system

#endif // !EMB_SYS_COMM_HH_
