#ifndef EMB_SYS_COMM_HH_
#define EMB_SYS_COMM_HH_

#include <memory>
#include <ostream>
#include <vector>

#include "cli.hh"
#include "log_protocol.hh"
#include "queue.hh"
#include "request.hh"
#include "uart.hh"

namespace monitoring_system {
using Settings = std::unique_ptr<cli::CliSettings>;
using FinalSettings = std::pair<uart::UartBaudrate, std::string>;
using RequestP = std::unique_ptr<logs::Request>;
using ProtocolP = std::unique_ptr<logs::Protocol>;
using UartRef = uart::UartInterface const &;

class LinuxHost final : public std::enable_shared_from_this<LinuxHost> {
public:
  void load_settings(Settings &&st) { user_settings_ = std::move(st); }
  void start_cli_interface(std::ostream &);

private:
  logs::RequestTypes handle_user_option(std::ostream &) const;
  logs::EventDisplayOptions handle_user_event_option(std::ostream &) const;
  FinalSettings handle_settings(std::ostream &) const;
  void make_request(RequestP &&, ProtocolP &&, UartRef);
  void display_logs(std::ostream &, logs::RequestTypes &);
  void handle_time_requirements(std::ostream &);
  void parse_time_window_events(std::ostream &);
  Settings user_settings_;
  logs::log_queue stored_logs_;
};

} // namespace monitoring_system

#endif // !EMB_SYS_COMM_HH_
