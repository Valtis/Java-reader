#pragma once

const unsigned char CONSTANT_Class = 7;
const unsigned char CONSTANT_Fieldref =	9;
const unsigned char CONSTANT_Methodref =	10;
const unsigned char CONSTANT_InterfaceMethodref =	11;
const unsigned char CONSTANT_String =	8;
const unsigned char CONSTANT_Integer = 	3;
const unsigned char CONSTANT_Float =	4;
const unsigned char CONSTANT_Long =	5;
const unsigned char CONSTANT_Double =	6;
const unsigned char CONSTANT_NameAndType = 	12;
const unsigned char CONSTANT_Utf8 =	1;




struct cp_pool
{
  unsigned char tag;

};
