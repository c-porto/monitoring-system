#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <vector>
#include <absl/strings/str_split.h>

#include "libs/cli.hh"
#include "libs/emb_sys_comm.hh"
#include "libs/log_protocol.hh"
#include "libs/queue.hh"
#include "libs/uart.hh"

int main(int argc, const char *argv[]) {
  auto host = std::make_shared<monitoring_system::LinuxHost>();
  try {
    auto settings = cli::CliSettings::parse_settings(argc, argv);
    host->load_settings(std::move(settings));
  } catch (std::exception &ex) {
    std::cerr << "Exception triggered parsing cli commands \n Exception "
                 "description: "
              << ex.what() << '\n';
    std::cerr << "Terminating the program..." << '\n';
    exit(1);
  }
  host->start_cli_interface(std::cout);
}
