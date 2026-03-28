#include <iostream>
#include <cassert>
#include <thread>
#include "LruCache.hpp"

int fast_get_page_int(const int key) { return key; }

// slow get page imitation
int slow_get_page_int(const int key) {
    std::cout << "< ... Network request ...";
    std::cout.flush();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    std::cout << " Ok >" << std::endl;

    return key;
}

int main() {
    int hits = 0;
    int miss = 0;
    size_t size;
    size_t amount;

    std::cin >> size;
    std::cin >> amount;

    caches::LruCache<int, int> cache{size};

    for (size_t i = 0; i < amount; i++) {
        int page;
        std::cin >> page;
        if (cache.lookup_update(page, fast_get_page_int))
            hits++;
        else
            miss++;
    }

    std::cout << hits << std::endl;

    return 0;
}
