#include "emb_sys_comm.hh"

#include <chrono>
#include <memory>
#include <thread>

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

void LinuxHost::display_logs(std::ostream &os, logs::RequestTypes &type) {
  // TODO
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
  } catch (std::exception const &ex) {
    std::cerr << "Could not open uart connection \n";
    std::cerr << "Exception occurred: " << ex.what() << '\n';
    std::cerr << "Terminating the program \n";
    exit(1);
  }
  while (true) {
    os << "___________________________________________________" << '\n';
    os << "        Linux Host CLI INTERFACE Started"
       << "\n\n";
    os << "Choose one of the Options Below:"
       << "\n\n";
    os << "1. Request total time online to embedded system "
       << "\n";
    os << "2. Request event logs to embedded system "
       << "\n\n";
    os << "Enter your option: ";

    auto request_type = self->handle_user_option(os);

    ProtocolP proto;
    RequestP rq;

    switch (request_type) {
      case logs::RequestTypes::kTotalTime:
        proto = std::move(std::make_unique<logs::TotalTimeProtocol>());
        rq = std::move(std::make_unique<logs::TotalTimeRequest>());
      case logs::RequestTypes::kEvents:
        proto = std::move(std::make_unique<logs::EventProtocol>());
        rq = std::move(std::make_unique<logs::EventsRequest>());
    }

    self->make_request(std::move(rq), std::move(proto), *channel);
    self->display_logs(os, request_type);

    std::this_thread::sleep_for(std::chrono::seconds{5});
  }
}
}  // namespace monitoring_system
