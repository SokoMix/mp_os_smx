#include <not_implemented.h>

#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(
    logger *logger)
{
    this->_logger = logger;
}

allocator_global_heap::~allocator_global_heap()
{}

allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
{
    if (other._logger != nullptr) {
        this->_logger = other._logger;
        other._logger = nullptr;
    }
}

allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    if (this != &other)
    {
        this->_logger = other._logger;
        other._logger = nullptr;
    }

    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(
    size_t value_size,
    size_t values_count)
{
    try {
        this->_logger->trace("Allocating memory");
        void * pointer = ::operator new(sizeof (allocator *) + sizeof(size_t) + value_size * values_count);
        size_t *size = reinterpret_cast<size_t *>(pointer);
        *size = value_size * values_count;
        allocator **p = reinterpret_cast<allocator **>(size + 1);
        *p = this;
        this->_logger->trace("Finished allocating memory");

        return reinterpret_cast<void *>(p + 1);
    } catch (std::bad_alloc const &e) {
        error_with_guard(e.what());
        throw;
    }
}

void allocator_global_heap::deallocate(
    void *at)
{
    void *block = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(at) - sizeof(allocator *) - sizeof(size_t));
    if (*reinterpret_cast<allocator **>(reinterpret_cast<unsigned char *>(block) + sizeof(size_t)) != this)
    {
        throw std::logic_error("I can't delete this");
    }
    size_t size = *reinterpret_cast<size_t *>(reinterpret_cast<unsigned char *>(at) - sizeof(size_t));
    std::string data_out;
    for (int i = 0; i < size; ++i) {
        data_out += *(reinterpret_cast<unsigned char *>(at) + i);
    }
    // void **pointer = &at;
    // pointer--;
    // void **p = pointer;
    // pointer--;
    // size_t *size = reinterpret_cast<size_t *>(pointer);
    ::operator delete(block);
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    return "allocator_global_heap";
}