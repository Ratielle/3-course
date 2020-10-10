#pragma once

#include <chrono>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

class LogDuration {
 public:
  explicit LogDuration(const std::string &msg = "")
      : message(msg + ": "), start(std::chrono::steady_clock::now()) {}

  ~LogDuration() {
    using namespace std::chrono;
    auto finish = steady_clock::now();
    auto dur = finish - start;
    std::ostringstream os;
    os << message;
    const double pm = 1. / 1000;
    const int step = 100;
    if (duration_cast<seconds>(dur).count() > step) {
      os << duration_cast<seconds>(dur).count() << " s";
    } else if (duration_cast<seconds>(dur).count() > 0) {
      os << duration_cast<milliseconds>(dur).count() * pm << " s";
    } else if (duration_cast<milliseconds>(dur).count() > step) {
      os << duration_cast<milliseconds>(dur).count() << " ms";
    } else if (duration_cast<milliseconds>(dur).count() > 0) {
      os << duration_cast<microseconds>(dur).count() * pm << " ms";
    } else if (duration_cast<microseconds>(dur).count() > step) {
      os << duration_cast<microseconds>(dur).count() << " us";
    } else if (duration_cast<microseconds>(dur).count() > 0) {
      os << duration_cast<nanoseconds>(dur).count() * pm << " us";
    } else {
      os << duration_cast<nanoseconds>(dur).count() << " ns";
    }
    os << std::endl;
    std::cerr << os.str();
  }

 private:
  std::string message;
  std::chrono::steady_clock::time_point start;
};

#ifndef UNIQ_ID
#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)
#endif

#define LOG_DURATION(message) LogDuration UNIQ_ID(__LINE__){message};