#pragma once
#include <cstdint>

class attribute_info
{
public:
  attribute_info() : attribute_name_index(0), attribute_length(0) { }
  virtual ~attribute_info() { }
  uint16_t attribute_name_index;
  uint32_t attribute_length;
  uint8_t *info;

};



class attribute_constant_value_info : public attribute_info
{
  uint16_t constant_value_index;
};

class code_attribute_info : public attribute_info
{

};