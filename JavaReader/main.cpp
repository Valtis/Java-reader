#include "HeaderReader.h"
#include <fstream>
#include <iostream>

int main()
{
  std::ifstream file("main.class", std::ios::binary);
  HeaderReader h;
  JavaHeader header;
  try 
  {
    header = h.ReadHeader(file);
  }
  catch (std::exception &ex)
  {
    std::cout << "Exception: " << ex.what() << "\n";
    system("pause");
    return -1;
  }

  if (header.magic_number == JAVA_MAGIC_NUMBER)
  {
    std::cout << "Magic number matches - 0xCAFEBABE\n";
  }
  else
  {
    std::cout << "Magic number does not match\n";
  }
  
  std::cout << "Major version: " << header.major_version << " Minor version: " << header.minor_version << "\n";
  std::cout << "Java version: " << header.GetVersion() << "\n";

  std::cout << "Constant pool count: " << header.constant_pool_count << "\n";
  
  std::cout << "Constant types: " << "\n";
  int index = 0;
  for (auto &p : header.constant_pool)
  {
    std::cout << "Index: " << index++ << " Type: ";
    if (p == nullptr)
    {
      std::cout << " Nullptr\n";
    }
    else 
    {
      std::cout << (int)p->tag << "\n";
      if (p->tag == 1)
      {
        cp_utf8_info *u = (cp_utf8_info *)p;
        for (int i = 0; i < u->length; ++i)
        {
          std::cout << u->bytes[i];
        }
        std::cout << "\n";
      }
    }
  }

  std::cout << "\n\nAccess flags: " << header.access_flags << std::endl;
  if (header.access_flags & ACC_PUBLIC)
  {
    std::cout << "  Public\n";
  }
  if (header.access_flags & ACC_FINAL)
  {
    std::cout << "  Final\n";
  }
  if (header.access_flags & ACC_SUPER)
  {
    std::cout << "  Super\n";
  }
  if (header.access_flags & ACC_INTERFACE)
  {
    std::cout << "  Interface\n";
  }
  if (header.access_flags & ACC_ABSTRACT)
  {
    std::cout << "  Abstract\n";
  }

  std::cout << "This_class: " << header.this_class << "\n";
  system("pause");

}