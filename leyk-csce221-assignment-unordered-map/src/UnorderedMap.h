#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"

//order to complete
//default constructor
//private helpers
//local/basic iterator
//insert/erase
//rule of 5


template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    size_type _bucket_count;
    HashNode **_buckets;

    HashNode * _head;
    size_type _size;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;
        HashNode * _ptr;

        explicit basic_iterator(UnorderedMap const * map, HashNode *ptr) noexcept : _map(map), _ptr(ptr) { /* TODO */ }

    public:
        basic_iterator(): _map(nullptr), _ptr(nullptr) {};

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;
        reference operator*() const {
            return _ptr->val;
        }
        pointer operator->() const {
            return &_ptr->val;
        }
        basic_iterator &operator++() {
            if (_ptr != nullptr) {
                auto bucket = _map->_bucket((*this)->first);
                _ptr = _ptr->next;
                if (_ptr == nullptr && _map != nullptr) {
                    for (auto i = bucket + 1; i < _map->_bucket_count; i++) {
                        if (_map->_buckets[i] != nullptr) {
                            _ptr = _map->_buckets[i];
                            break;
                        }
                    }
                }
            }
            return *this;
        }
        basic_iterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }
        bool operator==(const basic_iterator &other) const noexcept {
            return _ptr == other._ptr;
        }
        bool operator!=(const basic_iterator &other) const noexcept {
            return _ptr != other._ptr;
        }
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;
            
            explicit local_iterator( HashNode * node ) noexcept : _node(node) { /* TODO */ }

        public:
            local_iterator() : _node(nullptr) {
            }

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const {
                return _node->val;
            }
            pointer operator->() const {
                return &_node->val;
            }
            local_iterator & operator++() {
                _node = _node->next;
                return *this;
            }
            local_iterator operator++(int) {
                auto copy = *this;
                _node = _node->next;
                return copy;
            }

            bool operator==(const local_iterator &other) const noexcept {
                return _node == other._node;
            }
            bool operator!=(const local_iterator &other) const noexcept {
                return _node != other._node;
            }
    };

private:

    size_type _bucket(size_t code) const {
        return _range_hash(code, _bucket_count);
    }
    size_type _bucket(const Key & key) const {
        auto code = _hash(key);
        return _bucket(code);
    }
    size_type _bucket(const value_type & val) const {
        return _bucket(val.first);
    }

    HashNode*& _find(size_type bucket, const Key & key) {
        HashNode** curr = &_buckets[bucket];
        while (*curr != nullptr) {
            if (_equal(key, (*curr)->val.first)) {
                return *curr;
            }
            curr= &((*curr)->next);
        }
        return *curr;
    }

    HashNode*& _find(const Key & key) {
        auto bucket = _bucket(key);
        return _find(bucket, key);
    }

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) {
        HashNode* toAdd = new HashNode(std::move(value), _buckets[bucket]);
        _buckets[bucket] = toAdd;
        if (_head == nullptr || _bucket(_head->val.first) >= bucket) {
            _head = toAdd;
        }
        return toAdd;
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) {
        delete[] dst._buckets;
        dst._size = src._size;
        dst._head = src._head;
        dst._buckets = src._buckets;
        dst._bucket_count = src._bucket_count;
        src._size = 0;
        src._head = nullptr;
        src._buckets = new HashNode*[src._bucket_count]();
    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { },
                const key_equal & equal = key_equal { }) {
                    _bucket_count = next_greater_prime(bucket_count);
                    _hash = hash;
                    _equal = equal;
                    _size = 0;
                    _head = nullptr;
                    _buckets = new HashNode*[_bucket_count]();
                }

    ~UnorderedMap() {
        clear();
        delete[] _buckets;
    }

    UnorderedMap(const UnorderedMap & other) {
        _equal = other._equal;
        _hash = other._hash;
        _bucket_count = other._bucket_count;
        _head = nullptr;
        _size = 0;
        _buckets = new HashNode*[_bucket_count]();
        for (auto i = other.cbegin(); i != other.cend(); i++) {
            insert(*i);
        }

    }

    UnorderedMap(UnorderedMap && other) {
        _hash = std::move(other._hash);
        _equal = std::move(other._equal);
        _bucket_count = 0;
        _buckets = nullptr;
        _head = nullptr;
        _size = 0;
        _move_content(other, *this);
    }

    UnorderedMap & operator=(const UnorderedMap & other) {
        if (this != &other) {
            clear();
            delete[] _buckets;
            _hash = other._hash;
            _equal = other._equal;
            _bucket_count = other._bucket_count;
            _buckets = new HashNode*[_bucket_count]();
            _size = 0;
            _head = nullptr;
            for (auto i = other.cbegin(); i != other.cend(); i++) {
                insert(*i);
            }
        }
        return *this;
    }

    UnorderedMap & operator=(UnorderedMap && other) {
        if (this != &other) {
            clear();
            delete[] _buckets;
            _hash = std::move(other._hash);
            _equal = std::move(other._equal);
            _bucket_count = 0;
            _buckets = nullptr;
            _head = nullptr;
            _size = 0;
            _move_content(other, *this);
        }
        return *this;
    }

    void clear() noexcept { 
        for (auto i = 0; i < _bucket_count; i++) {
            HashNode* curr = _buckets[i];
            while (curr != nullptr) {
                HashNode* next = curr->next;
                delete curr;
                curr = next;
            }
            _buckets[i] = nullptr;
        }
        _head = nullptr;
        _size = 0;
    }

    size_type size() const noexcept {
        return _size;
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type bucket_count() const noexcept {
        return _bucket_count;
    }

    iterator begin() {
        return iterator(this, _head);
    }
    iterator end() {
        return iterator(this, nullptr);
    }

    const_iterator cbegin() const {
        return const_iterator(this, _head);
    };
    const_iterator cend() const {
        return const_iterator(this, nullptr);
    };

    local_iterator begin(size_type n) {
        return local_iterator(_buckets[n]);
    }
    local_iterator end(size_type n) {
        return local_iterator(nullptr);
    }

    size_type bucket_size(size_type n) {
        auto temp2 = 0;
        HashNode** curr = &_buckets[n];
        while (*curr != nullptr) {
            temp2++;
            curr= &((*curr)->next);
        }

        return temp2;
    }

    float load_factor() const {
        float x = size();
        float y = _bucket_count;
        return x / y;
    }

    size_type bucket(const Key & key) const {
        return _bucket(key);
    }

    std::pair<iterator, bool> insert(value_type && value) {
        auto bucket = _bucket(value.first);
        HashNode*& temp = _find(bucket, value.first);
        if (temp != nullptr) {
            return std::pair<iterator, bool>(iterator(this, temp), false);
        }
        HashNode* temp2 = _insert_into_bucket(bucket, std::move(value));
        _size++;
        return std::pair<iterator, bool>(iterator(this, temp2), true);
    }

    std::pair<iterator, bool> insert(const value_type & value) {
        std::pair<Key, T> temp0 = {value.first, value.second};
        auto bucket = _bucket(temp0.first);
        HashNode*& temp = _find(bucket, temp0.first);
        if (temp != nullptr) {
            return std::pair<iterator, bool>(iterator(this, temp), false);
        }
        HashNode* temp2 = _insert_into_bucket(bucket, temp0);
        _size++;
        return std::pair<iterator, bool>(iterator(this, temp2), true);
    }

    iterator find(const Key & key) {
        HashNode*& temp = _find(key);
        return iterator(this, temp);
    }

    T& operator[](const Key & key) {
        auto bucket = _bucket(key);
        HashNode*& curr = _find(bucket, key);
        if (curr != nullptr) {
            return curr->val.second;
        }
        HashNode* toAdd = _insert_into_bucket(bucket, {key, T()});
        _size++;
        return toAdd->val.second;
    }

    iterator erase(iterator pos) {
        auto next = pos;
        auto key = pos._ptr->val.first;
        auto bucket = _bucket(key);
        next++;
        if (_buckets[bucket] != pos._ptr) {
            HashNode* prev = _buckets[bucket];
            while (prev != nullptr && prev->next != pos._ptr) {
                prev = prev->next;
            }
            if (prev != nullptr && prev->next == pos._ptr) {
                prev->next = pos._ptr->next;
            }
        } 
        else {
            _buckets[bucket] = pos._ptr->next;
            if (_head == pos._ptr) {
                _head = next._ptr;
            }
        }
        _size--;
        delete pos._ptr;
        return next;
    }

    size_type erase(const Key & key) {
        HashNode* temp = _find(key);
        if (temp == nullptr) {
            return 0;
        }
        erase(iterator(this, temp));
        return 1;
    }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
