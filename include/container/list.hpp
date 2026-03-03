#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <stdexcept>
#include <iostream>
#include "../memoryriver/memoryriver.hpp"

namespace sjtu {

template <typename T>
class list {
protected:
    struct node {
        T val_;
        node* nxt_;
        node* pre_;

        node(const T& val = T(), node* nxt = nullptr, node* pre = nullptr)
            : val_(val), nxt_(nxt), pre_(pre) {}
        node(T&& val, node* nxt = nullptr, node* pre = nullptr)
            : val_(std::move(val)), nxt_(nxt), pre_(pre) {}
    };

    size_t size_;
    node* head_;
    node* tail_;

public:
    class iterator {
        friend class list;
    private:
        node* ptr_;
    public:
        explicit iterator(node* ptr = nullptr) : ptr_(ptr) {}

        node* get_ptr() const { return ptr_; }

        iterator& operator++() { ptr_ = ptr_->nxt_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }
        iterator& operator--() { ptr_ = ptr_->pre_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }

        T& operator*() const { return ptr_->val_; }
        T* operator->() const { return &(ptr_->val_); }

        bool operator==(const iterator& rhs) const { return ptr_ == rhs.ptr_; }
        bool operator!=(const iterator& rhs) const { return ptr_ != rhs.ptr_; }
    };

    class const_iterator {
        friend class list;
    private:
        const node* ptr_;
    public:
        explicit const_iterator(const node* ptr = nullptr) : ptr_(ptr) {}

        const_iterator& operator++() { ptr_ = ptr_->nxt_; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }
        const_iterator& operator--() { ptr_ = ptr_->pre_; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --*this; return tmp; }

        const T& operator*() const { return ptr_->val_; }
        const T* operator->() const { return &(ptr_->val_); }

        bool operator==(const const_iterator& rhs) const { return ptr_ == rhs.ptr_; }
        bool operator!=(const const_iterator& rhs) const { return ptr_ != rhs.ptr_; }
    };

    list() : size_(0) {
        head_ = new node();
        tail_ = new node();
        head_->nxt_ = tail_;
        tail_->pre_ = head_;
    }

    ~list() {
        clear();
        delete head_;
        delete tail_;
    }
    list(const list&) = delete;
    list& operator=(const list&) = delete;

    iterator insert(iterator pos, const T& value) {
        if (pos.get_ptr() == nullptr) {
            throw std::invalid_argument("invalid position");
        }
        node* new_node = new node(value, pos.get_ptr(), pos.get_ptr()->pre_);
        pos.get_ptr()->pre_->nxt_ = new_node;
        pos.get_ptr()->pre_ = new_node;
        ++size_;
        return iterator(new_node);
    }

    iterator erase(iterator pos) {
        if (pos.get_ptr() == nullptr || pos.get_ptr() == head_ || pos.get_ptr() == tail_) {
            throw std::invalid_argument("invalid erase position");
        }
        node* to_del = pos.get_ptr();
        node* ret = to_del->nxt_;
        to_del->pre_->nxt_ = to_del->nxt_;
        to_del->nxt_->pre_ = to_del->pre_;
        delete to_del;
        --size_;
        return iterator(ret);
    }

    void push_front(const T& value) {
        insert(iterator(head_->nxt_), value);
    }

    void push_back(const T& value) {
        insert(iterator(tail_), value);
    }

    void pop_back() {
        if (empty()) throw std::out_of_range("pop_back on empty list");
        erase(iterator(tail_->pre_));
    }

    bool empty() const { return size_ == 0; }

    size_t size() const { return size_; }

    void clear() {
        while (!empty()) {
            erase(iterator(head_->nxt_));
        }
    }

    T& front() {
        if (empty()) throw std::out_of_range("front on empty list");
        return head_->nxt_->val_;
    }
    const T& front() const {
        if (empty()) throw std::out_of_range("front on empty list");
        return head_->nxt_->val_;
    }

    T& back() {
        if (empty()) throw std::out_of_range("back on empty list");
        return tail_->pre_->val_;
    }
    const T& back() const {
        if (empty()) throw std::out_of_range("back on empty list");
        return tail_->pre_->val_;
    }

    iterator begin() { return iterator(head_->nxt_); }
    iterator end() { return iterator(tail_); }

    const_iterator cbegin() const { return const_iterator(head_->nxt_); }
    const_iterator cend() const { return const_iterator(tail_); }

    // serialization helpers (length plus element sequence)
    void serialize(std::ostream &os) const {
        size_t sz = size_;
        os.write(reinterpret_cast<const char *>(&sz), sizeof(sz));
        for (auto it = cbegin(); it != cend(); ++it)
            sjtu::Serializer<T>::write(os, *it);
    }
    void deserialize(std::istream &is) {
        size_t sz;
        is.read(reinterpret_cast<char *>(&sz), sizeof(sz));
        clear();
        for (size_t i = 0; i < sz; ++i) {
            T tmp;
            sjtu::Serializer<T>::read(is, tmp);
            push_back(tmp);
        }
    }
};

}

#endif