#include "HeaderReader.h"
#include <istream>
#include <limits>       

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
  ReadFields(header, file);


  return header;
}

void HeaderReader::ReadMagicNumber( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.magic_number, 4);
  SwapEndianess32(header.magic_number);
  if (header.magic_number != JAVA_MAGIC_NUMBER)
  {
    throw std::runtime_error("Magic constant does not match - not a java class file!");
  }
}

void HeaderReader::ReadMinorVersion( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.minor_version, 2);
  SwapEndianess16(header.minor_version);
}

void HeaderReader::ReadMajorVersion( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.major_version, 2);
  SwapEndianess16(header.major_version);
}

void HeaderReader::ReadConstantPoolCount( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.constant_pool_count, 2);
  SwapEndianess16(header.constant_pool_count);
}

void HeaderReader::ReadConstants( std::istream &file, JavaHeader &header )
{
  header.constant_pool.reserve(header.constant_pool_count);
  header.constant_pool.push_back(nullptr); // indexing begins from 1
  std::vector<Validation> validations;
  validations.reserve(header.constant_pool_count);

  for (int i = 1; i < header.constant_pool_count; ++i)
  {
    unsigned char tag; 
    file.read((char *)&tag, 1);
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

void HeaderReader::ReadClassData( std::istream & file, JavaHeader & header, std::vector<Validation> &validations )
{
  auto p = new cp_class_info;
  p->tag = CONSTANT_Class;
  file.read((char *)&p->name_index, 2);
  SwapEndianess16(p->name_index);

  validations.emplace_back(p->name_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadRef( std::istream & file, JavaHeader & header, unsigned char tag, std::vector<Validation> &validations )
{
  auto p = new cp_refs_info;
  p->tag = tag;

  file.read((char *)&p->class_index, 2);
  SwapEndianess16(p->class_index);
  validations.emplace_back(p->class_index, CONSTANT_Class, "Reference class index must point to CONSTANT_class");

  file.read((char *)&p->name_and_type_index, 2);
  SwapEndianess16(p->name_and_type_index);
  validations.emplace_back(p->name_and_type_index, CONSTANT_NameAndType);

  header.constant_pool.push_back(p);
}


void HeaderReader::ReadString( std::istream & file, JavaHeader & header, std::vector<Validation> &validations )
{
  auto p = new cp_string_info;
  p->tag = CONSTANT_String;
  file.read((char *)&p->string_index, 2);
  SwapEndianess16(p->string_index);

  validations.emplace_back(p->string_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadInteger( std::istream & file, JavaHeader & header )
{
  auto p = new cp_integer_float_info;
  p->tag = CONSTANT_Integer;
  uint32_t temp;
  file.read((char *)&temp, 4);
  SwapEndianess32(temp);
  p->value.i = temp;
  header.constant_pool.push_back(p);
}

void HeaderReader::ReadFloat( std::istream & file, JavaHeader & header )
{
  auto p = new cp_integer_float_info;
  p->tag = CONSTANT_Float;

  uint32_t temp;
  file.read((char *)&temp, 4);
  SwapEndianess32(temp);
  
  p->value.i = temp; // conserves the bit pattern;
  header.constant_pool.push_back(p);
}

void HeaderReader::ReadLong( std::istream & file, JavaHeader & header )
{
  auto p = new cp_long_double_info;
  p->tag = CONSTANT_Long;
  uint64_t temp;
  file.read((char *)&temp, 8);
  SwapEndianess64(temp);
  p->value.l = temp;
  header.constant_pool.push_back(p);
  header.constant_pool.push_back(nullptr); // takes two slots
}

void HeaderReader::ReadDouble( std::istream & file, JavaHeader & header )
{
  auto p = new cp_long_double_info;
  p->tag = CONSTANT_Double;
  uint64_t temp;
  file.read((char *)&temp, 8);
  SwapEndianess64(temp);
  p->value.l = temp;
  header.constant_pool.push_back(p);
  header.constant_pool.push_back(nullptr); // takes two slots
}

void HeaderReader::ReadNameAndType( std::istream & file, JavaHeader & header, std::vector<Validation> &validations )
{
  auto p = new cp_name_and_type_info;
  p->tag = CONSTANT_NameAndType;
  file.read((char *)&p->name_index, 2);
  SwapEndianess16(p->name_index);
  validations.emplace_back(p->name_index, CONSTANT_Utf8);

  file.read((char *)&p->descriptor_index, 2);
  SwapEndianess16(p->descriptor_index);
  validations.emplace_back(p->descriptor_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadUtf8( std::istream & file, JavaHeader & header )
{
  auto p = new cp_utf8_info;
  p->tag = CONSTANT_Utf8;
  file.read((char *)&p->length, 2);
  SwapEndianess16(p->length);

  p->bytes = new unsigned char[p->length];

  for (int i = 0; i < p->length; ++i)
  {
    file.read((char *)p->bytes + i, 1);
    if (p->bytes[i] == 0 || p->bytes[i] > 0xf0)
    {
      throw std::runtime_error("Invalid byte value in Utf8 constant");
    }
  }

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadAccessFlags( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.access_flags, 2);
  SwapEndianess16(header.access_flags);

  // check for invalid combinations
  if (header.access_flags & ACC_INTERFACE)
  {
    if (!(header.access_flags & ACC_ABSTRACT))
    {
      throw std::runtime_error("Invalid access flags - interface must be also abstract");
    }

    if ((header.access_flags & ACC_FINAL) || (header.access_flags & ACC_SUPER))
    {
      throw std::runtime_error("Invalid access flags - interface cannot set final or super flags");
    }
  } 
  else if ((header.access_flags & ACC_FINAL) && ((header.access_flags & ACC_ABSTRACT)))
  {
    throw std::runtime_error("Invalid access flags - class cannot be both final and abstract");
  }
}

void HeaderReader::ReadThisClass( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.this_class, 2);
  SwapEndianess16(header.this_class);

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

void HeaderReader::ReadSuperClass( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.super_class, 2);
  SwapEndianess16(header.super_class);
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

void HeaderReader::ReadInterfaceCount( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.interfaces_count, 2);
  SwapEndianess16(header.interfaces_count);
}

void HeaderReader::ReadInterfaces( JavaHeader &header, std::istream &file )
{
  for (int i = 0; i < header.interfaces_count; ++i)
  {
    uint16_t interface;
    file.read((char *)&interface, 2);
    SwapEndianess16(interface);

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

void HeaderReader::ReadFieldCount( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.field_count, 2);
  SwapEndianess16(header.field_count);
}

void HeaderReader::ReadFields( JavaHeader &header, std::istream &file )
{
  for (int i = 0; i < header.field_count; ++i)
  {
    field_info field;
    file.read((char *)&field.access_flags, 2);
    SwapEndianess16(field.access_flags);

    file.read((char *)&field.name_index, 2);
    SwapEndianess16(field.name_index);

    file.read((char *)&field.descriptor_index, 2);
    SwapEndianess16(field.descriptor_index);

    file.read((char *)&field.attributes_count, 2);
    SwapEndianess16(field.attributes_count);

    ReadFieldAttributes(field, file);


    header.fields.push_back(field);
  }
}

void HeaderReader::ReadFieldAttributes( field_info &field, std::istream &file )
{
  for (int i = 0; i < field.attributes_count; ++i)
  {
    attribute_info attribute;

    file.read((char *)&attribute.attribute_name_index, 2);
    SwapEndianess16(attribute.attribute_name_index);


    file.read((char *)&attribute.attribute_length, 4);
    SwapEndianess32(attribute.attribute_length);

    attribute.info = new uint8_t[attribute.attribute_length];

    for (int j = 0; j < attribute.attribute_length; ++j)
    {
      file.read((char *)(attribute.info +j ), 1);
    }
    field.attributes.push_back(attribute);
  }
}
