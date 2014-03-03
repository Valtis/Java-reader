#pragma once
#include <cstdint>
#include "AccessFlags.h"
#include "ConstantPool.h"
#include "Field.h"
#include "MethodInfo.h"
#include <vector>
#include <memory>

const unsigned int JAVA_MAGIC_NUMBER = 0xCAFEBABE;

struct JavaHeader
{
  JavaHeader() { }
  ~JavaHeader()
  {

  }

  uint32_t magic_number;
  uint16_t minor_version;
  uint16_t major_version;
  uint16_t constant_pool_count;
  std::vector<std::shared_ptr<cp_info>> constant_pool;
  uint16_t access_flags;
  uint16_t this_class;
  uint16_t super_class;
  uint16_t interfaces_count;
  std::vector<uint16_t> interfaces;
  uint16_t fields_count;
  std::vector<field_info> fields;
  uint16_t methods_count;
  std::vector<method_info> methods;

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