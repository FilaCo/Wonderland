#ifndef STDX_ALLOCATORS_XALLOC_H
#define STDX_ALLOCATORS_XALLOC_H

#include <stddef.h>

/**
 * @brief Generic allocator function type.
 *
 * This type specifies the interface that every allocator must meet in order to
 * be used in the stdx. The allocator function must provide a
 * functionality similar to `realloc`, but not exactly the same. The rules are
 * the following:
 *
 * - If `nsize == 0`, then it MUST behave as the `free` function.
 * - If `nsize != 0`, then it MUST behave like the `realloc` function.
 * - The allocator returns `NULL` if and only if it cannot fulfill the request.
 * - Allocator should never fail when `osize >= nsize`.
 *
 * @param ctx user data
 * @param ptr a pointer to the block beeing allocated/reallocated/freed
 * @param osize the original size of the block
 * @param nsize the new size of the block
 *
 * @return the actual pointer to the block
 */
typedef void *xalloc(void *ctx, void *ptr, size_t osize, size_t nsize);

#endif // STDX_ALLOCATORS_XALLOC_H
