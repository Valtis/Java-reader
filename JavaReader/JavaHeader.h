#pragma once
#include <memory>
#include "ConstantPool.h"
const unsigned int JAVA_MAGIC_NUMBER = 0xCAFEBABE;

struct JavaHeader
{
  JavaHeader()
  {
    constant_pool = nullptr;
  }

  ~JavaHeader()
  {
    delete [] constant_pool;
  }

  unsigned int magic_number;
  short int minor_version;
  short int major_version;
  short int constant_pool_count;

  cp_pool *constant_pool;

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