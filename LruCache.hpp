#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <cstddef>
#include <list>
#include <unordered_map>

namespace caches {

template <typename T, typename KeyT>
class LruCache
{
private:
    size_t size_;

    std::list<std::pair<KeyT, T>> cache_;

    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;

    std::unordered_map<KeyT, ListIt> hash_;

public:
    LruCache(size_t sz = 10) : size_(sz) {}

    bool full() const {
        return (cache_.size() == size_);
    }

    template <typename F>
    bool lookup_update(KeyT key, F get_page);
};

template <typename T, typename KeyT>
template <typename F>
bool LruCache<T, KeyT>::lookup_update(KeyT key, F get_page) {

    auto hit = hash_.find(key);

    if (hit == hash_.end()) {

        if (full()) {
            hash_.erase(cache_.back().first);
            cache_.pop_back();
        }

        cache_.emplace_front(key, get_page(key));

        hash_.emplace(key, cache_.begin());

        return false;
    }

    auto eltit = hit->second;

    cache_.splice(cache_.begin(), cache_, eltit);

    return true;
}

} // namespace caches

#endif // LRUCACHE_H
