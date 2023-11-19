#ifndef QUEUE_HH_
#define QUEUE_HH_

#include <cstddef>
#include <memory>
#include <stdexcept>

/* Namespace for data structures */
namespace ds {
/* Node struct for linked list*/
template <typename Tp>
struct Node {
  Tp value;
  std::shared_ptr<Node> next;
  Node(Tp item) : value{item}, next{nullptr} {}
};
/* Queue class implemented to be a static capacity circular linked list */
template <typename Tp>
class Queue {
  using NodeP = std::shared_ptr<Node<Tp>>;

 public:
  Queue(std::size_t capacity) : capacity_{capacity} {}
  auto enqueue(Tp item) -> void;
  auto dequeue() -> Tp;
  auto peek() const -> Tp { return head_->value; }
  auto lenght() const -> std::size_t { return lenght_; }
  auto capacity() const -> std::size_t { return capacity_; }
  operator bool() const { return lenght_ != 0; }
  auto operator[](std::size_t n) const -> Tp const &;

 private:
  std::size_t capacity_;
  std::size_t lenght_{0};
  NodeP head_{nullptr};
  NodeP tail_{nullptr};
};

/* Enqueue itens in the back of the queue */
template <typename Tp>
auto Queue<Tp>::enqueue(Tp item) -> void {
  if (lenght_ < capacity_) {
    if (tail_ == nullptr) {
      tail_ = std::make_shared<Node<Tp>>(item);
      head_ = tail_;
    } else {
      NodeP node = std::make_shared<Node<Tp>>(item);
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

/* Dequeue itens from the front of the queue */
template <typename Tp>
auto Queue<Tp>::dequeue() -> Tp {
  if (lenght_ == 0) {
    throw std::runtime_error("Attempt to Dequeue an empty Queue");
  }
  --lenght_;
  Tp value = head_->value;
  head_ = head_->next;
  if (lenght_ != 0) {
    tail_->next = head_;
  } else {
    tail_ = nullptr;
  }
  return value;
}

template <typename Tp>
auto Queue<Tp>::operator[](std::size_t element) const -> Tp const & {
  if (element-- > lenght_) {
    throw std::invalid_argument("Out of bounds index given to queue");
  }
  NodeP node = head_;
  for (std::size_t i{0}; i < element; ++i) node = node->next;

  return node->value;
}

}  // namespace ds
#endif  // !QUEUE_HH_
