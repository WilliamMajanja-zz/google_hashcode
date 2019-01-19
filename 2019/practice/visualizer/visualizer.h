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
    /*
    for (int i = 0; i < input.R; ++i) {
      for (int j = 0; j < input.C; ++j) {
        unsigned char color = min(255, output.coolwarm[i][j] * 4);
        unsigned char cc[] = {color, color, color};
        image_.draw_point(i, j, cc);
      }
    }
    */
    for (auto sl : output.sls) {
      for (int i = sl.st.X; i < sl.fin.X + 1; ++i) {
        for (int j = sl.st.Y; j < sl.fin.Y + 1; ++j) {
          if (i == sl.st.X || i == sl.fin.X || j == sl.st.Y || j == sl.fin.Y) {
            image_.draw_point(i, j, colors::red);
          } else {
            image_.draw_point(i, j, colors::red);
          }
        }
      }
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
