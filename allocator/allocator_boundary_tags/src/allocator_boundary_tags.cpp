#include <not_implemented.h>
#include <mutex>

#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags()
{
    throw not_implemented("allocator_boundary_tags::~allocator_boundary_tags()", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
        allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
        allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
        size_t space_size,
        allocator *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size < get_occupied_block_metadata_size())
    {
        throw std::logic_error("Too low memory");
    }

    space_size += get_allocator_metadata_size();

    try
    {
        _trusted_memory = parent_allocator == nullptr
                          ? ::operator new(space_size)
                          : parent_allocator->allocate(space_size, 1);
    }
    catch (std::bad_alloc const &ex)
    {
        // TODO: watch task!
        throw;
    }

    allocator **parent_allocator_space = reinterpret_cast<allocator **>(_trusted_memory);
    *parent_allocator_space = parent_allocator;

    class logger **logger_space = reinterpret_cast<class logger **>(parent_allocator_space + 1);
    *logger_space = logger;

    size_t *space_size_space = reinterpret_cast<size_t *>(logger_space + 1);
    *space_size_space = space_size;

    std::mutex *sync_object_space = reinterpret_cast<std::mutex *>(space_size_space + 1);
    //new (sync_object_space) std::mutex();
    allocator::construct(sync_object_space);

    allocator_with_fit_mode::fit_mode *fit_mode_space = reinterpret_cast<allocator_with_fit_mode::fit_mode *>(sync_object_space + 1);
    *fit_mode_space = allocate_fit_mode;

    void **first_block_address_space = reinterpret_cast<void **>(fit_mode_space + 1);
    *first_block_address_space = nullptr;

    // TODO: logs...
}

[[nodiscard]] void *allocator_boundary_tags::allocate(
        size_t value_size,
        size_t values_count)
{
    // TODO: std::unique_lock
    std::lock_guard<std::mutex> x(get_sync_object());
    /// block_size, previous_pointer, next_pointer, parent_pointer
    void *child_allocator = ::operator new(sizeof(size_t *) + 2 * sizeof(allocator **) + sizeof(allocator **) + value_size * values_count);

    size_t *size = reinterpret_cast<size_t *>(child_allocator);
    *size = values_count * values_count;

    allocator** parent_allocator = reinterpret_cast<allocator **>(size + 1);
    *parent_allocator = this;
//    allocator** next_pointer = reinterpret_cast<allocator **>(parent_allocator - 1);
//    next_pointer = nullptr;
    allocator** previous_pointer = reinterpret_cast<allocator **>(reinterpret_cast<size_t>(parent_allocator + 1));
    for (void **p = reinterpret_cast<void **>(this->get_first_occupied_block_address()); reinterpret_cast<allocator **>(reinterpret_cast<size_t *>(p) + 1) + 1 !=
            nullptr; ++p) {
        allocator ** aloc = reinterpret_cast<allocator **>(p);
        *aloc = reinterpret_cast<allocator *>(parent_allocator) + 1;
        
    }
}

void allocator_boundary_tags::deallocate(
        void *at)
{
    throw not_implemented("void allocator_boundary_tags::deallocate(void *)", "your code should be here...");
}

inline void allocator_boundary_tags::set_fit_mode(
        allocator_with_fit_mode::fit_mode mode)
{
    throw not_implemented("inline void allocator_boundary_tags::set_fit_mode(allocator_with_fit_mode::fit_mode)", "your code should be here...");
}

inline allocator *allocator_boundary_tags::get_allocator() const
{
    throw not_implemented("inline allocator *allocator_boundary_tags::get_allocator() const", "your code should be here...");
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept", "your code should be here...");
}

inline logger *allocator_boundary_tags::get_logger() const
{
    return *reinterpret_cast<class logger **>(reinterpret_cast<allocator **>(_trusted_memory) + 1);

    // return *reinterpret_cast<class logger **>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator *));
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boudary_tags";
}

size_t allocator_boundary_tags::get_occupied_block_metadata_size()
{
    return sizeof(size_t) + sizeof(void *) * 3;
}

size_t allocator_boundary_tags::get_allocator_metadata_size()
{
    return sizeof(allocator *) + sizeof(logger *) + sizeof(size_t) + sizeof(void *) + sizeof(std::mutex) + sizeof(allocator_with_fit_mode::fit_mode);
}

inline std::mutex& allocator_boundary_tags::get_sync_object()
{
    return *reinterpret_cast<std::mutex *>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator *) + sizeof(logger *) + sizeof(size_t));
}

inline void *allocator_boundary_tags::get_first_occupied_block_address()
{
    return *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(_trusted_memory) + sizeof(allocator *) + sizeof(logger *) + sizeof(size_t) + sizeof(std::mutex) + sizeof(allocator_with_fit_mode::fit_mode));

    // return *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(_trusted_memory) + get_allocator_metadata_size() - sizeof(void *));
}
