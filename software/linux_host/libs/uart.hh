#ifndef UART_HH_
#define UART_HH_

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
namespace uart {

enum class UartBaudrate { BR9600, BR19200, BR38400, BR115200 };

enum class UartStopBit { ONE_STOP_BIT, TWO_STOP_BIT };

enum class UartParityBit { DISABLE_PARITY_BIT, ENABLE_PARITY_BIT };

enum class UartBitsPerByte {
  FIVE_BITS_PER_BYTE,
  SIX_BITS_PER_BYTE,
  SEVEN_BITS_PER_BYTE,
  EIGHT_BITS_PER_BYTE
};

class UartInterface {
public:
    UartInterface(std::ostream&,std::string,UartBaudrate,UartStopBit,UartParityBit,UartBitsPerByte);
    ~UartInterface();
    void write_data(unsigned char* send_buffer, std::size_t len);
    std::size_t read_data(unsigned char* receive_buffer);
protected:
  uint8_t rx_buffer_[1024]{0};
  std::string port_;
  int serial_file_;
};

} // namespace uart
#endif // !UART_HH_
