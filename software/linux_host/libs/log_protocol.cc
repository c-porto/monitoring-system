#include "log_protocol.hh"

#include <absl/strings/str_split.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

#include "queue.hh"
#include "uart.hh"

namespace monitoring_system {
namespace logs {
log_queue
TotalTimeProtocol::deserialize_data(MessageFrame const &mf,
                                    uart::UartInterface const &uart) const {
  log_queue q = std::make_shared<ds::Queue<std::string>>(1);

  uint8_t rx_buffer[10000];

  std::memset(rx_buffer, 0, sizeof(rx_buffer) );

  usleep(100000);

  auto res_len = uart.read_data(rx_buffer, sizeof(rx_buffer));

  uint64_t seconds_on{0};

  for (std::size_t i{0}; i < res_len; ++i) {
    seconds_on |= (rx_buffer[i] << (8U * i));
  }

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::seconds(seconds_on));

  auto hrs = std::chrono::duration_cast<std::chrono::hours>(duration);

  duration -= hrs;

  auto min = std::chrono::duration_cast<std::chrono::minutes>(duration);

  duration -= min;

  const std::string kTotalTimeOn =
      std::to_string(hrs.count()) + " hours, " + std::to_string(min.count()) +
      " minutes, " + std::to_string(duration.count()) + " seconds On";

  q->enqueue(kTotalTimeOn);

  return q;
}
log_queue
EventProtocol::deserialize_data(MessageFrame const &mf,
                                uart::UartInterface const &uart) const {
  constexpr std::size_t kMaxNumberOfEvents{100};

  constexpr char packet_delimiter = '#';

  log_queue q = std::make_shared<ds::Queue<std::string>>(kMaxNumberOfEvents);

  char rx_buffer[10000];

  std::memset(rx_buffer, 0, sizeof(rx_buffer) );

  usleep(100000);

  auto res_len = uart.read_data(rx_buffer, sizeof(rx_buffer));

  std::cout << res_len << std::endl;
  usleep(10000000);

  if (res_len <= 1) {
    throw std::runtime_error("No logs available");
  }

  rx_buffer[res_len] = '0';

  const std::string full_msg{rx_buffer};

  std::cout << full_msg;

  std::vector<std::string> events = absl::StrSplit(full_msg, packet_delimiter);

  for (std::string const &event : events) {
    q->enqueue(event);
  }

  std::filesystem::path log_path{"./logs/logs.txt"};

  if (!std::filesystem::exists("./logs/")) {
    std::filesystem::create_directory("./logs/");
  }

  std::fstream log_file{log_path, std::ios::out | std::ios::app};

  auto current_time{
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

  auto time_info = std::localtime(&current_time);

  std::stringstream ss;

  ss << std::put_time(time_info, "%H:%M:%S | %d/%m/%Y");

  log_file << "Logs received on " << ss.str()
           << "______________________________ \n\n\n";

  if (q)
    q->dequeue();

  for (std::size_t i{0}; i < q->lenght(); ++i) {
    try {
      log_file << "Event Number: " << i << "| " << (*q)[i];
    } catch (std::exception const &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  log_file << "______________________________ \n\n\n";

  return q;
}
} // namespace logs
} // namespace monitoring_system
