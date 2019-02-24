#pragma once

#include "common.h"

void validate(const Input& input, const Output& output, bool enable_logging = true) {
  const auto& rides = output.rds;
  ASSERT(rides.size() == input.F, "Size of output is not equal to F: " << rides.size() << " != " << input.F);
  vector<char> assigned(input.N);

  for (int i = 0; i < rides.size(); ++i) {
    const auto& path = rides[i];
    for (int j = 0; j < path.size(); ++j) {
      ASSERT(0 <= path[j] && path[j] < input.N, "No ride #" << path[j] << ", max ride number is " << input.N - 1);
      ASSERT(!assigned[path[j]], "Ride #" << path[j] << " assigned twice");
      assigned[path[j]] = true;
    }
  }

  LOG("output format valid")
}

int distance(int x_0, int y_0, int x_1, int y_1) {
  return abs(x_0 - x_1) + abs(y_0 - y_1);
}

int calculate_score(const Input& input, const Output& output, bool enable_logging = true) {
  validate(input, output, enable_logging);

  int max_score = 0;
  for (const Ride& ride : input.rds) {
    max_score += distance(ride.st.first, ride.st.second,
                          ride.fin.first, ride.fin.second);
    max_score += input.B;
  }

  int score = 0;
  const auto& rides = output.rds;
  for (int veh_id = 0; veh_id < rides.size(); ++veh_id) {
    int score_for_veh = 0;
    int time = 0;
    const auto& path = rides[veh_id];
    int pos_x = 0, pos_y = 0;
    int number_of_bonuses = 0;
    int number_of_deliveries = 0;
    int idleness = 0;
    for (int ride_id = 0; ride_id < path.size(); ++ride_id) {
      Ride ride = input.rds[path[ride_id]];
      bool in_time = false;
      bool delivered = false;

      int time_to_start = distance(pos_x, pos_y, ride.st.first, ride.st.second);
      time += time_to_start;
      pos_x = ride.st.first, pos_y = ride.st.second;

      if (time <= ride.s) {
        in_time = true;
        idleness += ride.s - time;
        time = ride.s;
      }

      int time_to_end = distance(pos_x, pos_y, ride.fin.first, ride.fin.second);
      time += time_to_end;
      pos_x = ride.fin.first, pos_y = ride.fin.second;

      if (time <= ride.f) {
        delivered = true;
      }

      score_for_veh += in_time * input.B + delivered * time_to_end;
      number_of_bonuses += in_time;
      number_of_deliveries += delivered;
    }
    LOG("Score for vehicle #" << veh_id << " = " << score_for_veh << ", total deliveries " << path.size() << ", successful deliveries " << number_of_deliveries << ", with bonuses " << number_of_bonuses << ", idleness " << idleness);
    score += score_for_veh;
  }

  LOG("Max score (with all bonuses) = " << max_score);
  
  return score;
}
