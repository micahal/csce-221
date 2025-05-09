#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm> // std::random_access_iterator_tag
#include <cstddef> // size_t
#include <stdexcept> // std::out_of_range
#include <type_traits> // std::is_same

template <class T>
class Vector {
public:
    class iterator;
private:
    T* array;
    size_t _capacity, _size;

    // You may want to write a function that grows the vector
    void grow() {
        if (array == nullptr) {
            _capacity = 1;
            T* array2 = new T[_capacity];
            delete[] array; 
            array = array2;
        }
        else {
            _capacity = _capacity * 2;
            T* array2 = new T[_capacity];
            for (size_t i = 0; i < _size; i++) {
                array2[i] = std::move(array[i]);
            }
            delete[] array; 
            array = array2;
        }
    }
    

public:
    Vector() noexcept : array(nullptr), _capacity(0), _size(0) { /* TODO */ }
    Vector(size_t count, const T& value) : _capacity(count), _size(count), array(new T[count]) {
        for (auto i = 0; i < count; i++) {
            array[i] = value;
        }
    }
    explicit Vector(size_t count) : _size(count), _capacity(count), array(new T[count]()) { 
    }
    Vector(const Vector& other) : array(new T[other._capacity]), _capacity(other._capacity), _size(other._size) {
        for (size_t i = 0; i < _size; i++) {
            array[i] = other.array[i];
        }
    }
    Vector(Vector&& other) noexcept : array(other.array), _capacity(other._capacity), _size(other._size) { 
        other._size = 0;
        other._capacity = 0;
        other.array = nullptr;
     }

    ~Vector() {
        delete[] array;
        array = nullptr;
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        delete[] array;
        array = new T[other._capacity];
        _size = other._size;
        _capacity = other._capacity;
        for (auto i = 0; i < _size; i++) {
            array[i] = other.array[i];
        }
        return *this;
     }
    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] array;
        array = std::move(other.array);
        _size = other._size;
        _capacity = other._capacity;
        other.array = nullptr;
        other._size = 0;
        other._capacity = 0;
        return *this;
    }

    iterator begin() noexcept {
        return iterator(array);       
    }
    iterator end() noexcept {
        return iterator(array + _size);
    }

    [[nodiscard]] bool empty() const noexcept {
        if (_size == 0) {
            return true;
        }
        return false;
    }
    size_t size() const noexcept {
        return _size;
    }
    size_t capacity() const noexcept {
        return _capacity;
    }

    T& at(size_t pos) {
        if (pos < 0 || pos >= _size) {
            throw std::out_of_range("error");
        }
        return array[pos];
    }
    const T& at(size_t pos) const {
        if (pos < 0 || pos >= _size) {
            throw std::out_of_range("error");
        }
        return array[pos];
    }
    T& operator[](size_t pos) {
        return array[pos];
    }
    const T& operator[](size_t pos) const {
        return array[pos];
     }
    T& front() {
        return array[0];
    }
    const T& front() const {
        return array[0];
    }
    T& back() {
        return array[_size - 1];
    }
    const T& back() const {
        return array[_size - 1];
    }
    
    void push_back(const T& value) {
        insert(end(), value);
    }
    void push_back(T&& value) {
        insert(end(), std::move(value));
    }
    void pop_back() {
        _size--;
    }

    iterator insert(iterator pos, const T& value) {
        if (_size >= _capacity) {
            auto diff = pos - begin();
            grow();
            pos = diff + begin();
        }
        for (auto i = end(); i > pos; i--) {
            *i = std::move(*(i - 1));
        }
        *pos = value;
        _size++;
        return pos;
        
    }
    iterator insert(iterator pos, T&& value) {
        if (_size >= _capacity) {
            auto diff = pos - begin();
            grow();
            pos = diff + begin();
        }
        for (auto i = end(); i > pos; i--) {
            *i = std::move(*(i - 1));
        }
        *pos = std::move(value);
        _size++;
        return pos;
    }
    iterator insert(iterator pos, size_t count, const T& value) {
        while (_size + count >= _capacity) {
            auto diff = pos - begin();
            grow();
            pos = diff + begin();
        }
        for (auto i = end() - 1; i >= pos; i--) {
            *(i + count) = std::move(*i);
        }
        for (auto i = pos; i != (pos + count); i++) {
            *i = value;
        }
        _size = _size + count;
        return pos;

    }
    iterator erase(iterator pos) {
        for (auto i = pos + 1; i < end(); i++) {
            *(i - 1) = std::move(*i);
        }
        _size--;
        return pos;
    }
    iterator erase(iterator first, iterator last) {
        auto diff = last - first;
        for (auto i = last; i < end(); i++) {
            *(i - diff) = std::move(*i);
        }
        _size = _size - diff;
        return first;
    }

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    private:
        // Points to some element in the vector (or nullptr)
        T* _ptr;

    public:
        iterator() : _ptr(nullptr){ /* TODO */ }
        explicit iterator(T* ptr) {
            _ptr = ptr;
        }

        // This assignment operator is done for you, please do not add more
        iterator& operator=(const iterator&) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept { 
            return *_ptr;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            return _ptr;
        }

        // Prefix Increment: ++a
        iterator& operator++() noexcept {
            _ptr = _ptr + 1;
            return *this;
        }
        // Postfix Increment: a++
        iterator operator++(int) noexcept { 
            iterator temp = *this;
            _ptr = _ptr + 1;
            return temp;
        }
        // Prefix Decrement: --a
        iterator& operator--() noexcept {
            _ptr = _ptr - 1;
            return *this;
        }
        // Postfix Decrement: a--
        iterator operator--(int) noexcept {
            iterator temp = *this;
            _ptr = _ptr - 1;
            return temp;
        }

        iterator& operator+=(difference_type offset) noexcept {
            _ptr = _ptr + offset;
            return *this;
        }
        [[nodiscard]] iterator operator+(difference_type offset) const noexcept { 
            iterator temp = *this;
            temp += offset;
            return temp;
        }
        
        iterator& operator-=(difference_type offset) noexcept {
            _ptr = _ptr - offset;
            return *this;
        }
        [[nodiscard]] iterator operator-(difference_type offset) const noexcept {
            iterator temp = *this;
            temp -= offset;
            return temp;
        }
        [[nodiscard]] difference_type operator-(const iterator& rhs) const noexcept {
            return _ptr - rhs._ptr;
        }

        [[nodiscard]] reference operator[](difference_type offset) const noexcept {
            return *(_ptr + offset);
        }

        [[nodiscard]] bool operator==(const iterator& rhs) const noexcept {
            if (rhs._ptr == _ptr) {
                return true;
            }
            return false;
        }
        [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept { 
            if (rhs._ptr != _ptr) {
                return true;
            }
            return false;
        }
        [[nodiscard]] bool operator<(const iterator& rhs) const noexcept {
            if (_ptr < rhs._ptr) {
                return true;
            }
            return false;
        }
        [[nodiscard]] bool operator>(const iterator& rhs) const noexcept {
            if (_ptr > rhs._ptr) {
                return true;
            }
            return false;
        }
        [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept {
            if (_ptr <= rhs._ptr) {
                return true;
            }
            return false;
        }
        [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept {
            if (_ptr >= rhs._ptr) {
                return true;
            }
            return false;
        }
    };


    void clear() noexcept {
        delete[] array;
        array = nullptr;
        _size = 0;
    }
};

// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace {
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const& iterator) noexcept {
    return iterator + offset; 
}

#endif
