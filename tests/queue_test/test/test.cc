#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <iterator>
#include <queue.h>
#include <string>

/* Testing C++ macros*/
#define ARRAY_MOVE(size)                                                       \
  static_assert(size == 10, "Expected size 10 array");                         \
  inline auto array_move(std::array<std::string, size> &&arr) {                \
    return std::array<std::string, size>{arr};                                 \
  }

/* Static Global Variables*/
namespace {
std::array<std::string, 10> nodes = {"Node 1", "Node 2", "Node 3", "Node 4",
                                     "Node 5", "Node 6", "Node 7", "Node 8",
                                     "Node 9", "Node 10"};
} // namespace

TEST(QueueTest, ConstructorTest) {
  ds::Queue<std::string> fila{5};
  EXPECT_EQ(fila.capacity(), 5) << "Unequal capacity from constructor";
  EXPECT_EQ(fila.lenght(), 0) << "Unequal lenght from constructor";
}

TEST(QueueTest, EnqueueDequeueTest) {
  ds::Queue<std::string> fila{5};
  for (std::size_t i{0}; i < 5; ++i) {
    fila.enqueue(nodes[i]);
    EXPECT_EQ(fila.lenght(), 1) << "Unequal lenght after enqueueing ";
    auto tmp = fila.dequeue();
    EXPECT_EQ(fila.lenght(), 0) << "Unequal lenght after dequeueing ";
    EXPECT_EQ(tmp, nodes[i]);
  }
}

TEST(QueueTest, BooleanConversion) {
  ds::Queue<std::string> fila{1};
  std::size_t y{0};
  if (fila) {
    y = 1;
  }
  EXPECT_EQ(y, 0) << "Error in boolean conversion";
  fila.enqueue(nodes[1]);
  if (fila) {
    y = 1;
  }
  EXPECT_EQ(y, 1) << "Error in boolean conversion";
}

TEST(QueueTest, CircularTest) {
  ds::Queue<std::string> fila{5};
  for (std::size_t i{0}; i < nodes.size(); ++i) {
    fila.enqueue(nodes[i]);
  }
  bool completed_loop{true};
  std::size_t j{5};
  std::size_t initial_len{fila.lenght()};
  while (fila && completed_loop) {
    EXPECT_EQ(initial_len, fila.lenght()) << "Error in lenght decrement";
    std::string node = fila.dequeue();
    EXPECT_EQ(node, nodes[j]) << "Error in circular behavior";
    j++;
    initial_len--;
    if (j == 9) {
      completed_loop = !completed_loop;
    }
  }
}

TEST(QueueTest, MaxCapacityTest) {
  ds::Queue<std::string> fila{1};
  ds::Queue<std::string> queue{5};
  for (std::size_t i{0}; i < 1000000; ++i)
    fila.enqueue(nodes[0]);
  EXPECT_EQ(fila.lenght(), 1) << "Error in Unitary queue";
  for (std::size_t i{0}; i < 100000; ++i)
    for (const auto &n : nodes)
      queue.enqueue(n);
  EXPECT_EQ(queue.lenght(), 5) << "Error in Unitary queue";
}

/* ARRAY_MOVE(10) */

/*
TEST(Macros, FunctionConstructTest) {
  auto A = array_move(std::move(nodes));
  EXPECT_EQ(A.size(), 10) << "Error in array size move";
  EXPECT_EQ(A[0], "Node 1") << "Error in value move";
}
*/
