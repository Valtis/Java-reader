#include "HeaderReader.h"

#include <istream>
void SwapEndianess16(short int &val)
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
  header.constant_pool = new cp_pool[header.constant_pool_count];

  for (int i = 1; i < header.constant_pool)

}
