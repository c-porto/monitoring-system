#include "uart.hh"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <stdexcept>

namespace uart {

using termios = struct termios;

UartInterface::UartInterface(std::ostream &os, std::string serial,
                             UartBaudrate ubd, UartStopBit usb,
                             UartParityBit upb, UartBitsPerByte ubpb)
    : port_{serial} {
  tcflag_t bpbytes;

  speed_t baud;

  serial_file_ = open(port_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  fcntl(serial_file_, F_SETFL, 0);

  termios tty;

  if (serial_file_ < 0) {
    std::string err{"Unable to open: " + port_};
    throw std::runtime_error(err.c_str());
  }

  if (tcgetattr(serial_file_, &tty) != 0) {
    throw std::runtime_error("Unable to initialize termios struct");
  }

  if (upb == UartParityBit::kEnableParityBit) {
    tty.c_cflag |= PARENB;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity

  tty.c_cflag &=
      ~CSTOPB; // Clear stop field, only one stop bit used in communication

  tty.c_cflag &=
      ~CSIZE; // Clear all the size bits, then use one of the statements below

  switch (ubpb) {
  case UartBitsPerByte::kFiveBitsPerByte:
    tty.c_cflag |= CS5;
    break;
  case UartBitsPerByte::kSixBitsPerByte:
    tty.c_cflag |= CS6;
    break;
  case UartBitsPerByte::kSevenBitsPerByte:
    tty.c_cflag |= CS7;
    break;
  case UartBitsPerByte::kEightBitsPerByte:
    tty.c_cflag |= CS8;
    break;
  default:
    throw std::logic_error("Invalid option for bits per byte");
  }

  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |=
      CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;

  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo

  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

  tty.c_iflag &= ~(IXON | IEXTEN); // Turn off s/w flow ctrl

  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes
                         // (e.g. newline chars)
  tty.c_oflag &=
      ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_cc[VTIME] = 0; 
                      
  tty.c_cc[VMIN] = 0;

  switch (ubd) {
  case UartBaudrate::kBR9600:
    baud = B9600;
    break;
  case UartBaudrate::kBR19200:
    baud = B19200;
    break;
  case UartBaudrate::kBR38400:
    baud = B38400;
    break;
  case UartBaudrate::kBR115200:
    baud = B115200;
    break;
  default:
    std::logic_error("Invalid option for Baudrate");
  }

  cfsetispeed(&tty, baud);
  cfsetospeed(&tty, baud);

  if (tcsetattr(serial_file_, TCSANOW, &tty) != 0) {
    throw std::runtime_error("Error when saving tty settings");
  }

  os << "Sucessfully created uart connection" << std::endl;
}
UartInterface::~UartInterface() { close(serial_file_); };
std::size_t UartInterface::write_data(const void *send_buffer,
                                      std::size_t len) const {
  auto res = write(serial_file_, send_buffer, len);

  return res;
}
std::size_t UartInterface::read_data(void *receive_buffer,
                                     std::size_t buflen) const {
  auto msg_len = read(serial_file_, receive_buffer, buflen);

  return msg_len;
}

} // namespace uart
