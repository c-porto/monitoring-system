#include "libs/cli.hh"
#include "libs/emb_sys_comm.hh"
#include "libs/queue.hh"
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

int main(int argc, const char *argv[]) {
  auto host = std::make_shared<monitoring_system::LinuxHost>();
  try {
      auto settings = cli::CliSettings::parse_settings(argc, argv);
      host->load_settings(std::move(settings));
  } catch (std::exception &ex) {
    std::cerr
        << "Exception triggered parsing cli commands \n Exception description: "
        << ex.what() << '\n';
  }

  host->start_cli_interface(std::cout);
}
