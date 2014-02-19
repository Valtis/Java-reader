#include "HeaderReader.h"
#include <fstream>
#include <iostream>

int main()
{
  std::ifstream file("main.class", std::ios::binary);
  HeaderReader h;
  JavaHeader header = h.ReadHeader(file);

 /* for (int i = 31; i >= 0; --i)
  {
    std::cout << ((JAVA_MAGIC_NUMBER & (1 << i)) != 0 ? 1 : 0);
  }
  std::cout << "\n";
  for (int i = 31; i >= 0; --i)
  {
    std::cout << ((header.magic_number & (1 << i)) == 0 ? 0 : 1);
  }
  std::cout << "\n";*/

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

  system("pause");

}