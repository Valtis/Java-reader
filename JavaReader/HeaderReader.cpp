#include "HeaderReader.h"
#include <istream>
#include <limits>       

void SwapEndianess16(unsigned short int &val)
{
  val = (val >> 8) | (val << 8);
}

void SwapEndianess32(unsigned int &val)
{

  val = ((val & (255 << 24)) >> 24) | ((val & (255 << 16)) >> 8) | ((val & (255 << 8)) << 8) | ((val & 255) << 24);
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

  return header;
}

void HeaderReader::ReadMagicNumber( std::istream &file, JavaHeader &header )
{
  file.read((char *)&header.magic_number, 4);
  SwapEndianess32(header.magic_number);
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
      break;  
    case CONSTANT_Double:
      ReadDouble(file, header);
      break;
    case CONSTANT_NameAndType:
      ReadNameAndType(file, header);
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
  auto p = new cp_pool_class;
  p->tag = CONSTANT_Class;
  file.read((char *)&p->name_index, 2);
  SwapEndianess16(p->name_index);

  validations.emplace_back(p->name_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadRef( std::istream & file, JavaHeader & header, unsigned char tag, std::vector<Validation> &validations )
{
  auto p = new cp_pool_refs;
  p->tag = tag;

  file.read((char *)&p->class_index, 2);
  SwapEndianess16(p->class_index);
  validations.emplace_back(p->class_index, CONSTANT_Class);

  file.read((char *)&p->name_and_type_index, 2);
  SwapEndianess16(p->name_and_type_index);
  validations.emplace_back(p->name_and_type_index, CONSTANT_NameAndType);

  header.constant_pool.push_back(p);
}


void HeaderReader::ReadString( std::istream & file, JavaHeader & header, std::vector<Validation> &validations )
{
  auto p = new cp_pool_string;
  p->tag = CONSTANT_String;
  file.read((char *)&p->string_index, 2);
  SwapEndianess16(p->string_index);

  validations.emplace_back(p->string_index, CONSTANT_Utf8);

  header.constant_pool.push_back(p);
}

void HeaderReader::ReadInteger( std::istream & file, JavaHeader & header )
{
  auto p = new cp_pool_integer_float;
  p->tag = CONSTANT_Integer;
  unsigned int temp;
  file.read((char *)&temp, 4);
  SwapEndianess32(temp);
  p->value.i = temp;
}

void HeaderReader::ReadFloat( std::istream & file, JavaHeader & header )
{
  auto p = new cp_pool_integer_float;
  p->tag = CONSTANT_Float;

  unsigned int temp;
  file.read((char *)&temp, 4);
  SwapEndianess32(temp);
  
  p->value.i = temp; // conserves the bit pattern;
  
  /*if (temp == 0x7f800000)
  {
    p->value.f = std::numeric_limits<float>::infinity();
  }
  else if (temp == 0xff800000)
  {
    p->value.f = -std::numeric_limits<float>::infinity();
  }
  else if ((temp >= 0x7f800001 && temp <=0x7fffffff) || (temp>= 0xff800001 && temp <= 0xffffffff))
  {
    p->value.f = std::numeric_limits<float>::quiet_NaN();
  }
  else
  {
    int s = ((temp >> 31) == 0) ? 1 : -1;
    int e = ((temp >> 23) & 0xff);
    int m = (e == 0) ? (temp & 0x7fffff) << 1 : (temp & 0x7fffff) | 0x800000;
    p->value.f = s * m * pow(2, e - 150);
  }*/

}

void HeaderReader::ReadLong( std::istream & file, JavaHeader & header )
{
  throw std::logic_error("The method or operation is not implemented.");
}

void HeaderReader::ReadDouble( std::istream & file, JavaHeader & header )
{
  throw std::logic_error("The method or operation is not implemented.");
}

void HeaderReader::ReadNameAndType( std::istream & file, JavaHeader & header )
{
  throw std::logic_error("The method or operation is not implemented.");
}

void HeaderReader::ReadUtf8( std::istream & file, JavaHeader & header )
{
  auto p = new cp_pool_utf8;
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
