/**
 * Copyright (C) 2016, Wu Tao. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRiANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <array>
#include <benchmark/benchmark.h>
#include <exception>

#include "Status.h"
#include "StatusUtils.h"
#include "utils/Random.h"

using namespace lessdb;

static Status errorStatus(int i) {
  return DTHROW(Status::IOError(" ") << i);
}

struct SimpleException : public std::exception {
  SimpleException(const char *s) : s_(s) {}

  SimpleException &operator<<(int i) {
    std::ostringstream oss;
    oss << i;
    s_ += oss.str();
    return *this;
  }

 private:
  std::string s_;
};

static void errorException(int i) {
  throw(SimpleException(" ") << i);
}

static void randomDataSet(int size, std::vector<int> *a) {
  for (int i = 0; i < size; i++) {
    a->push_back(test::RandomIn(0, 1));
  }
}

static void StatusUtil_ThrowCatch(benchmark::State &state) {
  std::vector<int> a;
  randomDataSet(state.range(0), &a);

  while (state.KeepRunning()) {
    for (int i = 0; i < a.size(); i++) {
      if (a[i] == 0) {
        Status s = errorStatus(i);
        DTRACE(s);
      }
    }
  }
}

static void Exception_ThrowCatch(benchmark::State &state) {
  std::vector<int> a;
  randomDataSet(state.range(0), &a);

  while (state.KeepRunning()) {
    for (int i = 0; i < a.size(); i++) {
      if (a[i] == 0) {
        try {
          errorException(i);
        } catch (std::exception &e) {
        }
      }
    }
  }
}

static void Arguments(benchmark::internal::Benchmark *b) {
  b->Arg(100)->Arg(1000)->Arg(2000)->Arg(3000)->Arg(10000)->Arg(20000);
}

/**
 * According to our benchmark, StatusUtils's about 25% faster than exception,
 * though I've no idea where the improvement are yet. But more importantly,
 * when using StatusUtils you can take the choice whether to trace the stack
 * and discard the meaningless stack traces. By exception you have to record
 * all of the traces even though some of them are redundant. So in general
 * StatusUtils may be both theoritically and practically having better
 * performance.
 *
 * Benchmark enviroment:
 * Intel i5-6200U CPU @ 2.30GHz × 4, 7.2G Memory,
 * g++ (Ubuntu 5.4.0-6ubuntu1~16.04.2) 5.4.0, Release mode.
 *
 * Benchmark                               Time           CPU Iterations
 * ---------------------------------------------------------------------
 * StatusUtil_ThrowCatch/100          115890 ns     116009 ns       6034
 * StatusUtil_ThrowCatch/1000         880743 ns     877915 ns        729
 * StatusUtil_ThrowCatch/2.92969k    2894190 ns    2894309 ns        246
 * StatusUtil_ThrowCatch/9.76562k    9608284 ns    9643836 ns         73
 * StatusUtil_ThrowCatch/19.5312k   20996846 ns   21058824 ns         34
 * StatusUtil_ThrowCatch/1.95312k    1833844 ns    1827957 ns        372
 * Exception_ThrowCatch/100           138626 ns     138522 ns       5833
 * Exception_ThrowCatch/1000         1110022 ns    1111111 ns        648
 * Exception_ThrowCatch/1.95312k     2262170 ns    2254072 ns        307
 * Exception_ThrowCatch/2.92969k     3306050 ns    3310345 ns        203
 * Exception_ThrowCatch/9.76562k    11251745 ns   11266667 ns         60
 * Exception_ThrowCatch/19.5312k    22995823 ns   22933333 ns         30
 */

BENCHMARK(StatusUtil_ThrowCatch)->Apply(Arguments);
BENCHMARK(Exception_ThrowCatch)->Apply(Arguments);

BENCHMARK_MAIN();