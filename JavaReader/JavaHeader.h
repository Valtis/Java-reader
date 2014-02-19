#pragma once
#include <memory>
#include "ConstantPool.h"
#include <vector>
const unsigned int JAVA_MAGIC_NUMBER = 0xCAFEBABE;
const int ACC_PUBLIC = 0x0001;
const int ACC_FINAL = 0x0010;
const int ACC_SUPER = 0x0020;
const int ACC_INTERFACE = 0x0200;
const int ACC_ABSTRACT = 0x0400;

struct JavaHeader
{
  JavaHeader() { }
  ~JavaHeader() 
  {
  
  }

  void ReleaseMemory()
  {
    for (cp_info *p : constant_pool)
    {
      delete p;
    }
  }


  uint32_t magic_number;
  uint16_t minor_version;
  uint16_t major_version;
  uint16_t constant_pool_count;
  std::vector<cp_info *> constant_pool;
  uint16_t access_flags;
  uint16_t this_class;

#ifdef _DEBUG
  const char * GetVersion()
  {
    switch (major_version)
    {
    case 51:
      return "7";
    case 50:
      return "6";
    case 49:
      return "5";
    case 48:
      return "JDK 1.4";
    case 47:
      return "JDK 1.3";
    case 46:
      return "JDK 1.2";
    case 45:
      return "JDK 1.1";
    default:
      return "Unknown version";
    }
  }
#endif

};