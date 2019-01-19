#pragma once

#include "common.h"

class AsyncSolution {
public:
  AsyncSolution(const Input& input, size_t number, atomic<size_t>& free_threads_count):
    input_(input),
    number_(number),
    free_threads_count_(free_threads_count) {}

  virtual ~AsyncSolution() = default;

  const auto& input() const { return input_; }
  const auto& output() { return output_; }
  const auto& number() { return number_; }

  auto extract_output() { return move(output_); }

  void start_solving_thread() {
    assert(free_threads_count_ > 0);
    --free_threads_count_;
    LOG("start solving thread for solution: " << number_)
    solving_thread_ = make_unique<thread>([this]{this->solve();});
  }

  void join() { solving_thread_->join(); }

protected:
  virtual void solve_internal() = 0;

  const Input& input_;
  Output output_;

private:
  void solve() {
    solve_internal();
    ++free_threads_count_;
  }

  size_t number_;
  std::atomic<size_t>& free_threads_count_;
  std::unique_ptr<std::thread> solving_thread_;

  const string class_name_ = "AsyncSolution";
};
