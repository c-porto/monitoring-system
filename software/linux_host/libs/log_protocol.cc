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
#include <vector>

#include "queue.hh"
#include "uart.hh"

namespace monitoring_system {
namespace logs {
log_queue TotalTimeProtocol::deserialize_data(
    MessageFrame const &mf, uart::UartInterface const &uart) const {
  log_queue q = std::make_shared<ds::Queue<std::string>>(1);

  auto rx_buffer = std::make_unique<unsigned char[]>(mf.rx_msg_len_bytes);

  std::memset(rx_buffer.get(), 0, mf.rx_msg_len_bytes);

  auto res_len = uart.read_data(rx_buffer.get(), mf.rx_msg_len_bytes);

  uint64_t seconds_on{0};

  std::cout << mf.tx_msg_len_bytes << std::endl;

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
log_queue EventProtocol::deserialize_data(
    MessageFrame const &mf, uart::UartInterface const &uart) const {
  constexpr std::size_t kMaxNumberOfEvents{100};

  constexpr char packet_delimiter = '#';

  log_queue q = std::make_shared<ds::Queue<std::string>>(kMaxNumberOfEvents);

  auto rx_buffer = std::make_unique<unsigned char[]>(mf.rx_msg_len_bytes);

  std::memset(rx_buffer.get(), 0, mf.rx_msg_len_bytes);

  auto res_len = uart.read_data(rx_buffer.get(), mf.rx_msg_len_bytes);

  if (res_len == 0) {
    throw std::runtime_error("No logs available");
  }

  const std::string full_msg{reinterpret_cast<char *>(rx_buffer.get()),
                             res_len};

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

  q->dequeue();

  for (std::size_t i{0}; i < q->lenght(); ++i) {
    log_file << "Event Number: " << i << "| " << (*q)[i];
  }

  log_file << "______________________________ \n\n\n";

  return q;
}
}  // namespace logs
}  // namespace monitoring_system
