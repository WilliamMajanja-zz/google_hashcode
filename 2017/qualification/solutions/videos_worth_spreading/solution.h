#pragma once

#include "../../base/solution.h"

class Solution : public BaseSolution {
public:
  template <typename... T>
  Solution(T&&... args):
      BaseSolution(std::forward<T>(args)...) {}

  void solve_internal(const Input& input, Output& output) override {
    LOG("started")
    output.servers.resize(input.C);
    vector<vector<double>> serv_to_vid(input.C, vector<double>(input.V));
    vector<vector<Request>> req_by_vid(input.V);
    vector<vector<bool>> used_req_by_vid(input.V);
    vector<unordered_map<int, int>> end_to_serv(input.E);
    for (const auto& req : input.requests) {
      req_by_vid[req.V].push_back(req);
    }
    for (int i = 0; i < input.endpoints.size(); ++i) {
      for (auto con : input.endpoints[i].connections) {
        end_to_serv[i][con.first] = con.second;
      }
    }
    for (int vid = 0; vid < input.V; ++vid) {
      if (vid % 100 == 0) {
        LOG("init vid: " << vid << " total: " << input.V)
      }
      for (int sid = 0; sid < input.C; ++sid) {
        for (const auto& req : req_by_vid[vid]) {
          if (end_to_serv[req.E].find(sid) != end_to_serv[req.E].end()) {
            int old_lat = input.endpoints[req.E].L;
            int new_lat = end_to_serv[req.E][sid];
            serv_to_vid[sid][vid] += old_lat - new_lat;
          }
        }
      }
    }
    LOG("serv_to_vid inited")

    vector<int> sz(input.C);
    while (1) {
      int sid = 0;
      int vid = 0;
      for (int i = 0; i < input.C; ++i) {
        for (int j = 0; j < input.V; ++j) {
          if (serv_to_vid[i][j] > serv_to_vid[sid][vid]) {
            sid = i;
            vid = j;
          }
        }
      }
      if (serv_to_vid[sid][vid] == 0) {
        break;
      }
      sz[sid] += input.videos[vid];
      if (input.videos[vid] + sz[sid] <= input.X) {
        output.servers[sid].push_back(vid);
        LOG("add video: " << vid << " to server: " << sid <<
            " size: " << input.videos[vid] << 
            " improve: " << serv_to_vid[sid][vid] << " free space: " << input.X - sz[sid])
      }
      serv_to_vid[sid][vid] = 0;

      for (int i = 0; i < input.C; ++i) {
        for (const auto& req : req_by_vid[vid]) {
          if (end_to_serv[req.E].find(sid) != end_to_serv[req.E].end()) {
            if (end_to_serv[req.E].find(i) != end_to_serv[req.E].end()) {
              int old_lat = input.endpoints[req.E].L;
              int new_lat = end_to_serv[req.E][i];
              serv_to_vid[i][vid] -= old_lat - new_lat;
              serv_to_vid[i][vid] = max(0.0, serv_to_vid[i][vid]);
            }
          }
        }
      }
    }

    for (auto& serv : output.servers) {
      sort(serv.begin(), serv.end());
    }
    LOG("finished")
  }

private:
  const string class_name_ = "Solution";
};

