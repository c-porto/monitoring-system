#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include <memory>
#include <sstream>

#define SENSOR_READ_EVENT (1UL << 0UL)

#define mutex_t SemaphoreHandle_t
#define mutex_create() xSemaphoreCreateMutex()
#define mutex_lock(__mutex, __ms)                                              \
  xSemaphoreTake((__mutex), pdMS_TO_TICKS((__ms)))
#define mutex_unlock(__mutex) xSemaphoreGive((__mutex))

extern mutex_t mutex;
extern EventGroupHandle_t event_group;

/* Simple typedefs for convenience */
typedef double temperature_t;
typedef double humidity_t;
typedef double light_t;
typedef double air_quality_t;

enum class Result {
  Ok,
  Err,
};

/* Namespace for everything sensor related */
namespace sensor {
/* Forward declaration of Display class */
class Display;
/* Sensor IDs */
typedef enum {
  DHT11_ID,
  GYML8511_ID,
  CJMCU811_ID,
} sensor_id;

/* Measure Class responsible for keep tracking all measurements, time and
 * errors */
class Measure {
public:
  Measure();
  Measure(light_t, temperature_t, air_quality_t, humidity_t);
  Measure(Measure const &) = default;
  Measure &operator=(Measure const &) = default;
  temperature_t temp;
  humidity_t hm;
  light_t uv;
  air_quality_t air;
};

/* Simply for convenience */
using MeasureP = Measure *;

/* Abstract Parent class for all sensors*/
class Sensor {
public:
  sensor_id id;
  virtual void read(MeasureP data) = 0;
  virtual void init() = 0;
};

/* Simply for convenience */
using SensorP = Sensor *;

} // namespace sensor

/* Namespace for data structures */
namespace ds {
/* Node struct for linked list*/
template <typename Tp> struct Node {
  Tp value;
  std::shared_ptr<Node> next;
  Node(Tp item) : value{item}, next{nullptr} {}
};
/* Queue class implemented to be a static capacity circular linked list */
template <typename Tp> class Queue {
  using NodeP = std::shared_ptr<Node<Tp>>;

public:
  Queue(std::size_t capacity) : capacity_{capacity} {}
  auto enqueue(Tp item) -> void;
  auto dequeue() -> Tp;
  auto peek() const -> Tp { return head_->value; }
  auto lenght() const -> std::size_t { return lenght_; }
  auto capacity() const -> std::size_t { return capacity_; }
  operator bool() const { return lenght_ != 0; }

private:
  std::size_t capacity_;
  std::size_t lenght_{0};
  NodeP head_{nullptr};
  NodeP tail_{nullptr};
};

/* Enqueue itens in the back of the queue */
template <typename Tp> auto Queue<Tp>::enqueue(Tp item) -> void {
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
template <typename Tp> auto Queue<Tp>::dequeue() -> Tp {
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

} // namespace ds

namespace logs {
class Logger {
public:
  Logger(const char *);
  Logger const &operator<<(Result) const;
  Logger const &operator<<(sensor::MeasureP) const;
  Logger const &operator<<(const char *) const;

private:
  std::string tag_;
};
} // namespace logs
