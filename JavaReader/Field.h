#pragma once
#include <cstdint>
#include <vector>
#include "Attribute.h"
#include <memory>

struct field_info {
  uint16_t access_flags;
  uint16_t name_index;
  uint16_t descriptor_index;
  uint16_t attributes_count;
  std::vector<std::shared_ptr<attribute_info>> attributes;
};
