#include "cli.hh"

#include <memory>
namespace cli {

CliSettings::CliSettings() {
  uart_config_map_ = {
      {"-b",
       [](CliSettings &settings, std::string const &user_setting) {
         settings.baudrate_ = std::stoi(user_setting);
       }},
      {"--baudrate",
       [](CliSettings &settings, std::string const &user_setting) {
         settings.baudrate_ = std::stoi(user_setting);
       }},
      {"-p",
       [](CliSettings &settings, std::string const &user_setting) {
         settings.port_ = user_setting;
       }},
      {"--port",
       [](CliSettings &settings, std::string const &user_setting) {
         settings.port_ = user_setting;
       }},
  };

  help_map_ = {
      {"-h", [](CliSettings &settings) { settings.help_ = true; }},
      {"--help", [](CliSettings &settings) { settings.help_ = true; }},
  };
}
}  // namespace cli
