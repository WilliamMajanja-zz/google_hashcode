#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

#include <optional>

using Pos = std::pair<int64_t, int64_t>;

int64_t Distance(Pos lhs, Pos rhs) {
    return std::abs(lhs.first - rhs.first) + std::abs(lhs.second - rhs.second);
}

struct RideInfo {
    RideInfo() = default;
    RideInfo(int id, Pos start_pos, Pos finish_pos, int64_t min_start_time, int64_t max_finish_time)
        : id_(id)
        , start_pos_(start_pos)
        , finish_pos_(finish_pos)
        , min_start_time_(min_start_time)
        , max_finish_time_(max_finish_time) {
    }

    // id in input order
    int id_; 
    bool finished_{false};

    Pos start_pos_;
    Pos finish_pos_;

    int64_t min_start_time_;
    int64_t max_finish_time_;

    bool operator<(const RideInfo& rhs) const {
        return Distance(start_pos_, finish_pos_) > Distance(rhs.start_pos_, rhs.finish_pos_);
    }
};

struct State {
    Pos pos_{0, 0};
    int turn_{0};
};

int ProvideFakeId() {
    static int state = 0;
    return state++;
}

struct CarInfo {
    // id in input order
    CarInfo(int id) : real_id_(id) {
        fake_id_ = ProvideFakeId();
    }

    int real_id_;
    int fake_id_;

    State state_;
    std::optional<State> promise_state_;

    std::vector<int> rides_;

    bool operator<(const CarInfo& rhs) const {
        return fake_id_ < rhs.fake_id_;
    }
};

class Solution : public BaseSolution { public:
    template <typename... T>
    Solution(T&&... args):
        BaseSolution(std::forward<T>(args)...) {}

    void solve_internal(const Input& input, Output& output) override {
        std::vector<RideInfo> rides;
        rides.reserve(input.N);
        for (int i = 0; i < input.N; ++i) {
            const auto& inputRide = input.rds[i];
            rides.emplace_back(i, inputRide.st, inputRide.fin, inputRide.s, inputRide.f);
        }

        std::sort(rides.begin(), rides.end());

        std::vector<int> ride_id(input.N);
        for (int i = 0; i < input.N; ++i) {
            ride_id[rides[i].id_] = i;
        }

        std::set<CarInfo> cars;
        for (int i = 0; i < input.F; ++i) {
            cars.emplace(i);
        }

        output.rds.resize(input.F);
        for (int i = 0; i < input.N; ++i) {
            auto& ride = rides[i];

            const CarInfo* best_car = nullptr;
            int max_score = 0;
            int min_loss = 0;

            for (auto& car : cars) {
                auto can_place = [&](const auto& r, const auto& c) {
                    assert(r.min_start_time_ + Distance(r.start_pos_, r.finish_pos_) <= r.max_finish_time_);
                    int ft = std::max(r.min_start_time_, c.state_.turn_ + Distance(c.state_.pos_, r.start_pos_)) + Distance(r.start_pos_, r.finish_pos_);
                    bool ok = ft <= r.max_finish_time_;
                    if (c.promise_state_.has_value()) {
                        auto promise_state = c.promise_state_.value();
                        ok &= ft + Distance(r.finish_pos_, promise_state.pos_) < promise_state.turn_;
                    }
                    return ok;
                };

                if (can_place(ride, car)) {
                    int score = Distance(ride.start_pos_, ride.finish_pos_);
                    if (car.state_.turn_ + Distance(car.state_.pos_, ride.start_pos_) <= ride.min_start_time_) {
                        score += input.B;
                    }
                    const auto arrive_time = car.state_.turn_ + Distance(car.state_.pos_, ride.start_pos_);
                    const auto loss = std::abs(arrive_time - ride.min_start_time_);
                    if (!best_car || (score > max_score) || (score == max_score && loss < min_loss)) {
                        best_car = &car;
                        max_score = score;
                        min_loss = loss;
                    }
                }
            }

            if (best_car) {
                output.rds[best_car->real_id_].emplace_back(ride.id_);
                {
                    CarInfo new_car_info(best_car->real_id_);
                    new_car_info.state_ = best_car->state_;

                    State promise_state;
                    promise_state.turn_ = std::max(ride.min_start_time_, best_car->state_.turn_ + Distance(best_car->state_.pos_, ride.start_pos_));
                    promise_state.pos_ = ride.start_pos_;

                    new_car_info.promise_state_ = promise_state;

                    cars.emplace(std::move(new_car_info));
                }
                {
                    CarInfo new_car_info(best_car->real_id_);
                    State start_state;

                    int ft = std::max(ride.min_start_time_, best_car->state_.turn_ + Distance(best_car->state_.pos_, ride.start_pos_)) + Distance(ride.start_pos_, ride.finish_pos_);
                    start_state.turn_ = ft;
                    start_state.pos_ = ride.finish_pos_;

                    new_car_info.state_ = start_state;
                    new_car_info.promise_state_ = best_car->promise_state_;

                    cars.emplace(std::move(new_car_info));
                }
                cars.erase(*best_car);
            }
        }
    }

private:
    const string class_name_ = "Solution";
};

