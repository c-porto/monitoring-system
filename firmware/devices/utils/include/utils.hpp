#include <chrono>

namespace sensor {
typedef enum {
  DHT11_ID,
  GYML8511_ID,
  CJMCU811_ID,
} sensor_id;

class Sensor {
public:
  sensor_id id;
  virtual void read() = 0;
  virtual void init() = 0;
};
} // namespace sensor

namespace ds {
template <typename Tp> struct Node {
  Tp value;
  Node *next;
  Node(Tp item) : value{item}, next{nullptr} {};
};

template <typename Tp> class Queue {
public:
  Queue() : head_{nullptr}, tail_{nullptr}, lenght_{0} {};
  void enqueue(Tp item);
  Tp dequeue();
  Tp peek();
  size_t lenght() { return lenght_; };

private:
  Node<Tp> *head_;
  Node<Tp> *tail_;
  size_t lenght_;
};

template <typename Tp> void Queue<Tp>::enqueue(Tp item) {
  lenght_++;
  if (tail_ == nullptr) {
    tail_ = new Node(item);
    head_ = tail_;
  } else {
    Node<Tp> *node = new Node(item);
    tail_->next = node;
    tail_ = node;
  }
}

template <typename Tp> Tp Queue<Tp>::dequeue() {
  lenght_--;
  Node<Tp> *head = head_;
  head_ = head_->next;
  Tp value = head->value;
  head->next = nullptr;
  if (lenght_ == 0) {
    this->tail = nullptr;
  }
  delete head;
  return value;
}

template <typename Tp> Tp Queue<Tp>::peek() { return head_->value; }

} // namespace ds

namespace log {
template <typename Tp> struct LogData {
  sensor::sensor_id id;
  std::chrono::year_month_day date;
  Tp measure;
  LogData(sensor::sensor_id ID, std::chrono::year_month_day data, Tp sample)
      : id{ID}, date{data}, measure{sample} {};
};
} // namespace log
