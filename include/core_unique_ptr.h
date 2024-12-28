#pragma once

#include <core_exec_ctx.h>
#include <core_macros.h>
#include <core_types.h>

namespace core {

template <typename T, AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID> struct UniquePtr;
template <typename T, AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID, typename... Args> UniquePtr<T, TAllocId> makeUnique(Args&&... args);

template <typename T, AllocatorId TAllocId>
struct UniquePtr {
    inline static core::AllocatorContext& allocator = core::getAllocator(TAllocId);

    using value_type = T;

    NO_COPY(UniquePtr);

    UniquePtr() : m_ptr(nullptr) {}
    UniquePtr(value_type* newPtr) : m_ptr(newPtr) {}
    UniquePtr(UniquePtr&& other) {
        if (this == &other) return;
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if (this == &other) return *this;
        reset();
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
        return *this;
    }

    ~UniquePtr() { reset(); }

    UniquePtr copy() const {
        UniquePtr ret;
        ret.m_ptr = allocator.construct<value_type>(*m_ptr);
        return ret;
    }

    void reset(value_type* newPtr = nullptr) {
        if (m_ptr) {
            m_ptr->~T();
            allocator.free(m_ptr, 1, sizeof(value_type));
        }
        m_ptr = newPtr;
    }

    value_type* release() {
        value_type* ret = m_ptr;
        m_ptr = nullptr;
        return ret;
    }

    void swap(UniquePtr& other) {
        value_type* tmp = m_ptr;
        m_ptr = other.m_ptr;
        other.m_ptr = tmp;
    }

    value_type* operator->() const { return m_ptr; }
    value_type& operator*() const { return *m_ptr; }

    value_type* get() const { return m_ptr; }

    explicit operator bool() const { return m_ptr != nullptr; }

    value_type& operator[](addr_size idx) const { return m_ptr[idx]; }

private:
    value_type* m_ptr;
};

template <typename T, AllocatorId TAllocId, typename... Args>
UniquePtr<T, TAllocId> makeUnique(Args&&... args) {
    T* rawPtr = getAllocator(TAllocId).construct<T>(args...);
    UniquePtr<T, TAllocId> ret (rawPtr);
    return ret;
}

} // namespace core
