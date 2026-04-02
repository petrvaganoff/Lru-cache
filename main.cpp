#include <iostream>
#include <string_view>
#include "LruCache.hpp"
#include "LfuCache.hpp"


int fast_get_page_int(const int key) { return key; }

template <typename Cache>
int process_requests(Cache& cache, size_t amount) {
    int hits_ = 0;

    for (size_t i = 0; i < amount; ++i) {
        int page;
        std::cin >> page;

        if (cache.lookup_update(page, fast_get_page_int))
            ++hits_;
    }

    return hits_;
}

int main(int argc, char* argv[]) {
    int hits = 0;
    size_t size;
    size_t amount;
    
    if (argc != 2) {
        std::cout << "Error: use with one argument.\n";
        std::cout << "Usage: cashes [--lru|--lfu]\n";
        return 1;
    }

    std::cin >> size >> amount;
    
    std::string_view mode{argv[1]};

    if (mode == "--lru") {
        caches::LruCache<int, int> cache{size};
        hits = process_requests(cache, amount);
    } else if (mode == "--lfu") {
        caches::LfuCache<int, int> cache{size};
        // TODO: вынести отдельно, использовать указатель на cache
        hits = process_requests(cache, amount);
    } else {
        std::cout << "Error: wrong argument.\n";
        return 1;
    }

    std::cout << hits << std::endl;

    return 0;
}
