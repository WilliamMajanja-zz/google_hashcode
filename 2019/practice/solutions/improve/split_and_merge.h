#pragma once

class ImproveSplitAndMerge {
public:
  static vector<Input> split_input(const Input& input) {
    const string class_name_ = "ImproveSpliteAndMerge";
    vector<Input> result{};

    const int kLen = 50;
    for (int i = 0; i < input.R; i += kLen) {
      for (int j = 0; j < input.C; j += kLen) {
        int ii = min(i + kLen, input.R);
        int jj = min(j + kLen, input.C);
        Input new_input;
        new_input.R = ii - i;
        new_input.C = jj - j;
        new_input.L = input.L;
        new_input.H = input.H;

        new_input.pizza.assign(new_input.R, vector<char>(new_input.C));
        for (int k = i; k < ii; ++k) {
          for (int q = j; q < jj; ++q) {
            new_input.pizza[k - i][q - j] = input.pizza[k][q];
          }
        }

        new_input.pointer = make_pair(i, j);
        result.emplace_back(move(new_input));
      }
    }

    LOG("finished splitting into " << result.size() << " inputs")
    return result;
  }

  static Output merge_output(const Input& input, const vector<Output>& splitted_output) {
    const string class_name_ = "ImproveSpliteAndMerge";
    Output result;
    for (const auto& qwe : splitted_output) {
      for (auto sl : qwe.sls) {
        sl.st.X += qwe.pointer.X;
        sl.st.Y += qwe.pointer.Y;
        sl.fin.X += qwe.pointer.X;
        sl.fin.Y += qwe.pointer.Y;
        result.sls.emplace_back(sl);
      }
    }
    LOG("finished merging " << splitted_output.size() << " outputs")
    return result;
  }
};
