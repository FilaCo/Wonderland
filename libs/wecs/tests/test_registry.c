#include "stdx/alloc/xalloc.h"
#include "unity.h"
#include "wecs/registry.h"
#include <stdlib.h>

void *test_alloc([[maybe_unused]] void *ctx, void *ptr,
                 [[maybe_unused]] size_t osize, size_t nsize) {
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  return realloc(ptr, nsize);
}

void setUp(void) {}

void tearDown(void) {}

void test_registry_spawn(void) {
  // arrange
  registry sut = registry_new(test_alloc);
  id expected = 0;

  // act
  id actual = registry_spawn(sut);

  // assert
  TEST_ASSERT_EQUAL(expected, actual);
  TEST_ASSERT_TRUE(registry_is_alive(sut, actual));

  // cleanup
  registry_free(sut);
}

void test_registry_despawn(void) {
  // arrange
  registry sut = registry_new(test_alloc);
  id id_ = registry_spawn(sut);

  // act
  registry_despawn(sut, id_);

  // assert
  TEST_ASSERT_TRUE(registry_is_dead(sut, id_));

  // cleanup
  registry_free(sut);
}

void test_registry_spawn_with_recycling(void) {
  // arrange
  registry sut = registry_new(test_alloc);

  // act
  for (int i = 0; i < 10; ++i) {
    registry_spawn(sut);
  }

  for (uint32_t i = 0; i < 10; i += 2) {
    registry_despawn(sut, i);
  }

  // assert
  for (uint32_t i = 0; i < 10; ++i) {
    TEST_ASSERT_EQUAL((i & 1) == 0, registry_is_dead(sut, i));
  }

  for (uint32_t i = 0; i < 10; i += 2) {
    id id_ = (8 - i) | (1 << 20);
    TEST_ASSERT_EQUAL(id_, registry_spawn(sut));
  }

  // cleanup
  registry_free(sut);
}
