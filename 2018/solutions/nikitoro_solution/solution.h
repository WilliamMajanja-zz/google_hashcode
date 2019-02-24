#pragma once

#include "../../base/solution.h"
#include "../../base/calculate_score.h"

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
        , max_finish_time_(max_finish_time)
    {
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

struct CarInfo {
    // id in input order
    CarInfo() = default;
    CarInfo(int id) : id_(id) {
    }

    CarInfo(int id, const std::vector<RideInfo> rides) : id_(id) {
        for (const auto& ride : rides) {
            pos_ = ride.finish_pos_;
            turn_ = std::max(ride.min_start_time_, turn_ + Distance(pos_, ride.start_pos_)) +
                    Distance(ride.start_pos_, ride.finish_pos_);
            rides_.emplace_back(ride.id_);
        }

    }

    int id_;

    Pos pos_{0, 0};
    int64_t turn_{0};

    std::vector<int> rides_;
};

class Solution : public BaseSolution {
public:
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

        std::vector<CarInfo> cars;
        cars.reserve(input.F);
        for (int i = 0; i < input.F; ++i) {
            cars.emplace_back(i);
        }

        std::sort(rides.begin(), rides.end());

        std::vector<int> ride_id(input.N);
        for (int i = 0; i < input.N; ++i) {
            ride_id[rides[i].id_] = i;
        }

        auto try_insert = [&] {
            for (auto& ride : rides) {
                if (ride.finished_) {
                    continue;
                }

                auto can_place = [](const auto& r, const auto& c) {
                    assert(r.min_start_time_ + Distance(r.start_pos_, r.finish_pos_) <= r.max_finish_time_);
                    return c.turn_ + Distance(c.pos_, r.start_pos_) + Distance(r.start_pos_, r.finish_pos_) <= r.max_finish_time_;
                };
                std::vector<int64_t> avaliable_cars;
                for (int i = 0; i < input.F; ++i) {
                    const auto& car = cars[i];
                    if (can_place(ride, car)) {
                        avaliable_cars.emplace_back(i);
                    }
                }

                if (avaliable_cars.empty()) {
                    continue;
                }

                std::sort(avaliable_cars.begin(), avaliable_cars.end(), [&cars, &ride](const auto& lhs, const auto& rhs) {
                    auto loss = [&](const auto& it) {
                        const auto arrive_time = it.turn_ + Distance(it.pos_, ride.start_pos_);
                        return std::abs(arrive_time - ride.min_start_time_);
                    };
                    return loss(cars[lhs]) < loss(cars[rhs]);
                });

                auto& car = cars[avaliable_cars.front()];
                car.pos_ = ride.finish_pos_;
                car.turn_ = std::max(ride.min_start_time_, car.turn_ + Distance(car.pos_, ride.start_pos_)) +
                            Distance(ride.start_pos_, ride.finish_pos_);
                car.rides_.emplace_back(ride.id_);

                ride.finished_ = true;
            }
        };

        try_insert();

        output.rds.resize(input.F);
        for (int i = 0; i < input.F; ++i) {
            output.rds[i] = cars[i].rides_;
        }

        int base_score = calculate_score(input, output, false);
        LOG(" base score is " << base_score);

        LOG("start improver");

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<int> rand_car(0, input.F - 1);
        for (int iter = 0; iter < 10000; ++iter) {
            std::vector<CarInfo> was = cars;

            for (int attempt = 0; attempt < input.F / 20; ++attempt) {
                int car_id = rand_car(gen);
                auto& car = cars[car_id];
                car.pos_ = {0, 0};
                car.turn_ = 0;
                for (auto j : car.rides_) {
                    rides[ride_id[j]].finished_ = false;
                }
                car.rides_.clear();
            }

            try_insert();

            Output new_output;
            new_output.rds.resize(input.F);
            for (int i = 0; i < input.F; ++i) {
                new_output.rds[i] = cars[i].rides_;
            }

            int score = calculate_score(input, new_output, false);
            if (score > base_score) {
                base_score = score;
                std::swap(new_output, output);
                LOG("new best score is " << base_score);
            } else {
                cars = was;
            }
        }
    }

private:
    const string class_name_ = "Solution";
};

