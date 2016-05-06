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
 * all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <unordered_map>
#include <list>

#include "CacheStrategy.h"

namespace lessdb {

class LRUCacheStrategy final : public CacheStrategy {
  // Slice points at the data of key.
  typedef std::list<Slice> CacheList;

  struct LRUHandle {
    boost::any value;
    CacheList::const_iterator where;
  };

  // key => LRUHandle mapping
  typedef std::unordered_map<std::string, LRUHandle> HandleTable;

 public:
  ~LRUCacheStrategy() override = default;

  LRUCacheStrategy(size_t capacity)
      : CacheStrategy(capacity),
        capacity_(capacity) {}

  HANDLE Insert(const Slice &key, const boost::any &value) override {
    assert(cache_.size() <= capacity_);
    assert(handleTable_.size() == cache_.size());

    auto it = handleTable_.find(key.ToString());
    if (it != handleTable_.end()) {
      // cache hit: remove from cache
      cache_.erase(it->second.where);
      handleTable_.erase(it);
    } else if (cache_.size() == capacity_) {
      // cache full: remove least recently used
      handleTable_.erase(cache_.rbegin()->ToString());
      cache_.erase(std::prev(cache_.end()));
    }

    // insert new entry;
    it = handleTable_.emplace(std::make_pair<std::string, LRUHandle>(
                                  key.ToString(), {value}))
             .first;
    cache_.push_front(it->first);
    it->second.where = cache_.begin();
    return reinterpret_cast<Handle *>(&it->second);
  }

  void Erase(const Slice &key) override {
    auto it = handleTable_.find(key.ToString());
    if (it != handleTable_.end()) {
      cache_.erase(it->second.where);
      handleTable_.erase(it);
    }
  }

  HANDLE Lookup(const Slice &key) override {
    auto it = handleTable_.find(key.ToString());
    if (it == handleTable_.end())
      return NULL;

    // cache hit, remove from cache
    cache_.erase(it->second.where);
    LRUHandle value = std::move(it->second);
    handleTable_.erase(it);

    // push in the front
    it = handleTable_.emplace(std::make_pair(key.ToString(), std::move(value)))
             .first;
    cache_.push_front(it->first);
    it->second.where = cache_.begin();
    return reinterpret_cast<Handle *>(&it->second);
  }

  boost::any Value(HANDLE handle) const override {
    return reinterpret_cast<LRUHandle *>(handle)->value;
  }

 private:
  HandleTable handleTable_;
  CacheList cache_;
  const size_t capacity_;
};

std::unique_ptr<CacheStrategy> CacheStrategy::Default(size_t capacity) {
  return std::unique_ptr<CacheStrategy>(new LRUCacheStrategy(capacity));
}

}  // namespace lessdb