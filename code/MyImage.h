#ifndef _MYIMAGE_H_
#define _MYIMAGE_H_

#include "SOIL.h"

class Myimage
{
public:
  int w, h;
  unsigned char* buf;
  Myimage(string filename)
  {
    this->buf = SOIL_load_image(filename.c_str(), &this->w, &this->h, 0, SOIL_LOAD_RGB);
  }
  ~Myimage()
  {
    SOIL_free_image_data(this->buf);
  }
};

#endif // _MYIMAGE_H_
