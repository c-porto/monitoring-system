#pragma once
#include <chrono>
#include <functional>
#include <ostream>
#include <unordered_map>

typedef double temperature_t;
typedef double humidity_t;
typedef double light_t;
typedef double air_quality_t;

namespace sensor
{
typedef enum
{
    DHT11_ID,
    GYML8511_ID,
    CJMCU811_ID,
} sensor_id;

struct Measure
{
    std::chrono::year_month_day last_time_measure;
    bool err{false};
    sensor_id last_id{};
    temperature_t temp{};
    humidity_t hm{};
    light_t uv{};
    air_quality_t air{};
    friend std::ostream &operator<<(std::ostream &os, Measure const &ms);
};

using MeasureP = Measure *;

class Sensor
{
  public:
    sensor_id id;
    virtual void read(MeasureP data) = 0;
};

using SensorP = Sensor *;

class SensorAPI
{
  public:
    SensorAPI(SensorP sensor, MeasureP data);
    void update_data();

  private:
    SensorP sensor_{nullptr};
    MeasureP data_{nullptr};
};

} // namespace sensor

namespace ds
{
template <typename Tp> struct Node
{
    Tp value;
    Node *next;
    Node(Tp item) : value{item}, next{nullptr} {};
};

template <typename Tp> class Queue
{
  public:
    Queue() : head_{nullptr}, tail_{nullptr}, lenght_{0} {};
    void enqueue(Tp item);
    Tp dequeue();
    Tp peek();
    size_t lenght()
    {
        return lenght_;
    };

  private:
    Node<Tp> *head_;
    Node<Tp> *tail_;
    size_t lenght_;
};

template <typename Tp> void Queue<Tp>::enqueue(Tp item)
{
    lenght_++;
    if (tail_ == nullptr)
    {
        tail_ = new Node(item);
        head_ = tail_;
    }
    else
    {
        Node<Tp> *node = new Node(item);
        tail_->next = node;
        tail_ = node;
    }
}

template <typename Tp> Tp Queue<Tp>::dequeue()
{
    lenght_--;
    Node<Tp> *head = head_;
    head_ = head_->next;
    Tp value = head->value;
    head->next = nullptr;
    if (lenght_ == 0)
    {
        this->tail = nullptr;
    }
    delete head;
    return value;
}

template <typename Tp> Tp Queue<Tp>::peek()
{
    return head_->value;
}

} // namespace ds

namespace logs
{
template <typename Tp> struct LogData
{
    sensor::sensor_id id;
    std::chrono::year_month_day date;
    Tp measure;
    LogData(sensor::sensor_id ID, std::chrono::year_month_day data, Tp sample) : id{ID}, date{data}, measure{sample} {};
    friend std::ostream &operator<<(std::ostream &os, LogData const &log);
};

} // namespace logs

using log_handler = std::function<void(ds::Queue<logs::LogData<double>> &, sensor::Measure &)>;

const std::unordered_map<sensor::sensor_id, log_handler> log_conversion{
    {sensor::DHT11_ID,
     [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
         logs::LogData<double> temp{sample.last_id, sample.last_time_measure, sample.temp};
         fila.enqueue(temp);
         logs::LogData<double> hm{sample.last_id, sample.last_time_measure, sample.hm};
         fila.enqueue(hm);
     }},
    {sensor::GYML8511_ID,
     [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
         logs::LogData<double> uv{sample.last_id, sample.last_time_measure, sample.uv};
         fila.enqueue(uv);
     }},
    {sensor::CJMCU811_ID, [](ds::Queue<logs::LogData<double>> &fila, sensor::Measure &sample) {
         logs::LogData<double> air{sample.last_id, sample.last_time_measure, sample.air};
         fila.enqueue(air);
     }}};
