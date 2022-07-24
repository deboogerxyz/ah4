#include <stdbool.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "libs/nuklear.h"
#include "libs/nuklear_sdl_gl3.h"

extern bool gui_isOpen;

void gui_handleToggle(struct nk_context *ctx);
void gui_render(struct nk_context *, SDL_Window *);
