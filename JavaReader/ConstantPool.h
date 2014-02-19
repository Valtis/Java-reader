#pragma once
#include "ConstantPoolStructs.h"

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

class cp_pool
{
public:
  cp_pool() { }
  virtual ~cp_pool() { }
  unsigned char tag;

};

class cp_pool_class : public cp_pool
{
public:
  cp_pool_class() { }
  ~cp_pool_class() { }
  unsigned short int name_index;
};

class cp_pool_refs: public cp_pool
{
public:
  unsigned short int class_index;
  unsigned short int name_and_type_index;

};

class cp_pool_string : public cp_pool
{
public:
  unsigned short int string_index;
};

class cp_pool_integer_float : public cp_pool
{
public:
  
  union {
    int i;
    float f;
  } value;
};




class cp_pool_utf8 : public cp_pool
{
public:
  cp_pool_utf8() : bytes(nullptr) { }
  ~cp_pool_utf8() { delete [] bytes; }

  unsigned short int length;
  unsigned char *bytes;
};