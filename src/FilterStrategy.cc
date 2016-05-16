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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>
#include <boost/functional/hash.hpp>
#include "FilterStrategy.h"
#include "Slice.h"

namespace lessdb {

class BloomFilterStrategy : public FilterStrategy {
  //
  // m: the total number of bits our bloom filter has.
  // n: the number of elements.
  // k: the number of hash functions.
  // For a given m and n, the value of k that minimizes the false positive
  // probability is ln2*(m/n).
  //
  // Use double-hashing to generate a sequence of hash values.
  // See analysis in [Kirsch,Mitzenmacher 2006].
  // The double-hashing functions is of the form gi(x) = h1(x) + i*h2(x).
  //

  const size_t kMinBloomFilterLength = 64;

 public:
  BloomFilterStrategy(size_t bits_per_key) : bits_per_byte_(bits_per_key) {
    k_ = static_cast<size_t>(bits_per_key * 0.69);  // ln2 ~= 0.69
  }

  inline void Put(const Slice &key, Slice &bits) override {
    size_t h1 = boost::hash_range(key.RawData(), key.RawData() + key.Len());
    size_t h2 = (h1 >> 17) | (h1 << 15);  // Rotate right 17 bits
    for (size_t i = 0; i < k_; ++i) {
      size_t g = (h1 + i * h2) % bits.Len();
      bits[g / 8] |= (1 << (g % 8));
    }
  }

  inline bool MightContain(const Slice &key, const Slice &bits) const override {
    size_t h1 = boost::hash_range(key.RawData(), key.RawData() + key.Len());
    size_t h2 = (h1 >> 17) | (h1 << 15);  // Rotate right 17 bits
    for (size_t i = 0; i < k_; ++i) {
      size_t g = (h1 + i * h2) % bits.Len();
      if (!(bits[g / 8] & (1 << (g % 8))))
        return false;
    }
    return true;
  }

 private:
  size_t bits_per_byte_;
  size_t k_;
};

FilterStrategy *FilterStrategy::Default(size_t bits_per_bytes) {
  return new BloomFilterStrategy(bits_per_bytes);
}

}  // namespace lessdb
