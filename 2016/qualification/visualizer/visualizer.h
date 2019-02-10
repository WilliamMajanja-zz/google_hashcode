#pragma once

#include "../base/common.h"
#include "../base/log.h"
#include "colors.h"
#include <CImg.h>

using namespace cimg_library;

class Visualizer {
public:
  Visualizer(size_t height, size_t width, size_t depth = 1):
      width_(width),
      height_(height),
      depth_(depth),
      image_(width, height, depth, /*channel_count = */ 3) {
    image_.fill(0);
  }

  void visualize_output(const Input& input, const Output& output) {
    LOG("started")
    init_values(input);
    visualize_input(input);

    image_.display();
    LOG("finished")
  }

  void visualize_input(const Input& input) {
    LOG("started")

    draw_objects(input.shops, colors::red);
    draw_objects(input.orders, colors::green);

    LOG("finished")
  }

private:
  void init_values(const Input& input) {
    n_rows = input.n_rows;
    n_cols = input.n_cols;
  }


  std::pair<std::pair<int, int>, std::pair<int, int>> get_rect(int row, int col) {
    double block_height = double(image_.height()) / n_rows;
    double block_width = double(image_.width()) / n_cols;
    return {
      {int(col * block_width), int(row * block_height)},
      {int((col + 1) * block_width), int((row + 1) * block_height)}
    };
  }

  template <typename T>
  void draw_objects(const vector<T>& objs, const unsigned char* color) {
    for (const T& obj : objs) {
      auto [corn1, corn2] = get_rect(obj.row, obj.col);
      image_.draw_rectangle(corn1.first, corn1.second, corn2.first, corn2.second, color);
    }
  }

  void draw_grid(int n_rows, int n_cols) {
    vector<int> values_x(n_cols);
    for (int i = 0; i < n_cols; ++i) {
      values_x[i] = int(i * double(image_.width()) / n_cols);
    }
    vector<int> values_y(n_cols);
    for (int i = 0; i < n_rows; ++i) {
      values_y[i] = int(i * double(image_.height()) / n_rows);
    }
    // image_.draw_grid(values_x, values_y, colors::white, 0.1);
  }

private:
  int n_rows;
  int n_cols;

private:
  const size_t width_;
  const size_t height_;
  const size_t depth_;
  CImg<unsigned char> image_;

  const std::string class_name_ = "Visualizer";
};
