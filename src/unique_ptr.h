#pragma once

#include <types.h>
#include <alloc.h>
#include <core_traits.h>

namespace core {

template <typename T, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct unique_ptr {
    using data_type = T;
    using uptr_type = unique_ptr<T, TAllocator>;

    unique_ptr() : _ptr(nullptr) {}
    unique_ptr(data_type* newPtr) : _ptr(newPtr) {}
    unique_ptr(uptr_type&& other) {
        _ptr = other._ptr;
        other._ptr = nullptr;
    }
    uptr_type& operator=(uptr_type&& other) {
        if (this == &other) return *this;
        reset();
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    // no copy 
    unique_ptr(const uptr_type&) = delete;
    unique_ptr& operator=(const uptr_type&) = delete;
    
    ~unique_ptr() { reset(); }

    uptr_type copy() const {
        uptr_type ret;
        ret._ptr = construct<TAllocator, data_type>(*_ptr);
        return ret;
    }

    void reset(data_type* newPtr = nullptr) {
        if (_ptr) {
            if constexpr (!core::is_trivially_destructible_v<data_type>) {
                _ptr->~T();
            }
            TAllocator::free(_ptr);
        }
        _ptr = newPtr;
    }

    data_type* steal() {
        data_type* ret = _ptr;
        _ptr = nullptr;
        return ret;
    }

    data_type* operator->() const { return _ptr; }
    data_type& operator*() const { return *_ptr; }

    data_type* get() const { return _ptr; }

    explicit operator bool() const { return _ptr != nullptr; }

    data_type& operator[](addr_size idx) const { return _ptr[idx]; }

private:
    data_type* _ptr;
};

template <typename T, typename TAllocator = CORE_DEFAULT_ALLOCATOR(), typename... Args>
unique_ptr<T, TAllocator> make_unique(Args&&... args) {
    T* rawPtr = construct<TAllocator, T>(args...);
    unique_ptr<T, TAllocator> ret (rawPtr);
    return ret;
}


} // namespace core
