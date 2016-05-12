#ifndef CHECKER_IS_INCLUDED
#define CHECKER_IS_INCLUDED

// Note: OpenGL ES only takes 2^n times 2^n texture.
const int checker_pattern_wid=8;
const int checker_pattern_hei=8;
extern const unsigned char checker_pattern[checker_pattern_wid*checker_pattern_hei*4];

#endif
