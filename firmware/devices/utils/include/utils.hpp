#pragma once
#include <chrono>
#include <functional>
#include <ostream>
#include <unordered_map>

/* Simple typedefs for convenience */
typedef double temperature_t;
typedef double humidity_t;
typedef double light_t;
typedef double air_quality_t;

/* Namespace for everything sensor related */
namespace sensor {
/* Sensor IDs */
typedef enum {
  DHT11_ID,
  GYML8511_ID,
  CJMCU811_ID,
} sensor_id;

/* Measure Struct responsible for keep tracking all measurements, time and
 * errors */
struct Measure {
  std::chrono::year_month_day last_time_measure;
  bool err{false};
  sensor_id last_id;
  temperature_t temp;
  humidity_t hm;
  light_t uv;
  air_quality_t air;
  friend std::ostream &operator<<(std::ostream &os, Measure const &ms);
  operator bool() const noexcept { return err; }
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

/* Standard API for updating global measurements instance for individual sensors
 */
class SensorAPI {
public:
  SensorAPI(SensorP sensor, MeasureP data);
  void update_data();

private:
  SensorP sensor_{nullptr};
  MeasureP data_{nullptr};
};

} // namespace sensor

/* Namespace for data structures */
namespace ds {
/* Node struct for linked list*/
template <typename __Tp> struct Node {
  __Tp value;
  Node *next;
  Node(__Tp item) : value{item}, next{nullptr} {}
};
/* Queue based on a circular linked list with static capacity */
template <typename __Tp> class Queue {
  /* Typedef for convenience */
  using NodeP = Node<__Tp> *;

public:
  /* Constructor */
  Queue(std::size_t capacity) : capacity_{capacity} {}
  /* Enqueue at the end of queue */
  auto enqueue(__Tp item) -> void;
  /* Dequeue head of the queue */
  auto dequeue() -> __Tp;
  /* Peek the value of head node */
  auto peek() const -> __Tp { return head_->value; }
  /* Lenght of the queue */
  auto lenght() const -> std::size_t { return lenght_; }
  /* Maximum capacity of the queue */
  auto capacity() const -> std::size_t { return capacity_; }
  /* Standard Bool conversion for empty queue check */
  operator bool() const { return lenght_ != 0; }

private:
  /* Stores maximum capacity of the queue */
  std::size_t capacity_;
  /* Stores current lenght of the queue */
  std::size_t lenght_{0};
  /* Pointer to the head Node of the queue, meaning the first item of queue */
  NodeP head_{nullptr};
  /* Pointer to the tail Node of the queue, meaning the last item of queue */
  NodeP tail_{nullptr};
};

template <typename __Tp> auto Queue<__Tp>::enqueue(__Tp item) -> void {
  /* Checks if the current lenght exceed max capacity */
  if (lenght_ < capacity_) {
    /* Checks if the queue is empty */
    if (tail_ == nullptr) {
      /* Creates first Node with item parameter */
      tail_ = new Node(item);
      /* Makes head point to tail */
      head_ = tail_;
    } else {
      /* Creates new Node to add */
      NodeP node = new Node(item);
      /* Makes the tail node point to newly created node  */
      tail_->next = node;
      /* Makes the tail point to the new node */
      tail_ = node;
      /* Asserts circular functionality */
      tail_->next = head_;
    }
    /* Updates lenght */
    lenght_++;
  } else /* If lenght is equal to capacity */ {
    /* Updates item in head  */
    head_->value = item;
    /* Makes the tail node point to head */
    tail_->next = head_;
    /* Makes the tail point to head */
    tail_ = head_;
    /* Updates head to the next node */
    head_ = head_->next;
  }
}
template <typename __Tp> auto Queue<__Tp>::dequeue() -> __Tp {
  /* Decreases lenght */
  --lenght_;
  /* Saves variable to return */
  __Tp value = head_->value;
  /* Creates temporary node pointer to help deleting the dequeued node*/
  NodeP tmp = head_;
  /* Updates head */
  head_ = head_->next;
  /* Decoupling the node to be destroyed */
  tmp->next = nullptr;
  /* Checking if the queue is not empty */
  if (lenght_ != 0) {
    /* Restore circular behavior of the queue */
    tail_->next = head_;
  } else {
    /* Updates tail to avoid nullptr dereference */
    tail_ = nullptr;
  }
  /* Deletes dequeued node memory*/
  delete tmp;
  /* Return value*/
  return value;
}

} // namespace ds

/* Namespace for log related stuff */
namespace logs {
/* LogData struct that defines variables to be send to linux host */
template <typename Tp> struct LogData {
  sensor::sensor_id id;
  std::chrono::year_month_day date;
  Tp measure;
  LogData(sensor::sensor_id ID, std::chrono::year_month_day data, Tp sample)
      : id{ID}, date{data}, measure{sample} {};
  friend std::ostream &operator<<(std::ostream &os, LogData<double> const &log);
};

} // namespace logs

/* Used for naming convenience */
using LogHandler =
    std::function<void(ds::Queue<logs::LogData<double>> &, sensor::Measure &)>;

/* Map used for handle individual measurements, needed since dht11 reads two
 * variables at time */
const std::unordered_map<sensor::sensor_id, LogHandler> log_conversion{
    /* Dht11 handler for single read enqueue */
    {sensor::DHT11_ID,
     [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
       logs::LogData<double> temp{sample.last_id, sample.last_time_measure,
                                  sample.temp};
       fila.enqueue(temp);
       logs::LogData<double> hm{sample.last_id, sample.last_time_measure,
                                sample.hm};
       fila.enqueue(hm);
     }},
    /* Gyml8511 handler for single read enqueue */
    {sensor::GYML8511_ID,
     [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
       logs::LogData<double> uv{sample.last_id, sample.last_time_measure,
                                sample.uv};
       fila.enqueue(uv);
     }},
    /* Cjmcu811  handler for single read enqueue */
    {sensor::CJMCU811_ID,
     [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
       logs::LogData<double> air{sample.last_id, sample.last_time_measure,
                                 sample.air};
       fila.enqueue(air);
     }}};
