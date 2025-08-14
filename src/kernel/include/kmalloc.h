#ifndef KMALLOC_H
#define KMALLOC_H

#include "types.h"

void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif // KMALLOC_H
