#include "emb_sys_comm.hh"
#include <chrono>
#include <thread>

namespace monitoring_system {
    void LinuxHost::start_cli_interface(std::ostream& os) {
        os << "Starting Monitoring System Linux Host" << '\n';
        while(true) {
            os << "Port: " << user_settings_->get_port().value_or("Nenhuma Porta") << '\n';
            os << "Baud: " << user_settings_->get_baudrate().value_or(0) << '\n';
            std::this_thread::sleep_for(std::chrono::seconds{5});
        }
    }

}
