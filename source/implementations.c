/*
 * sokol and nuklear are single header libraries that need to be compiled
 * that compilation takes a noticable amount of time
 * 	we compile them here in this file so that they almost never get recompiled
 */
#include "implementations.h"

#define SOKOL_IMPL
#ifndef SOKOL_GLCORE33
#define SOKOL_GLCORE33
#endif
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#include "sokol_fetch.h"

// include nuklear.h before the sokol_nuklear.h implementation
#define NK_IMPLEMENTATION
#include "nuklear.h"
#undef NK_IMPLEMENTATION

#define SOKOL_NUKLEAR_IMPL
#include "sokol_nuklear.h"
#undef SOKOL_NUKLEAR_IMPL
#undef SOKOL_IMPL
