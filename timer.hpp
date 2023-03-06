//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef timer_hpp
#define timer_hpp

#include <cstdint>
#include <string>
#include <chrono>

namespace util {
    //=================================================================================
    class timer_t{
        std::chrono::time_point<std::chrono::steady_clock> start_time ;
        std::int64_t time_duration ;
    public:
        static auto now() ->std::string ;
        timer_t();
        timer_t(std::int64_t milliseconds,bool block);
        auto time(std::int64_t milliseconds,bool block)->void;

        auto start() ->void ;
        auto elapsed() const ->std::int64_t ;
        auto expired() const ->bool;
        auto remaining() const -> std::int64_t ;
    };
}

#endif /* timer_hpp */
