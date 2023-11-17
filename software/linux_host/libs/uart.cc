#include "uart.hh"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <ostream>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>

namespace uart {

using termios = struct termios;

UartInterface::UartInterface(std::ostream &os, std::string serial,
                             UartBaudrate ubd, UartStopBit usb,
                             UartParityBit upb, UartBitsPerByte ubpb)
    : port_{serial} {
  tcflag_t bpbytes;
  speed_t baud;

  serial_file_ = open(port_.c_str(), O_RDWR);
  termios tty;

  if (serial_file_ < 0) {
    std::string err{"Unable to open: " + port_};
    throw std::runtime_error(err.c_str());
  }

  if (tcgetattr(serial_file_, &tty) != 0) {
    throw std::runtime_error("Unable to initialize termios struct");
  }

  tty.c_cflag &= ~PARENB;

  if (upb == UartParityBit::ENABLE_PARITY_BIT) {
    tty.c_cflag |= PARENB;
  }

  tty.c_cflag &=
      ~CSTOPB; // Clear stop field, only one stop bit used in communication

  tty.c_cflag &=
      ~CSIZE; // Clear all the size bits, then use one of the statements below

  switch (ubpb) {
  case UartBitsPerByte::FIVE_BITS_PER_BYTE:
    tty.c_cflag |= CS5;
    break;
  case UartBitsPerByte::SIX_BITS_PER_BYTE:
    tty.c_cflag |= CS6;
    break;
  case UartBitsPerByte::SEVEN_BITS_PER_BYTE:
    tty.c_cflag |= CS7;
    break;
  case UartBitsPerByte::EIGHT_BITS_PER_BYTE:
    tty.c_cflag |= CS8;
    break;
  default:
    throw std::logic_error("Invalid option for bits per byte");
  }

  tty.c_cflag &= ~CRTSCTS;

  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_lflag &= ~ICANON;

  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;

  tty.c_lflag &= ~ISIG;

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;

  tty.c_cc[VTIME] = 10;
  tty.c_cc[VMIN] = 0;

  switch (ubd) {
  case UartBaudrate::BR9600:
    baud = B9600;
    break;
  case UartBaudrate::BR19200:
    baud = B19200;
    break;
  case UartBaudrate::BR38400:
    baud = B38400;
    break;
  case UartBaudrate::BR115200:
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
void UartInterface::write_data(unsigned char *send_buffer, std::size_t len) {
  write(serial_file_, send_buffer, len);
}
std::size_t UartInterface::read_data(unsigned char *receive_buffer) {
  auto msg_len = read(serial_file_, rx_buffer_, sizeof(rx_buffer_));

  if (msg_len < 0) {
    throw std::runtime_error("Error reading the uart message");
  }

  std::memcpy(receive_buffer, rx_buffer_, sizeof(uint8_t) * msg_len);

  std::memset(rx_buffer_, 0, sizeof(rx_buffer_));

  return msg_len;
}

} // namespace uart
