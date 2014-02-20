#pragma once
#include "JavaHeader.h"
#include <fstream>
#include <string>
class Validation
{
public:
  Validation(unsigned int i, unsigned char type) : index(i), expectedType(type) {}
  Validation(unsigned int i, unsigned char type, std::string msg) : index(i), expectedType(type), message(msg) {}

  void Validate(const std::vector<cp_info *> &constants)
  {
    if (index < 1 || index >= constants.size())
    {
      throw std::runtime_error("Invalid index given - outside constant pool: Index " + std::to_string(index));
    }

    if (constants[index]->tag != expectedType)
    {
      throw std::runtime_error("Constant was not of expected type. Expected: " + 
        std::to_string(expectedType) + " actual: " + std::to_string(constants[index]->tag) +
        " Index: " + std::to_string(index) +  (message.length() > 0 ? " Message: " + message : ""));
    }

  }
private:
  unsigned int index;
  unsigned char expectedType;
  std::string message;
};


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

  void ReadConstants( std::istream &file,  JavaHeader &header);
  void ReadClassData( std::istream & file, JavaHeader & header, std::vector<Validation> &validations);
  void ReadRef( std::istream & file, JavaHeader & header, unsigned char tag, std::vector<Validation> &validations);  
  void ReadString( std::istream & file, JavaHeader & header, std::vector<Validation> &validations );
  void ReadInteger( std::istream & file, JavaHeader & header );
  void ReadFloat( std::istream & file, JavaHeader & header );
  void ReadLong( std::istream & file, JavaHeader & header );
  void ReadDouble( std::istream & file, JavaHeader & header );
  void ReadNameAndType( std::istream & file, JavaHeader & header, std::vector<Validation> &validations );
  void ReadUtf8( std::istream & file, JavaHeader & header);

  void ReadAccessFlags( std::istream &file, JavaHeader &header );
  void ReadThisClass( std::istream &file, JavaHeader &header );
  void ReadSuperClass( std::istream &file, JavaHeader &header );
  void ReadInterfaceCount( std::istream &file, JavaHeader &header );
  void ReadInterfaces( JavaHeader &header, std::istream &file );
  void ReadFieldCount( std::istream &file, JavaHeader &header );
  void ReadFields( JavaHeader &header, std::istream &file );

  void ReadFieldAttributes( field_info &field, std::istream &file );

};
