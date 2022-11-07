#pragma once
#include <string>

class ImageWriter {
 public:
  void savePixel();
  void writeToFile(std::string filename);
};
