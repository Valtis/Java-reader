#include "HeaderReader.h"
#include <fstream>
#include <iostream>

int main()
{
  std::ifstream file("Peli.class", std::ios::binary);
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
  std::cout << "Super_class: " << header.super_class << "\n";
  std::cout << "Super class name: ";
  {
    auto cp = (cp_utf8_info *)header.constant_pool[((cp_class_info*)header.constant_pool[header.super_class])->name_index];
    for (int j = 0; j < cp->length; ++j)
    {
      std::cout << cp->bytes[j];
    }
    std::cout << "\n";
  }
  std::cout << "Interfaces_count: " << header.interfaces_count << "\n";

  if (header.interfaces_count != 0)
  {
    std::cout << "Interfaces: \n";
    for (auto i : header.interfaces)
    {
      std::cout << "  " << i << "\n";
      std::cout << "  Name: ";
      auto cp = (cp_utf8_info *)header.constant_pool[((cp_class_info*)header.constant_pool[i])->name_index];
      for (int j = 0; j < cp->length; ++j)
      {
        std::cout << cp->bytes[j];
      }
      std::cout << "\n";

    }
  }

  std::cout << "Field_count: " << header.field_count << "\n";

  std::cout << "Fields:\n";
  for (const auto &field : header.fields)
  {
    std::cout << "  Access flag: " << field.access_flags << "\n";
    if (field.access_flags & ACC_PUBLIC)
      std::cout << "    Public" << "\n";
    if (field.access_flags & ACC_PRIVATE)
      std::cout << "    Private" << "\n";
    if (field.access_flags & ACC_PROTECTED)
      std::cout << "    Protected" << "\n";
    if (field.access_flags & ACC_STATIC)
      std::cout << "    Static" << "\n";
    if (field.access_flags & ACC_FINAL)
      std::cout << "    Final" << "\n";
    if (field.access_flags & ACC_VOLATILE)
      std::cout << "    Volatile" << "\n";
    if (field.access_flags & ACC_TRANSIENT)
      std::cout << "    Transient" << "\n";
    if (field.access_flags & ACC_SYNTHETIC)
      std::cout << "    Synthetic" << "\n";
    if (field.access_flags & ACC_ENUM)
      std::cout << "    Enum" << "\n";

    std::cout << "  Name: ";
    auto cp = (cp_utf8_info *)header.constant_pool[field.name_index];
    for (int i = 0; i < cp->length; ++i)
    {
      std::cout << cp->bytes[i];
    }
    std::cout << "\n  Descriptor: ";

    cp = (cp_utf8_info *)header.constant_pool[field.descriptor_index];
    for (int i = 0; i < cp->length; ++i)
    {
      std::cout << cp->bytes[i];
    }
    std::cout << "\n";

    std::cout << "  Attribute count: " << field.attributes_count << "\n";

    for (const auto a : field.attributes)
    {
      std::cout << "    Attribute name: ";
      auto cp = (cp_utf8_info *)header.constant_pool[a.attribute_name_index];
      for (int i = 0; i < cp->length; ++i)
      {
        std::cout << cp->bytes[i];
      }

      std::cout << "\n    Attribute length: " << a.attribute_length << "\n";
      std::cout << "    Attribute data: ";

      for (int i = 0; i < a.attribute_length; ++i)
      {
        std::cout << a.info[i];
      }
      uint16_t temp = 0;
      temp = a.info[0] << 8 | a.info[1];
      std::cout << "\n    Temp: " << temp << "\n";

      std::cout << "    Tag: " << (int)header.constant_pool[temp]->tag << "\n";
      if (header.constant_pool[temp]->tag == CONSTANT_Utf8)
      {
        std::cout << "    UTF tag value: ";

        auto cp = (cp_utf8_info *)header.constant_pool[temp];
        for (int i = 0; i < cp->length; ++i)
        {
          std::cout << cp->bytes[i];
        }
        std::cout << "\n";
      }
      else if (header.constant_pool[temp]->tag == CONSTANT_Long)
      {
        std::cout << "Long value: " << ((cp_long_double_info *)header.constant_pool[temp])->value.l;
      }


      std::cout << "\n";
    }


    std::cout << "\n";
  }

  system("pause");

}


/*
ACC_PUBLIC 	0x0001 	Declared public; may be accessed from outside its package.
ACC_PRIVATE 	0x0002 	Declared private; usable only within the defining class.
ACC_PROTECTED 	0x0004 	Declared protected; may be accessed within subclasses.
ACC_STATIC 	0x0008 	Declared static.
ACC_FINAL 	0x0010 	Declared final; never directly assigned to after object construction (JLS §17.5).
ACC_VOLATILE 	0x0040 	Declared volatile; cannot be cached.
ACC_TRANSIENT 	0x0080 	Declared transient; not written or read by a persistent object manager.
ACC_SYNTHETIC 	0x1000 	Declared synthetic; not present in the source code.
ACC_ENUM 	0x4000 	Declared as an element of an enum.
*/