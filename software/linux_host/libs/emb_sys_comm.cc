#include "emb_sys_comm.hh"

#include <absl/strings/numbers.h>
#include <absl/strings/str_split.h>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "log_protocol.hh"
#include "request.hh"
#include "uart.hh"

namespace monitoring_system {

void LinuxHost::make_request(RequestP &&rq, ProtocolP &&pr, UartRef uart) {
  auto request = std::move(rq);
  auto protocol = std::move(pr);

  const logs::MessageFrame msg_frame = request->emb_sys_log_request(uart);

  stored_logs_ = protocol->deserialize_data(msg_frame, uart);
}

RequestP LinuxHost::request_factory(logs::RequestTypes &rq) {
  switch (rq) {
    case logs::RequestTypes::kTotalTime:
      return std::make_unique<logs::TotalTimeRequest>();
    case logs::RequestTypes::kEvents:
      return std::make_unique<logs::EventsRequest>();
    default:
      return nullptr;
  }
}

ProtocolP LinuxHost::protocol_factory(logs::RequestTypes &rq) {
  switch (rq) {
    case logs::RequestTypes::kTotalTime:
      return std::make_unique<logs::TotalTimeProtocol>();
    case logs::RequestTypes::kEvents:
      return std::make_unique<logs::EventProtocol>();
    default:
      return nullptr;
  }
}

void LinuxHost::parse_time_window_events(std::ostream &os) {
  os << "\n\n"
     << "Please provide the events time window \n";
  os << "Use the following format: [%h:%m:%s %D/%M/%Y] [%h:%m:%s %D/%M/%Y]"
     << '\n'
     << "Enter time window: ";

  std::string input;
  std::getline(std::cin, input);
  //
  // queue logs date format -> [%h:%m:%s %D/%M/%Y] log format ->
  // [SENSORID][date]-[measurament]
  //
  std::vector<std::string_view> input_tokens =
      absl::StrSplit(input, absl::ByAnyChar(": /]"));

  int in_hour, in_min, in_sec, in_day, in_month, in_year;
  int out_hour, out_min, out_sec, out_day, out_month, out_year;

  if (!absl::SimpleAtoi({input_tokens[0].begin() + 1, input_tokens[0].end()},
                        &in_hour)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[1], &in_min)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[2], &in_sec)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[3], &in_day)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[4], &in_month)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[5], &in_year)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi({input_tokens[6].begin() + 1, input_tokens[0].end()},
                        &out_hour)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[7], &out_min)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[8], &out_sec)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[9], &out_day)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[10], &out_month)) {
    throw std::runtime_error("User input is invalid");
  }
  if (!absl::SimpleAtoi(input_tokens[11], &out_year)) {
    throw std::runtime_error("User input is invalid");
  }
  std::vector<int> logs_matched;
  for (std::size_t i{0}; i < stored_logs_->lenght(); ++i) {
    std::array<bool, 6> err;
    int __hour, __min, __sec, __day, __month, __year;
    std::vector<std::string_view> date =
        absl::StrSplit((*stored_logs_)[i], absl::ByAnyChar(": /]"));
    err[0] = absl::SimpleAtoi(
        {input_tokens[0].begin() + 1, input_tokens[0].end()}, &__hour);
    err[1] = absl::SimpleAtoi(input_tokens[1], &__min);
    err[2] = absl::SimpleAtoi(input_tokens[2], &__sec);
    err[3] = absl::SimpleAtoi(input_tokens[3], &__day);
    err[4] = absl::SimpleAtoi(input_tokens[4], &__month);
    err[5] = absl::SimpleAtoi(input_tokens[5], &__year);
    for (auto const &e : err) {
      if (!e) throw std::logic_error("Wrong log format");
    }
    if (in_year > __year) break;
    if (in_month > __month) break;
    if (in_day > __day) break;
    if (in_hour > __hour) continue;
    if (in_min > __min) continue;
    if (in_sec > __sec) continue;
    if (out_hour < __hour) break;
    if (out_min < __min) break;
    if (out_sec < __sec) break;
    logs_matched.push_back(i);
  }
  if (logs_matched.empty()) {
    os << "No event happened in that window" << '\n';
  }
  os << "Displaying events that happened in that time window" << '\n';
  for (auto const &event : logs_matched) {
    os << (*stored_logs_)[event] << '\n';
  }
  stored_logs_->clear();
  return;
}

logs::EventDisplayOptions LinuxHost::handle_user_event_option(
    std::ostream &os) const {
  static int err_count{0};
  int opt;

  while (!(std::cin >> opt)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    os << "Attempted a invalid option, please try again :)"
       << "\n\n";
    os << "Please choose one of the Options Below:"
       << "\n\n";
    os << "1. Request all events provided by the embedded system"
       << "\n";
    os << "2. Request event from a especific time window"
       << "\n\n";
    os << "Enter your option: ";
  }
  switch (opt) {
    case 1:
      return logs::EventDisplayOptions::kAllEvents;
    case 2:
      return logs::EventDisplayOptions::kTimeWindowEvents;
    default:
      ++err_count;
  }
  if (err_count < 3) {
    return this->handle_user_event_option(os);
  }
  exit(1);
}

void LinuxHost::display_logs(std::ostream &os, logs::RequestTypes &type) {
  os << "\n\n";
  switch (type) {
    case logs::RequestTypes::kTotalTime:
      os << "Option choosen was Total Time online"
         << "\n";
      os << "The result provided by the embedded system was: "
         << stored_logs_->dequeue() << "\n";
      return;
    case logs::RequestTypes::kEvents:
      os << "Option choosen was Log Events"
         << "\n";
      break;
  }
  os << "___________________________________________________" << '\n';
  os << "              Log Events Display Options "
     << "\n\n";
  os << "Choose one of the Options Below:"
     << "\n\n";
  os << "1. Request all events provided by the embedded system"
     << "\n";
  os << "2. Request event from a especific time window"
     << "\n\n";
  os << "___________________________________________________"
     << "\n\n";
  os << "Enter your option: ";

  auto display_option = this->handle_user_event_option(os);

  if (display_option == logs::EventDisplayOptions::kAllEvents) {
    os << "Displaying all events" << '\n';
    while (stored_logs_) {
      os << stored_logs_->dequeue() << '\n';
    }
    return;
  }
  try {
    this->parse_time_window_events(os);
  } catch (std::exception const &ex) {
    os << "Exception Ocurred parsing time window events"
       << "\n"
       << "Description: " << ex.what() << '\n';
  }
}

logs::RequestTypes LinuxHost::handle_user_option(std::ostream &os) const {
  static int err_count{0};
  int opt;

  while (!(std::cin >> opt)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    os << "Attempted a invalid option, please try again :)"
       << "\n\n";
    os << "Please choose one of the Options Below:"
       << "\n\n";
    os << "1. Request total time online to embedded system "
       << "\n";
    os << "2. Request event logs to embedded system "
       << "\n\n";
    os << "Enter your option: ";
  }
  switch (opt) {
    case 1:
      return logs::RequestTypes::kTotalTime;
    case 2:
      return logs::RequestTypes::kEvents;
    default:
      ++err_count;
  }
  if (err_count < 3) {
    return this->handle_user_option(os);
  }
  exit(1);
};

FinalSettings LinuxHost::handle_settings(std::ostream &os) const {
  if (user_settings_->get_help()) {
    os << "This program is used to open a uart connection to communicate "
          "with "
          "embedded systems"
       << '\n';
    os << "Usage: ";
    os << "linuxhost [OPTION]..." << '\n';
    os << "OPTIONS:" << '\n' << '\n';
    os << "     -p,  --port: serial port" << '\n';
    os << "     -b, --baudrate: baudrate used in uart" << '\n';
    os << "     -h, --help: used to open this menu" << '\n' << '\n';
    os << "Examples: \n";
    os << "linuxhost --port /dev/ttyUSB0 --baudrate 115200" << '\n';
    exit(0);
  }

  os << "Starting Monitoring System Linux Host" << '\n';

  uart::UartBaudrate baud;
  if (!user_settings_->get_baudrate()) {
    os << "Baudrate wasn't provided, using defaults" << '\n';
  } else {
    switch (static_cast<uart::UartBaudrate>(
        user_settings_->get_baudrate().value())) {
      case uart::UartBaudrate::kBR9600:
        baud = uart::UartBaudrate::kBR9600;
      case uart::UartBaudrate::kBR19200:
        baud = uart::UartBaudrate::kBR19200;
      case uart::UartBaudrate::kBR38400:
        baud = uart::UartBaudrate::kBR38400;
      case uart::UartBaudrate::kBR115200:
        baud = uart::UartBaudrate::kBR115200;
      default:
        os << "Baudrate provided isn't supported, using defaults" << '\n'
           << "Default Baudrate: 115200" << '\n';
        baud = uart::UartBaudrate::kBR115200;
    }
  }
  std::string port;
  if (!user_settings_->get_port()) {
    os << "Port wasn't provided, using defaults" << '\n'
       << "Default Port: /dev/ttyUSB0" << '\n';
    port = "/dev/ttyUSB0";
  } else if (user_settings_->get_port().value().find("/dev/") ==
             std::string::npos) {
    os << "Port provided isn't valid, using defaults" << '\n'
       << "Default Port: /dev/ttyUSB0" << '\n';
    port = "/dev/ttyUSB0";
  } else {
    port = user_settings_->get_port().value();
  }

  os << "\n Finished configurations, trying to create a Uart Connection \n";

  return {baud, port};
}

void LinuxHost::start_cli_interface(std::ostream &os) {
  auto self = shared_from_this();

  auto cfg = self->handle_settings(os);

  std::unique_ptr<uart::UartInterface> channel;
  try {
    auto tmp = std::make_unique<uart::UartInterface>(
        os, cfg.second, cfg.first, uart::UartStopBit::kOneStopBit,
        uart::UartParityBit::kDisableParityBit,
        uart::UartBitsPerByte::kEightBitsPerByte);
    channel = std::move(tmp);
  } catch (std::exception &ex) {
    std::cerr << "Could not open uart connection \n";
    std::cerr << "Exception occurred: " << ex.what() << '\n';
    std::cerr << "Terminating the program... \n";
    exit(1);
  }
  while (true) {
    os << "___________________________________________________" << '\n';
    os << "              Linux Host CLI Started"
       << "\n\n";
    os << "Choose one of the Options Below:"
       << "\n\n";
    os << "1. Request total time online to embedded system "
       << "\n";
    os << "2. Request event logs to embedded system "
       << "\n\n";
    os << "___________________________________________________"
       << "\n\n";
    os << "Enter your option: ";

    auto request_type = self->handle_user_option(os);

    ProtocolP proto = self->protocol_factory(request_type);
    RequestP rq = self->request_factory(request_type);

    try {
      self->make_request(std::move(rq), std::move(proto), *channel);
    } catch (std::exception &ex) {
      std::cerr << "Exception occurred, it's description is: " << ex.what()
                << "\n";
      std::cerr << "Terminating the program..."
                << "\n";
      exit(1);
    }
    self->display_logs(os, request_type);

    os << "Restarting the cli in 5 seconds"
       << "\n";
    std::this_thread::sleep_for(std::chrono::seconds{5});
  }
}
}  // namespace monitoring_system
