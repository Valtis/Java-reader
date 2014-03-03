#include "HeaderReader.h"
#include <istream>
#include <limits>       
#include <iostream>
#include "Utility.h"

void SwapEndianess16(uint16_t &val)
{
  val = (val >> 8) | (val << 8);
}

void SwapEndianess32(uint32_t &val)
{
  uint32_t mask = 255;
  val = ((val & (mask << 24)) >> 24) | ((val & (mask << 16)) >> 8) | ((val & (mask << 8)) << 8) | ((val & mask) << 24);
}

void SwapEndianess64(uint64_t &val)
{
  uint64_t mask = 255;
  val =
    ((val & (mask << 54)) >> 54) |
    ((val & (mask << 48)) >> 40) |
    ((val & (mask << 40)) >> 24) |
    ((val & (mask << 32)) >> 8) |
    ((val & (mask << 24)) << 8) |
    ((val & (mask << 16)) << 24) |
    ((val & (mask << 8)) << 40) |
    (val & 255) << 54;
}

void Read8(std::istream &file, uint8_t &value)
{
  file.read((char *)&value, 1);
}

void Read16(std::istream &file, uint16_t &value)
{
  file.read((char *)&value, 2);
  SwapEndianess16(value);
}

void Read32(std::istream &file, uint32_t &value)
{
  file.read((char *)&value, 4);
  SwapEndianess32(value);
}

void Read64(std::istream &file, uint64_t &value)
{
  file.read((char *)&value, 8);
  SwapEndianess64(value);
}

HeaderReader::HeaderReader()
{
}


HeaderReader::~HeaderReader()
{
}

JavaHeader HeaderReader::ReadHeader(std::istream &file)
{
  JavaHeader header;

  ReadMagicNumber(file, header);

  ReadMinorVersion(file, header);
  ReadMajorVersion(file, header);

  ReadConstantPoolCount(file, header);
  ReadConstants(file, header);

  ReadAccessFlags(file, header);

  ReadThisClass(file, header);
  ReadSuperClass(file, header);

  ReadInterfaceCount(file, header);
  ReadInterfaces(header, file);

  ReadFieldCount(file, header);
  ReadFields(file, header);

  ReadMethodCount(file, header);
  //ReadMethods(file, header);


  return header;
}

void HeaderReader::ReadMagicNumber(std::istream &file, JavaHeader &header)
{
  Read32(file, header.magic_number);
  if (header.magic_number != JAVA_MAGIC_NUMBER)
  {
    throw std::runtime_error("Magic constant does not match - not a java class file!");
  }
}

void HeaderReader::ReadMinorVersion(std::istream &file, JavaHeader &header)
{
  Read16(file, header.minor_version);
}

void HeaderReader::ReadMajorVersion(std::istream &file, JavaHeader &header)
{
  Read16(file, header.major_version);
}

void HeaderReader::ReadConstantPoolCount(std::istream &file, JavaHeader &header)
{
  Read16(file, header.constant_pool_count);
}

void HeaderReader::ReadConstants(std::istream &file, JavaHeader &header)
{
  header.constant_pool.reserve(header.constant_pool_count);
  header.constant_pool.push_back(nullptr); // indexing begins from 1
  std::vector<Validation> validations;
  validations.reserve(header.constant_pool_count);

  for (int i = 1; i < header.constant_pool_count; ++i)
  {
    unsigned char tag;
    Read8(file, tag);
    switch (tag)
    {
    case CONSTANT_Class:
      ReadClassData(file, header, validations);
      break;
    case CONSTANT_Fieldref:
      ReadRef(file, header, tag, validations);
      break;
    case CONSTANT_Methodref:
      ReadRef(file, header, tag, validations);
      break;
    case CONSTANT_InterfaceMethodref:
      ReadRef(file, header, tag, validations);
      break;
    case CONSTANT_String:
      ReadString(file, header, validations);
      break;
    case CONSTANT_Integer:
      ReadInteger(file, header);
      break;
    case CONSTANT_Float:
      ReadFloat(file, header);
      break;
    case CONSTANT_Long:
      ReadLong(file, header);
      ++i;
      break;
    case CONSTANT_Double:
      ReadDouble(file, header);
      ++i;
      break;
    case CONSTANT_NameAndType:
      ReadNameAndType(file, header, validations);
      break;
    case CONSTANT_Utf8:
      ReadUtf8(file, header);
      break;
    default:
      throw std::runtime_error("Unidentified constant type " + tag);
    }
  }

  for (auto &v : validations)
  {
    v.Validate(header.constant_pool);
  }
}

void HeaderReader::ReadClassData(std::istream & file, JavaHeader & header, std::vector<Validation> &validations)
{
  auto p = std::make_shared<cp_class_info>();
  p->tag = CONSTANT_Class;
  Read16(file, p->name_index);
  validations.emplace_back(p->name_index, CONSTANT_Utf8);
  header.constant_pool.push_back(p);
}

void HeaderReader::ReadRef(std::istream & file, JavaHeader & header, unsigned char tag, std::vector<Validation> &validations)
{
  auto p = std::make_shared<cp_refs_info>();
  p->tag = tag;

  Read16(file, p->class_index);
  Read16(file, p->name_and_type_index);

  validations.emplace_back(p->class_index, CONSTANT_Class, "Reference class index must point to CONSTANT_class");
  validations.emplace_back(p->name_and_type_index, CONSTANT_NameAndType);

  header.constant_pool.push_back(p);
}


void HeaderReader::ReadString(std::istream & file, JavaHeader & header, std::vector<Validation> &validations)
{
  auto p = std::make_shared<cp_string_info>();
  p->tag = CONSTANT_String;

  Read16(file, p->string_index);
  validations.emplace_back(p->string_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadInteger(std::istream & file, JavaHeader & header)
{
  auto p = std::make_shared<cp_integer_float_info>();
  p->tag = CONSTANT_Integer;

  uint32_t temp;
  Read32(file, temp);
  p->value.i = temp;
  header.constant_pool.push_back(p);
}

void HeaderReader::ReadFloat(std::istream & file, JavaHeader & header)
{
  auto p = std::make_shared<cp_integer_float_info>();
  p->tag = CONSTANT_Float;

  uint32_t temp;
  Read32(file, temp);

  p->value.i = temp; // conserves the bit pattern;
  header.constant_pool.push_back(p);
}

void HeaderReader::ReadLong(std::istream & file, JavaHeader & header)
{
  auto p = std::make_shared<cp_long_double_info>();
  p->tag = CONSTANT_Long;
  uint32_t high, low;
  Read32(file, high);
  Read32(file, low);

  p->value.l = ((uint64_t)(high) << 32) + low;
  header.constant_pool.push_back(p);
  header.constant_pool.push_back(nullptr); // takes two slots
}

void HeaderReader::ReadDouble(std::istream & file, JavaHeader & header)
{
  auto p = std::make_shared<cp_long_double_info>();
  p->tag = CONSTANT_Double;

  uint32_t high, low;
  Read32(file, high);
  Read32(file, low);
 
  p->value.l = ((uint64_t)(high) << 32) + low;


  header.constant_pool.push_back(p);
  header.constant_pool.push_back(nullptr); // takes two slots
}

void HeaderReader::ReadNameAndType(std::istream & file, JavaHeader & header, std::vector<Validation> &validations)
{
  auto p = std::make_shared<cp_name_and_type_info>();
  p->tag = CONSTANT_NameAndType;

  Read16(file, p->name_index);
  Read16(file, p->descriptor_index);

  validations.emplace_back(p->descriptor_index, CONSTANT_Utf8);
  validations.emplace_back(p->name_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadUtf8(std::istream & file, JavaHeader & header)
{
  auto p = std::make_shared<cp_utf8_info>();
  p->tag = CONSTANT_Utf8;

  Read16(file, p->length);

  p->bytes = new unsigned char[p->length];

  for (int i = 0; i < p->length; ++i)
  {
    Read8(file, p->bytes[i]);
    if (p->bytes[i] == 0 || p->bytes[i] > 0xf0)
    {
      throw std::runtime_error("Invalid byte value in Utf8 constant");
    }
  }

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadAccessFlags(std::istream &file, JavaHeader &header)
{
  Read16(file, header.access_flags);

  // check for invalid combinations
  if (header.access_flags & ACC_INTERFACE)
  {
    if (!(header.access_flags & ACC_ABSTRACT))
    {
      throw std::runtime_error("Invalid access flags - interface must be also abstract");
    }

    if ((header.access_flags & ACC_FINAL) || (header.access_flags & ACC_SUPER) || (header.access_flags & ACC_ENUM)) 
    {
      throw std::runtime_error("Invalid access flags - interface cannot set final, super or enum flags");
    }
  }
  else if ((header.access_flags & ACC_FINAL) && ((header.access_flags & ACC_ABSTRACT)))
  {
    throw std::runtime_error("Invalid access flags - class cannot be both final and abstract");
  }

  if ((header.access_flags & ACC_ANNOTATION) && !(header.access_flags & ACC_INTERFACE))
  {
    throw std::runtime_error("If annotation flag is set, interface flag must also be set");
  }


}

void HeaderReader::ReadThisClass(std::istream &file, JavaHeader &header)
{
  Read16(file, header.this_class);

  if (header.this_class < 1 || header.this_class >= header.constant_pool_count)
  {
    throw std::runtime_error("Invalid index to this class: " + std::to_string(header.this_class));
  }

  if (header.constant_pool[header.this_class]->tag != CONSTANT_Class)
  {
    throw std::runtime_error("Invalid constant type for this_class. Expected " + std::to_string(CONSTANT_Class) +
      " but actual type is " + std::to_string(header.constant_pool[header.this_class]->tag));
  }

}

void HeaderReader::ReadSuperClass(std::istream &file, JavaHeader &header)
{
  Read16(file, header.super_class);
  if (header.super_class != 0)
  {
    if (header.super_class < 1 || header.super_class >= header.constant_pool_count)
    {
      throw std::runtime_error("Invalid super class index: " + header.super_class);
    }

    if (header.constant_pool[header.super_class]->tag != CONSTANT_Class)
    {
      throw std::runtime_error("Invalid constant type for super_class. Expected " + std::to_string(CONSTANT_Class) +
        " but actual type is " + std::to_string(header.constant_pool[header.super_class]->tag));
    }
  }
}

void HeaderReader::ReadInterfaceCount(std::istream &file, JavaHeader &header)
{
  Read16(file, header.interfaces_count);
}

void HeaderReader::ReadInterfaces(JavaHeader &header, std::istream &file)
{
  for (int i = 0; i < header.interfaces_count; ++i)
  {
    uint16_t interface;
    Read16(file, interface);

    if (interface < 1 || interface >= header.constant_pool_count)
    {
      throw std::runtime_error("Invalid interface index: " + interface);
    }

    if (header.constant_pool[interface]->tag != CONSTANT_Class)
    {
      throw std::runtime_error("Invalid constant type for interface. Expected " + std::to_string(CONSTANT_Class) +
        " but actual type is " + std::to_string(header.constant_pool[interface]->tag));
    }

    header.interfaces.push_back(interface);
  }
}

void HeaderReader::ReadFieldCount(std::istream &file, JavaHeader &header)
{
  Read16(file, header.fields_count);
}

void HeaderReader::ReadFields(std::istream &file, JavaHeader &header)
{

  for (int i = 0; i < header.fields_count; ++i)
  {
    field_info field;

    ReadFieldAccessFlags(file, field, header.access_flags & ACC_INTERFACE);

    Read16(file, field.name_index);
    Validation name_index_validation(field.name_index, CONSTANT_Utf8);
    name_index_validation.Validate(header.constant_pool);

    Read16(file, field.descriptor_index);
    Validation descriptor_index_validation(field.descriptor_index, CONSTANT_Utf8);
    descriptor_index_validation.Validate(header.constant_pool);

    Read16(file, field.attributes_count);
    ReadAttributes(file, header, field.attributes_count, field.attributes);

    header.fields.push_back(field);
  }
}

void HeaderReader::ReadAttributes(std::istream &file, const JavaHeader &header, const int count, std::vector<std::shared_ptr<attribute_info>> &attributes)
{
  for (int i = 0; i < count; ++i)
  {
    std::shared_ptr<attribute_info> attribute;
    uint16_t name_index;
    uint32_t attribute_length;
    Read16(file, name_index);
    Validation name_validation(name_index, CONSTANT_Utf8);
    name_validation.Validate(header.constant_pool);


    Read32(file, attribute_length);
    
    if (StrCmp(header.constant_pool[name_index].get(), "ConstantValue"))
    {
      if (attribute_length != 2)
      {
        throw std::runtime_error("Invalid attribute length for attribute ConstantValue");
      }
      attribute = ReadConstantValueAttribute(file);
    }
    else 
    {
      std::cout << "Unknown attribute: ";
      PrintStr(header.constant_pool[name_index].get());
      // skip the attribute bytes
      file.seekg(attribute_length, std::ios_base::cur);
      continue;  // unknown attributes must be silently ignored
    }
    
   
    attribute->attribute_name_index = name_index;
    
    attributes.push_back(attribute);
  }
}


std::shared_ptr<attribute_info> HeaderReader::ReadConstantValueAttribute(std::istream & file)
{
  auto attr = std::make_shared<attribute_constant_value_info>();
  Read16(file, attr->constant_value_index);

  return attr;
}


void HeaderReader::ReadFieldAccessFlags(std::istream & file, field_info &field, bool isInterface)
{
  Read16(file, field.access_flags);
  CheckForMultipleAccessLevels(field.access_flags, "Invalid access modifiers for a field - cannot have multiple access levels");


  if ((field.access_flags & ACC_FINAL) && (field.access_flags & ACC_VOLATILE))
  {
    throw std::runtime_error("Field cannot be both volatile and final");
  }

  if (isInterface)
  {
    if (!(field.access_flags & ACC_PUBLIC) || !(field.access_flags & ACC_STATIC) || !(field.access_flags & ACC_FINAL))
    {
      throw std::runtime_error("Interface fields must be public, static and final");
    }
    
    if ((field.access_flags & ACC_VOLATILE) || (field.access_flags & ACC_TRANSIENT) || (field.access_flags & ACC_ENUM))
    {
      throw std::runtime_error("Interface cannot have its volatile, transient or enum flags set");
    }
  }

}

void HeaderReader::ReadMethodCount(std::istream & file, JavaHeader &header)
{
  Read16(file, header.methods_count);
}

void HeaderReader::ReadMethods(std::istream & file, JavaHeader &header)
{
  for (int i = 0; i < header.methods_count; ++i)
  {
    method_info method;
    ReadMethodAccessFlags(file, method, header.access_flags & ACC_INTERFACE);

    Read16(file, method.name_index);
    Validation name_validation(method.name_index, CONSTANT_Utf8);
    name_validation.Validate(header.constant_pool);

    Read16(file, method.descriptor_index);
    
    Validation descriptor_validation(method.descriptor_index, CONSTANT_Utf8);
    descriptor_validation.Validate(header.constant_pool);

    ReadAttributes(file, header, method.attribute_count, method.attributes);


    header.methods.push_back(method);
  }
}

void HeaderReader::ReadMethodAccessFlags(std::istream & file, method_info &method, bool isInterface)
{
  Read16(file, method.access_flags);
  CheckForMultipleAccessLevels(method.access_flags, "Invalid access modifiers for a method - cannot have multiple access levels");
  
  if ((method.access_flags & ACC_ABSTRACT) && ((method.access_flags & ACC_FINAL) || (method.access_flags & ACC_NATIVE) || (method.access_flags & ACC_PRIVATE) ||
     (method.access_flags & ACC_STATIC) || (method.access_flags & ACC_STRICT) || (method.access_flags & ACC_SYNCHRONIZED))) 
  {
    throw std::runtime_error("Abstract method cannot have its final, native, private, static, strict or synchronized fields set");
  }

  if (isInterface)
  {
    if (!(method.access_flags & ACC_ABSTRACT) || !(method.access_flags & ACC_PUBLIC))
    {
      throw std::runtime_error("Interface methods must have their abstract and public flags set");
    }
    
    if ((method.access_flags & ACC_STATIC) || (method.access_flags & ACC_FINAL) || (method.access_flags & ACC_SYNCHRONIZED) ||
      (method.access_flags & ACC_NATIVE) || (method.access_flags & ACC_STRICT))
    {
      throw std::runtime_error("Interface methods cannot have their static, final, synchronized, native or strict flags set");
    }
  }
  /* add checks for <init> access flags
   "A specific instance initialization method (§2.9) may have at most one of its ACC_PRIVATE, ACC_PROTECTED, and ACC_PUBLIC flags set, and may also have its ACC_STRICT, 
    ACC_VARARGS and ACC_SYNTHETIC flags set, but must not have any of the other flags in Table 4.5 set."
   */

}

void HeaderReader::CheckForMultipleAccessLevels(const uint16_t access_flags, std::string errorMsg)
{
  int access_modifiers = 0;
  if (access_flags & ACC_PUBLIC)
  {
    ++access_modifiers;
  }

  if (access_flags & ACC_PROTECTED)
  {
    ++access_modifiers;
  }

  if (access_flags & ACC_PRIVATE)
  {
    ++access_modifiers;
  }

  if (access_modifiers > 1)
  {
    throw std::runtime_error(errorMsg);
  }
}
