#pragma once
#include "JavaHeader.h"
#include <fstream>

class HeaderReader
{
public:
  HeaderReader();
  ~HeaderReader();
  JavaHeader ReadHeader(std::istream &file);

private:

  void ReadMagicNumber( std::istream &file, JavaHeader &header );
  void ReadMinorVersion( std::istream &file, JavaHeader &header );
  void ReadMajorVersion( std::istream &file, JavaHeader &header );
  void ReadConstantPoolCount( std::istream &file, JavaHeader &header );
  void ReadConstants( std::istream &file,  JavaHeader &header );

};
