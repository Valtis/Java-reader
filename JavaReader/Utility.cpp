#include "ConstantPool.h"
#include <stdexcept>
#include <string>
#include <iostream>
#include "Utility.h"




bool StrCmp(std::shared_ptr<cp_info> &first, std::shared_ptr<cp_info> second)
{
  return StrCmp(first.get(), second.get());
}
bool StrCmp(cp_info *first, cp_info *second)
{
  if (first == nullptr || second == nullptr)
  {
    throw std::runtime_error("Null parameter in StrCmp");
  }

  if (first->tag != CONSTANT_Utf8 || second->tag != CONSTANT_Utf8)
  {
    throw std::runtime_error("Parameters must be strings");
  }

  auto firstString = static_cast<cp_utf8_info *>(first);
  auto secondString = static_cast<cp_utf8_info *>(second);

  if (firstString->length != secondString->length)
  {
    return false;
  }

  return !memcmp(firstString->bytes, secondString->bytes, firstString->length);
}

bool StrCmp(std::shared_ptr<cp_info> &first, const std::string &str)
{
  return StrCmp(first.get(), str);
}

bool StrCmp(cp_info *first, const std::string &str)
{
  if (first == nullptr)
  {
    throw std::runtime_error("Null parameter in StrCmp");
  }

  if (first->tag != CONSTANT_Utf8)
  {
    throw std::runtime_error("Parameters must be strings");
  }

  auto firstString = static_cast<cp_utf8_info *>(first);

  if (firstString->length != str.length())
  {
    return false;
  }

  return !memcmp(firstString->bytes, str.c_str(), firstString->length);
}

void PrintStr(std::shared_ptr<cp_info> &cp)
{
  PrintStr(cp.get());
}

void PrintStr(cp_info *cp)
{
  if (cp == nullptr)
  {
    throw std::runtime_error("Null parameter in PrintStr");
  }

  if (cp->tag != CONSTANT_Utf8)
  {
    throw std::runtime_error("Parameter must be a string for PrintStr");
  }

  cp_utf8_info *str = static_cast<cp_utf8_info *>(cp);


  for (int i = 0; i < str->length; ++i)
  {
    std::cout << str->bytes[i];
  }

  std::cout << "\n";

}