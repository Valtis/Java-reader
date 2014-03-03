#include "HeaderReader.h"
#include <fstream>
#include <iostream>
#include "Utility.h"

void PrintMagicNumber(JavaHeader &header);
void PrintVersionInfo(JavaHeader &header);
void PrintConstantPool(JavaHeader &header);
void PrintAccessFlags(JavaHeader &header);
void PrintThisAndSuperClass(JavaHeader &header);
void PrintInterfaces(JavaHeader &header);
void PrintFields(JavaHeader &header);

int main()
{
  std::ifstream file("TestiClass.class", std::ios::binary);
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
  
  PrintMagicNumber(header);
  PrintVersionInfo(header);
  PrintConstantPool(header);
  PrintAccessFlags(header);
  PrintThisAndSuperClass(header);
  PrintInterfaces(header);
  PrintFields(header);


  std::cout << "Method count:" << header.methods_count << "\n";

  system("pause");

}

void PrintMagicNumber(JavaHeader &header)
{
  if (header.magic_number == JAVA_MAGIC_NUMBER)
  {
    std::cout << "Magic number matches - 0xCAFEBABE\n";
  }
  else
  {
    std::cout << "Magic number does not match\n";
  }
}

void PrintVersionInfo(JavaHeader &header)
{
  std::cout << "Major version: " << header.major_version << " Minor version: " << header.minor_version << "\n";
  std::cout << "Java version: " << header.GetVersion() << "\n";
}

void PrintConstantPool(JavaHeader &header)
{
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
      if (p->tag == CONSTANT_Utf8)
      {
        PrintStr(p);
      }
    }
  }
}

void PrintAccessFlags(JavaHeader &header)
{
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
}

void PrintThisAndSuperClass(JavaHeader &header)
{
  std::cout << "This_class: " << header.this_class << "\n";
  std::cout << "Super_class: " << header.super_class << "\n";
  std::cout << "Super class name: ";
  {
    auto cp_class = static_cast<cp_class_info *>(header.constant_pool[header.super_class].get());
    PrintStr(header.constant_pool[cp_class->name_index]);
  }
}

void PrintInterfaces(JavaHeader &header)
{
  std::cout << "Interfaces_count: " << header.interfaces_count << "\n";

  if (header.interfaces_count != 0)
  {
    std::cout << "Interfaces: \n";
    for (auto i : header.interfaces)
    {
      std::cout << "  " << i << "\n";
      std::cout << "  Name: ";
      auto cp_class = static_cast<cp_class_info *>(header.constant_pool[i].get());
      PrintStr(header.constant_pool[cp_class->name_index]);
    }
  }
}

void PrintFieldAccessFlags(const field_info &field);

void PrintFieldAttributes(const field_info &field, JavaHeader &header);

void PrintFields(JavaHeader &header)
{
  std::cout << "Field_count: " << header.fields_count << "\n";

  std::cout << "Fields:\n";
  for (const auto &field : header.fields)
  {
    PrintFieldAccessFlags(field);

    std::cout << "  Name: ";
    PrintStr(header.constant_pool[field.name_index]);

    std::cout << "  Descriptor: ";
    PrintStr(header.constant_pool[field.descriptor_index]);

    std::cout << "  Attribute count: " << field.attributes_count << "\n";

    PrintFieldAttributes(field, header);

    std::cout << "\n";
  }
}

void PrintFieldAccessFlags(const field_info &field)
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
}

void PrintConstant(uint16_t constant_value_index, JavaHeader &header)
{
  auto cp = header.constant_pool[constant_value_index];
  
  if (cp->tag == CONSTANT_Utf8)
  {
    std::cout << "    Constant_Utf8, value: ";
    PrintStr(cp);
  }
  else if (cp->tag == CONSTANT_Long)
  {
    std::cout << "    Constant_Long, value: ";
    auto t = static_cast<cp_long_double_info *>(cp.get());
    std::cout << t->value.l;
  }
  else if (cp->tag == CONSTANT_Integer)
  {
    std::cout << "    Constant_Integer, value: ";
    auto t = static_cast<cp_integer_float_info *>(cp.get());
    std::cout << t->value.i;
  }
  else if (cp->tag == CONSTANT_Float)
  {
    std::cout << "    Constant_Float, value: ";
    auto t = static_cast<cp_integer_float_info *>(cp.get());
    std::cout << t->value.f;
  }
  else if (cp->tag == CONSTANT_Double)
  {
    std::cout << "    Constant_Double, value: ";
    auto t = static_cast<cp_long_double_info *>(cp.get());
    std::cout << t->value.d;
  }
  else 
  {
    std::cout << "TAG: " << (int)cp->tag << "\n";
  }

}

void PrintFieldAttributes(const field_info &field, JavaHeader &header)
{
  for (const auto a : field.attributes)
  {
    std::cout << "    Attribute name: ";
    PrintStr(header.constant_pool[a->attribute_name_index]);

    if (a->type == AttributeType::CONSTANT_VALUE)
    {
      std::cout << "    Constant value attribute\n";
      auto *cv = static_cast<attribute_constant_value_info *>(a.get());
      PrintConstant(cv->constant_value_index, header);
    }

    

    std::cout << "\n";
  }
}
