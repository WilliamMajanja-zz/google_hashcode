#pragma once

#include "../base/common.h"
#include "../base/log.h"
#include "colors.h"
#include <Cimg.h>

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
    LOG("visualize field: " << input.R << 'x' << input.C)
    for (int i = 0; i < input.endpoints.size(); ++i) {
      image_.draw_line(0, input.endpoints[i].L, colors
    }
    image_.display();
  }

private:
  const size_t width_;
  const size_t height_;
  const size_t depth_;
  CImg<unsigned char> image_;

  const std::string class_name_ = "Visualizer";
};
