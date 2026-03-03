#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP
#pragma GCC("Ofast")
#include <cstddef>
#include <cstring>
#include <utility>
#include <iostream>

#include "exceptions.hpp"
#include "../memoryriver/memoryriver.hpp"


namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
   public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a
        // type. For instance, for an iterator, iterator::value_type is the type
        // that the iterator points to. STL algorithms and containers may use
        // these type_traits (e.g. the following typedef) to work properly. In
        // particular, without the following code,
        // @code{std::sort(iter, iter1);} would not compile.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type:
        // https://blog.csdn.net/u014299153/article/details/72419713 About
        // iterator_category: https://en.cppreference.com/w/cpp/iterator
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::output_iterator_tag;
        friend class const_iterator;
        friend class vector;

       private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        T* ptr_;
        T* begin_;

       public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator() = delete;
        iterator(T* begin_, T* ptr_) : ptr_(ptr_), begin_(begin_) {
        }
        iterator(const_iterator& rhs) : ptr_(rhs.ptr_), begin_(rhs.begin_) {
        }
        iterator operator+(const int& n) const {
            // TODO
            return iterator(begin_, ptr_ + n);
        }
        iterator operator-(const int& n) const {
            return iterator(begin_, ptr_ - n);
            // TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const iterator& rhs) const {
            if (rhs.begin_ != begin_) throw invalid_iterator();
            return ptr_ - rhs.ptr_;
            // TODO
        }
        iterator& operator+=(const int& n) {
            ptr_ = ptr_ + n;
            return *this;
            // TODO
        }
        iterator& operator-=(const int& n) {
            ptr_ = ptr_ - n;
            return *this;
            // TODO
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            T* tmp = ptr_;
            ++ptr_;
            return iterator(begin_, tmp);
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            ++ptr_;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            T* tmp = ptr_;
            --ptr_;
            return iterator(begin_, tmp);
        }
        /**
         *
         * TODO --iter
         */
        iterator& operator--() {
            --ptr_;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const {
            return *ptr_;
        }
        T* operator ->() const {
            return ptr_;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator& rhs) const {
            return (begin_ == rhs.begin_) && (ptr_ == rhs.ptr_);
        }
        bool operator==(const const_iterator& rhs) const {
            return (begin_ == rhs.begin_) && (ptr_ == rhs.ptr_);
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator& rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::output_iterator_tag;
        friend class iterator;
        friend class vector;

       private:
        const T* begin_;
        const T* ptr_;

       public:
        /*TODO*/
        const_iterator() = delete;
        const_iterator(const T* begin_, const T* ptr_)
            : ptr_(ptr_), begin_(begin_) {
        }
        const_iterator(const iterator& rhs)
            : ptr_(rhs.ptr_), begin_(rhs.begin_) {
        }
        const_iterator operator+(const int& n) const {
            // TODO
            return const_iterator(begin_, ptr_ + n);
        }
        const_iterator operator-(const int& n) const {
            return const_iterator(begin_, ptr_ - n);
            // TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const iterator& rhs) const {
            if (rhs.begin_ != begin_) throw invalid_iterator();
            return ptr_ - rhs.ptr_;
            // TODO
        }

        const_iterator& operator+=(const int& n) {
            ptr_ = ptr_ + n;
            return *this;
            // TODO
        }
        const_iterator& operator-=(const int& n) {
            ptr_ = ptr_ - n;
            return *this;
            // TODO
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const T* tmp = ptr_;
            ++ptr_;
            return const_iterator(begin_, tmp);
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const T* tmp = ptr_;
            --ptr_;
            return const_iterator(begin_, tmp);
        }
        /**
         *
         * TODO --iter
         */
        const_iterator& operator--() {
            --ptr_;
            return *this;
        }
        /**
         * TODO *it
         */
        const T& operator*() const {
            return *ptr_;
        }
        const T* operator ->() const {
            return ptr_;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator& rhs) const {
            return (begin_ == rhs.begin_) && (ptr_ == rhs.ptr_);
        }
        bool operator==(const const_iterator& rhs) const {
            return (begin_ == rhs.begin_) && (ptr_ == rhs.ptr_);
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator& rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO Constructs
     * At least two: default constructor, copy constructor
     */
    vector() : data(nullptr), capacity(0), size_(0) {
    }
    vector(const vector& other) : capacity(other.capacity), size_(other.size_) {
        data = static_cast<T*>(operator new(capacity * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) new (&data[i]) T(other.data[i]);
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for (size_t i = 0; i < size_; ++i) data[i].~T();
        operator delete(data);
    }
    /**
     * TODO Assignment operator
     */
    vector& operator=(const vector& other) {
        if (this == &other) return *this;
        if (capacity >= other.size_) {
            size_t min_size = (size_ < other.size_) ? size_ : other.size_;
            for (size_t i = 0; i < min_size; ++i) data[i] = other.data[i];
            for (size_t i = size_; i < other.size_; ++i)
                new (&data[i]) T(other.data[i]);
            for (size_t i = other.size_; i < size_; ++i) data[i].~T();

            size_ = other.size_;
            return *this;
        }
        for (size_t i = 0; i < size_; ++i) data[i].~T();
        operator delete(data);
        capacity = other.capacity;
        size_ = other.size_;
        data = static_cast<T*>(operator new(capacity * sizeof(T)));
        if constexpr (std::is_trivially_copyable<T>::value) {
            std::memcpy(data, other.data, size_ * sizeof(T));
        } else {
            for (size_t i = 0; i < size_; ++i) new (&data[i]) T(other.data[i]);
        }

        return *this;
    }
    vector(vector&& other) noexcept
        : data(other.data), size_(other.size_), capacity(other.capacity) {
        other.data = nullptr;
        other.size_ = other.capacity = 0;
    }
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            // 直接交换所有权，原有的 data 会随 other 的销毁而自动释放
            std::swap(data, other.data);
            std::swap(size_, other.size_);
            std::swap(capacity, other.capacity);
        }
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size_)
     */
    T& at(const size_t& pos) {
        if (pos >= size_) 
        {
            std::cerr<<"at pos "<<pos<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        return data[pos];
    }
    const T& at(const size_t& pos) const {
        if (pos >= size_) 
        {
             std::cerr<<"at pos "<<pos<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        return data[pos];
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size_)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T& operator[](const size_t& pos) {
        if (pos >= size_)
        {
            std::cerr<<"operator[] pos "<<pos<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        } 
        return data[pos];
    }
    const T& operator[](const size_t& pos) const {
        if (pos >= size_) {
             std::cerr<<"operator[] pos "<<pos<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        return data[pos];
    }

    // serialization support (size + elements)
    void serialize(std::ostream &os) const {
        size_t sz = size_;
        os.write(reinterpret_cast<const char *>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
            sjtu::Serializer<T>::write(os, data[i]);
    }
    void deserialize(std::istream &is) {
        size_t sz;
        is.read(reinterpret_cast<char *>(&sz), sizeof(sz));
        // sanity check: prevent allocating unreasonable amounts of memory
        if (sz > 10000000) {
            throw std::bad_alloc();  // size too large, likely corrupted data
        }
        // clear and reset to desired capacity
        clear();
        // reserve space without constructing objects
        for (size_t i = 0; i < sz; ++i) {
            T tmp;
            sjtu::Serializer<T>::read(is, tmp);
            push_back(tmp);
        }
    }

    /**
     * access the first element.
     * throw container_is_empty if size_ == 0
     */
    const T& front() const {
        if (size_ == 0) throw container_is_empty();
        return data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size_ == 0
     */
    const T& back() const {
        if (size_ == 0) throw container_is_empty();
        return data[size_ - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(data, data);
    }
    const_iterator begin() const {
        return const_iterator(data, data);
    }
    const_iterator cbegin() const {
        return const_iterator(data, data);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(data, data + size_);
    }
    const_iterator end() const {
        return const_iterator(data, data + size_);
    }
    const_iterator cend() const {
        return const_iterator(data, data + size_);
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
        return size_ == 0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return size_;
    }
    /**
     * clears the contents
     */
    void clear() {
        for (size_t i = 0; i < size_; ++i) data[i].~T();
        operator delete(data);
        size_ = capacity = 0;
        data = nullptr;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T& value) {
        return insert(pos.ptr_ - pos.begin_, value);
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size_ (in this situation ind can be
     * size_ because after inserting the size_ will increase 1.)
     */
    iterator insert(const size_t& ind, const T& value) {
        if (ind > size_) 
        {
            std::cerr<<"insert ind "<<ind<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        if (size_ == capacity) expand();
        if (size_ > ind) {
            new (&data[size_]) T(std::move(data[size_ - 1]));
            for (size_t i = size_ - 1; i > ind; i--) {
                data[i] = std::move(data[i - 1]);
            }
            data[ind] = std::move(value);
        } else
            new (&data[size_]) T(std::move(value));
        ++size_;
        return iterator(data, data + ind);
    }
    iterator insert(iterator pos, T&& value) {
        return insert(pos.ptr_ - pos.begin_, std::move(value));
    }
    iterator insert(const size_t& ind, T&& value) {
        if (ind > size_) 
        {
            std::cerr<<"insert ind "<<ind<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        if (size_ == capacity) expand();

        if (size_ > ind) {
            new (&data[size_]) T(std::move(data[size_ - 1]));
            for (size_t i = size_ - 1; i > ind; --i) {
                data[i] = std::move(data[i - 1]);
            }
            data[ind] = std::move(value);
        } else {
            new (&data[size_]) T(std::move(value));
        }
        ++size_;
        return iterator(data, data + ind);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is
     * returned.
     */
    iterator erase(iterator pos) {
        return erase(pos.ptr_ - pos.begin_);
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size_
     */
    iterator erase(const size_t& ind) {
        if (ind >= size_) 
        {
            std::cerr<<"erase ind "<<ind<<" size "<<size_<<"\n";
            throw index_out_of_bound();
        }
        for (size_t i = ind; i < size_ - 1; ++i) {
            data[i] = std::move(data[i + 1]);
        }
        data[size_ - 1].~T();
        --size_;
        return iterator(data, data + ind);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T& value) {
        if (size_ == capacity) expand();
        new (&data[size_++]) T(value);
    }
    void push_back(T&& value) {
        if (size_ == capacity) expand();
        new (&data[size_++]) T(std::move(value));
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size_() == 0
     */
    void pop_back() {
        if (size_ == 0) throw container_is_empty();
        data[--size_].~T();
    }

#include <cstring>      // 必须包含 memcpy
#include <type_traits>  // 必须包含

    void expand() {
        if (capacity == 0)
            capacity = 1;
        else
            capacity *= 2;

        T* newdata = static_cast<T*>(operator new(capacity * sizeof(T)));

        if constexpr (std::is_trivially_copyable<T>::value) {
            if (size_ > 0) {
                std::memcpy(newdata, data, size_ * sizeof(T));
            }
        } else {
            for (size_t i = 0; i < size_; ++i) {
                new (&newdata[i]) T(std::move(data[i]));
                data[i].~T();
            }
        }

        operator delete(data);
        data = newdata;
    }
    void sort(bool (*compare)( T&, T&));

   private:
    size_t size_, capacity;
    T* data;
};


template <typename T>
void quick_sort(T* arr, int low, int high, bool (*compare)(T&,  T&)) {
    if (low < high) {
        // Partitioning index
        int pi = partition(arr, low, high, compare);
        
        // Recursively sort the two halves
        quick_sort(arr, low, pi - 1, compare);
        quick_sort(arr, pi + 1, high, compare);
    }
}

// Partition function for QuickSort
template <typename T>
int partition(T* arr, int low, int high, bool (*compare)( T&,  T&)) {
    T pivot = arr[high];  // Choosing the last element as pivot
    int i = (low - 1);  // Index of the smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than the pivot
        if (compare(arr[j], pivot)) {
            i++;  // Increment index of smaller element
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Main sort function
template <typename T>
void vector<T>::sort(bool (*compare)( T&,  T&)) {
    if (size_ > 1) {
        quick_sort(data, 0, size_ - 1, compare);
    }
}

}  // namespace sjtu

#endif
