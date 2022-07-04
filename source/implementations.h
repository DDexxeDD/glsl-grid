#ifndef _implementations_h
#define _implementations_h

/*
 * this file needs to be included _BEFORE_ an inclusion of nuklear.h
 * 	or we will get compilation warnings about nk_labelf (NK_INCLUDE_STANDARD_VARARGS)
 */

// nuklear flags
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS

#endif
