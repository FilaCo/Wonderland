#ifndef WECS_REGISTRY_H
#define WECS_REGISTRY_H

#include <stddef.h>

typedef struct __registry *registry;

registry registry_new(void);
registry registry_with_capacity(size_t cap);

#endif // WECS_REGISTRY_H
