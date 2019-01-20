#pragma once

#include "common.h"
#include "async_solution.h"

template<class TSolution, class TSplitAndMerge>
class SolutionManager {
public:
  SolutionManager(const std::string& fname):
      input_(read_input(fname)),
      splitted_input_(TSplitAndMerge::split_input(input_)),
      free_threads_count_(MAX_SOLVING_THREAD_COUNT) {}

  const auto& input() const { return input_; }
  const auto& splitted_input() const { return splitted_input_; }
  const auto& output() const { return output_; }

  void init_solutions() {
    for (int solution_number = 0; solution_number < splitted_input_.size(); ++solution_number) {
      solutions_.push_back(
          make_unique<TSolution>(
              splitted_input_[solution_number],
              solution_number,
              free_threads_count_
          )
      );
    }
    LOG("finished: solutions inited: " << solutions_.size())
  }

  void run_solutions() {
    for (auto& solution : solutions_) {
      solution->start_solving_thread();
      while (free_threads_count_ == 0) {}
    }
    for (auto& solution : solutions_) {
      solution->join();
    }
    LOG("finished solutions count: " << solutions_.size())
    for (auto& solution : solutions_) {
      splitted_output_.push_back(solution->extract_output());
    }
    LOG("all output extracted")
  }

  void merge_output() {
    output_ = TSplitAndMerge::merge_output(input_, splitted_output_);
    LOG("all output merged")
  }

private:
  Input input_;
  Output output_;
  vector<Input> splitted_input_;
  vector<Output> splitted_output_;
  vector<std::unique_ptr<AsyncSolution>> solutions_;
  atomic<size_t> free_threads_count_;

  const string class_name_ = "SolutionManager";
};
