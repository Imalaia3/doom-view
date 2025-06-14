#pragma once
#include <stdint.h>

struct FileLump {
    uint32_t filepos;
    uint32_t size;
    char name[8];
} __attribute__((packed));