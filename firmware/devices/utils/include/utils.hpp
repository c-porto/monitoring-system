#pragma once
#include <chrono>
#include <functional>
#include <memory>
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
