#ifndef CLI_HH_
#define CLI_HH_

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cli {

class CliSettings {
  using config_handler =
      std::function<void(CliSettings &, std::string const &)>;
  using help_handler = std::function<void(CliSettings &)>;

public:
  CliSettings();
  auto get_baudrate() {return baudrate_;}
  auto get_port() {return port_;}
  auto get_help() {return help_;}
  static std::unique_ptr<CliSettings> parse_settings(int argc,
                                                     const char **argv) {
    auto settings = std::make_unique<CliSettings>();
    for (std::size_t i{1}; i < argc; ++i) {
      std::string user_input{argv[i]};

      if (auto h{settings->help_map_.find(user_input)};
          h != settings->help_map_.end()) {
        h->second(*settings);
      } else if (auto c{settings->uart_config_map_.find(user_input)};
                 c != settings->uart_config_map_.end()) {
        if (++i > argc) {
          throw std::runtime_error("Not enought parameters");
        }
        c->second(*settings, {argv[i]});
      } else {
        throw std::runtime_error("Invalid parameters");
      }
    }
    return settings;
  }

private:
  bool help_{false};
  std::optional<std::string> port_;
  std::optional<uint32_t> baudrate_;
  std::unordered_map<std::string, config_handler> uart_config_map_;
  std::unordered_map<std::string, help_handler> help_map_;
};
} // namespace cli
#endif // !CLI_HH_
