#pragma once
#include <cstdint>
#include <vector>

struct attribute_info
{
  uint16_t attribute_name_index;
  uint32_t attribute_length;
  uint8_t *info;

};

struct field_info {
  uint16_t access_flags;
  uint16_t name_index;
  uint16_t descriptor_index;
  uint16_t attributes_count;
  std::vector<attribute_info> attributes;
};
