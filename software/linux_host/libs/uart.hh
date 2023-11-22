#ifndef UART_HH_
#define UART_HH_

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
namespace uart {

enum class UartBaudrate { kBR9600, kBR19200, kBR38400, kBR115200 };

enum class UartStopBit { kOneStopBit, kTwoStopBit };

enum class UartParityBit { kDisableParityBit, kEnableParityBit };

enum class UartBitsPerByte {
  kFiveBitsPerByte,
  kSixBitsPerByte,
  kSevenBitsPerByte,
  kEightBitsPerByte
};

class UartInterface {
public:
  UartInterface(std::ostream &, std::string, UartBaudrate, UartStopBit,
                UartParityBit, UartBitsPerByte);
  ~UartInterface();
  std::size_t write_data(const char *send_buffer, std::size_t len) const;
  std::size_t read_data(char *receive_buffer, std::size_t buflen) const;
  auto get_port() const { return serial_file_; }

protected:
  std::string port_;
  int serial_file_;
};

} // namespace uart
#endif // !UART_HH_
