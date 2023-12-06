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
log_queue TotalTimeProtocol::deserialize_data(MessageFrame &mf,
                                              uart::UartInterface &uart) const {
  log_queue q = new ds::Queue<std::string>(1);

  const char tx_buffer[1] = {mf.cmd};

  char rx_buffer[mf.rx_msg_len_bytes];

  int abytes = uart.write_data(tx_buffer, 1);

  usleep(100000);

  auto res_len = uart.read_data(rx_buffer, sizeof(rx_buffer));

  uint64_t seconds_on{0};

 // for (std::size_t i{0}; i < res_len; ++i) {
    //seconds_on |= (static_cast<uint64_t>(rx_buffer[i]) << (8 * i));
  //}
  //
  seconds_on = static_cast<std::uint64_t>(rx_buffer[0]);

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
log_queue EventProtocol::deserialize_data(MessageFrame &mf,
                                          uart::UartInterface &uart) const {
  constexpr std::size_t kMaxNumberOfEvents{100};

  constexpr char packet_delimiter = '#';

  log_queue q = new ds::Queue<std::string>(kMaxNumberOfEvents);

  const char tx_buffer[1] = {mf.cmd};

  char rx_buffer[mf.rx_msg_len_bytes];

  int abytes = uart.write_data(tx_buffer, 1);

  usleep(100000);

  auto res_len = uart.read_data(rx_buffer, sizeof(rx_buffer));

  if (res_len <= 1) {
    throw std::runtime_error("No logs available");
  }

  rx_buffer[res_len + 1] = '\0';

  std::vector<std::string> events = absl::StrSplit(rx_buffer, packet_delimiter);

  for (std::string const &event : events) {
    q->enqueue(event);
  }

  std::filesystem::path log_path{"./logs/logs.txt"};

  if (!std::filesystem::exists("./logs/")) {
    std::filesystem::create_directory("./logs/");
  }

  std::ofstream log_file{log_path, std::ios::app};

  auto current_time{
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};

  auto time_info = std::localtime(&current_time);

  std::stringstream ss;

  ss << std::put_time(time_info, "%H:%M:%S | %d/%m/%Y");

  log_file << "________________________________________________________________"
              "_____________________________ \n\n\n"
           << "                                           Logs received on "
           << ss.str() << "\n\n";

  if (*q) {
    q->dequeue();
  }

  log_queue final_queue = new ds::Queue<std::string>(kMaxNumberOfEvents);

  for (std::size_t i{0}; i < q->lenght(); ++i) {
    try {
      log_file << "Event Number: " << i << "| " << q->peek() << '\n';
      final_queue->enqueue(q->dequeue());
    } catch (std::exception const &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  log_file << "________________________________________________________________"
              "_____________________________ \n\n\n";

  return final_queue;
}
} // namespace logs
} // namespace monitoring_system
