#pragma once
#include <cstdint>

enum class AttributeType : int
{
  CONSTANT_VALUE, CODE, STACK_MAP_TABLE, EXCEPTIONS, INNER_CLASSES, ENCLOSING_METHODS, SYNTHETIC,
  SIGNATURE, SOURCE_FILE, SOURCE_DEBUG_EXTENSION, LINE_NUMBER_TABLE, LOCAL_VARIABLE_TABLE, 
  LOCAL_VARIABLE_TYPE_TABLE, DEPRECATED, RUNTIME_VISIBLE_ANNOTATIONS, RUNTIME_INVISIBLE_ANNTOTATIONS,
  RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS, RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS, ANNOTATION_DEFAULT,
  BOOTSTRAP_METHODS 
};

class attribute_info
{
public:
  attribute_info() : attribute_name_index(0) { }
  virtual ~attribute_info() { }
  uint16_t attribute_name_index;
  AttributeType type;
};



class attribute_constant_value_info : public attribute_info
{
public:
  attribute_constant_value_info() { type = AttributeType::CONSTANT_VALUE; }
  uint16_t constant_value_index;
};

class code_attribute_info : public attribute_info
{
public:

};