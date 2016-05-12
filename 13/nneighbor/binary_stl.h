#ifndef BINARY_STL_IS_INCLUDED
#define BINARY_STL_IS_INCLUDED

#include "vector"

bool IsAsciiStl(const char fn[]);
void LoadBinaryStl(std::vector <float> &vtx,std::vector <float> &nom,const char fn[]);

#endif
