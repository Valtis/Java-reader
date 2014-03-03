#pragma once
#include <cstdint>
#include <vector>
#include "Attribute.h"
#include <memory>

struct method_info
{
  uint16_t access_flags;
  uint16_t name_index;
  uint16_t descriptor_index;
  uint16_t attribute_count;
  std::vector<std::shared_ptr<attribute_info>> attributes;
};