#define MAX_SOLVING_THREAD_COUNT 18
#pragma once

#include "log.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

#define X first
#define Y second
#define PT pair<int, int>

using namespace std;

string class_name_;
bool enable_logging = true;

struct Endpoint {
  int L, K;
  vector<PT> connections;
};

struct Request {
  int V, E, N;
};

struct Input {
  int V, E, R, C, X;
  vector<int> videos;
  vector<Endpoint> endpoints;
  vector<Request> requests;
};

struct Output {
  vector<vector<int>> servers;
};

inline Input read_input(const std::string& fname) {
  ifstream in_f(fname);
  Input in;
  in_f >> in.V >> in.E >> in.R >> in.C >> in.X;
  for (int i = 0; i < in.V; ++i) {
    int x;
    in_f >> x;
    in.videos.push_back(x);
  }
  LOG("read videos count: " << in.videos.size())
  for (int i = 0; i < in.E; ++i) {
    Endpoint endp;
    in_f >> endp.L >> endp.K;
    for (int j = 0; j < endp.K; ++j) {
      int c, L;
      in_f >> c >> L;
      endp.connections.emplace_back(c, L);
    }
    in.endpoints.push_back(move(endp));
  }
  LOG("read endpoints count: " << in.endpoints.size())
  for (int i = 0; i < in.R; ++i) {
    Request req;
    in_f >> req.V >> req.E >> req.N;
    in.requests.push_back(req);
  }
  LOG("read requests count: " << in.requests.size())
  return in;
}

inline void print_output(const Output& output, size_t score, const std::string& fpath = "./") {
  auto fname = fpath + to_string(score) + ".ans";
  fstream out_f(fname, fstream::out);

  out_f << output.servers.size() << '\n';
  for (const auto& server : output.servers) {
    for (auto qwe : server) {
      out_f << qwe << ' ';
    }
    out_f << '\n';
  }

  LOG("output has been printed to file: " << fname)
}
