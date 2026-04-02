#ifndef LFUCACHE
#define LFUCACHE

#include <cstddef>
#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>

namespace caches {

template <typename T, typename KeyT>
class LfuCache
{
private:
    // ёмкость кэша
    size_t capacity_;

    // один элемент кэша
    struct Node {
        // данные кэша
        T value;
        // частота
        size_t freq;
        // итератор на элемент списка внутри freq_map_
        // нужен чтобы не искать элемент внутри списка, а сразу удалять за O(1)
        typename std::list<KeyT>::iterator position_;
    };

    // хеш таблица с элементами кэша
    std::unordered_map<KeyT, Node> cache_;

    // хеш таблица списков ключей элементов кэша с одинаковыми частотами.
    // внутри одного списка хранятся ключи в порядке давности добавления
    // недавно добавленный стоит последним.
    std::unordered_map<size_t, std::list<KeyT>> freq_map_;

    // минимальная частота среди всех элементов кэша
    size_t min_freq_;

    // alias для итератора по cache_
    using CacheIt = typename std::unordered_map<KeyT, Node>::iterator;

    // повышение частоты уже существующего элемента кэша
    void touch(CacheIt it);

    // вытесняет самый старый элемент с минимальной частотой
    void evict();

public:
    // explicit запрещает неявное создание кэша из числа (LfuCache cache = 1;)
    explicit LfuCache(size_t sz = 10)
        : capacity_(sz), min_freq_(0) {}

    bool full() const {
        return (cache_.size() == capacity_);
    }

    template <typename F>
    bool lookup_update(const KeyT& key, F&& get_page);
};

// F&& — forwarding reference: позволяет принимать любой callable (lvalue/rvalue)
// без лишнего копирования и корректно пробрасывать его через std::forward
template <typename T, typename KeyT>
template <typename F>
bool LfuCache<T, KeyT>::lookup_update(const KeyT& key, F&& get_page) {

    // если емкость нулевая, ничего не кэшируем
    if (capacity_ == 0) {
        return false;
    }

    // поиск элемента в кэше
    auto hit = cache_.find(key);

    // если элемент найден в кэше
    if (hit != cache_.end()) {
        touch(hit);
        return true;
    }

    // если кэш полон вытесняем элемент
    if (full()) {
        evict();
    }

    // берем список ключей с частотой 1
    // если его нет - создаем
    auto& list = freq_map_[1];
    // добавляем новый элемент в список
    list.push_back(key);

    // вставка нового элемента в кэш
    cache_.emplace(
        key,
        Node{
            // вызываем get_page(key), сохраняя исходную категорию значения callable через std::forward
            std::forward<F>(get_page)(key),
            1,
            std::prev(list.end())
        }
    );

    // новый элемент всегда вставляется с минимальной частотой 1
    min_freq_ = 1;

    return false;

}

// Повышает частоту элемента: переносит ключ из списка старой частоты
// в список новой и при необходимости обновляет min_freq_.
template <typename T, typename KeyT>
void LfuCache<T, KeyT>::touch(CacheIt it) {

    // сохраняем старую частоту элемента
    size_t old_freq = it->second.freq;

    // находим список, со старой частотой
    auto freq_it = freq_map_.find(old_freq);

    // удаляем ключ из списка старой частоты точно по итератору
    freq_it->second.erase(it->second.position_);

    // проверка не стал ли список этой частоты пустым после удаления
    if (freq_it->second.empty()) {
        // удаляем запись этой частоты из freq_map_
        freq_map_.erase(freq_it);

        // если это была минимальная частота и список опустел,
        // минимальная частота увеличивается на 1
        if (min_freq_ == old_freq) {
            ++min_freq_;
        }
    }

    // обновление частоты элемента кэша
    ++it->second.freq;

    // берем  список ключей для новой частоты
    // либо создаем пустой через оператор []
    auto& new_list = freq_map_[it->second.freq];

    // добавляем ключ в конец списка новой частоты
    new_list.push_back(it->first);

    // обновление position_ элемента кэша
    it->second.position_ = std::prev(new_list.end());
}

template <typename T, typename KeyT>
void LfuCache<T, KeyT>::evict() {

    // находим список ключей для минимальной частоты
    auto freq_it = freq_map_.find(min_freq_);

    // если не нашли - выходим
    if (freq_it == freq_map_.end()) {
        return;
    }

    // удаляем самый старый элемент с такой частотой
    const KeyT victim_key = freq_it->second.front();
    freq_it->second.pop_front();

    // если список стал пустым - удаляем
    if (freq_it->second.empty()) {
        freq_map_.erase(freq_it);
    }

    // удаляем элемент из кэша
    cache_.erase(victim_key);
}


} // namespace caches

#endif
