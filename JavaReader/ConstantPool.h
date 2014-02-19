#pragma once

const unsigned char CONSTANT_Class = 7;
const unsigned char CONSTANT_Fieldref =	9;
const unsigned char CONSTANT_Methodref =	10;
const unsigned char CONSTANT_InterfaceMethodref =	11;
const unsigned char CONSTANT_String =	8;
const unsigned char CONSTANT_Integer = 	3;
const unsigned char CONSTANT_Float =	4;
const unsigned char CONSTANT_Long =	5;
const unsigned char CONSTANT_Double =	6;
const unsigned char CONSTANT_NameAndType = 	12;
const unsigned char CONSTANT_Utf8 =	1;

class cp_info
{
public:
  cp_info() { }
  virtual ~cp_info() { }
  uint8_t tag;
};

class cp_class_info : public cp_info
{
public:
  cp_class_info() { }
  ~cp_class_info() { }
  uint16_t name_index;
};

class cp_refs_info: public cp_info
{
public:
  uint16_t class_index;
  uint16_t name_and_type_index;
};

class cp_string_info : public cp_info
{
public:
  uint16_t string_index;
};

class cp_integer_float_info : public cp_info
{
public:
  
  union 
  {
    int32_t i;
    float f;
  } value;
};

class cp_long_double_info : public cp_info
{
public:
  
  union 
  {
    int64_t l;
    double d;
  } value;

};

class cp_name_and_type_info : public cp_info
{
public:
  uint16_t name_index;
  uint16_t descriptor_index;
};


class cp_utf8_info : public cp_info
{
public:
  cp_utf8_info() : bytes(nullptr) { }
  ~cp_utf8_info() { delete [] bytes; }

  uint16_t length;
  uint8_t *bytes;
};