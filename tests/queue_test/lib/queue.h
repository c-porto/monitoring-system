#include <cstddef>
namespace ds {
template <typename __Tp> struct Node {
  __Tp value;
  Node *next;
  Node(__Tp item) : value{item}, next{nullptr} {}
};
template <typename __Tp> class Queue {
  using NodeP = Node<__Tp> *;

public:
  Queue(std::size_t capacity) : capacity_{capacity} {}
  auto enqueue(__Tp item) -> void;
  auto dequeue() -> __Tp;
  auto peek() const -> __Tp { return head_->value; }
  auto lenght() const -> std::size_t { return lenght_; }
  auto capacity() const -> std::size_t { return capacity_; }
  operator bool() const { return lenght_ != 0; }

private:
  std::size_t capacity_;
  std::size_t lenght_{0};
  NodeP head_{nullptr};
  NodeP tail_{nullptr};
};

template <typename __Tp> auto Queue<__Tp>::enqueue(__Tp item) -> void {
  if (lenght_ < capacity_) {
    if (tail_ == nullptr) {
      tail_ = new Node(item);
      head_ = tail_;
    } else {
      NodeP node = new Node(item);
      tail_->next = node;
      tail_ = node;
      tail_->next = head_;
    }
    lenght_++;
  } else {
    head_->value = item;
    tail_->next = head_;
    tail_ = head_;
    head_ = head_->next;
  }
}
template <typename __Tp> auto Queue<__Tp>::dequeue() -> __Tp {
  --lenght_;
  __Tp value = head_->value;
  NodeP tmp = head_;
  head_ = head_->next;
  tmp->next = nullptr;
  if (lenght_ != 0) {
    tail_->next = head_;
  } else {
    tail_ = nullptr;
  }
  delete tmp;
  return value;
}

} // namespace ds
