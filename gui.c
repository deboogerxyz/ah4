#include "gui.h"

bool gui_isOpen = 1;

void gui_handleToggle(struct nk_context *ctx)
{
	struct nk_input *in = &ctx->input;

	if (nk_input_is_key_released(in, NK_KEY_TEXT_START)) {
		gui_isOpen ^= 1;
		ctx->style.cursor_visible = gui_isOpen;
	}
}

void gui_render(struct nk_context *ctx, SDL_Window *window)
{
	int flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR;

	if (nk_begin(ctx, "ah4", nk_rect(0, 0, 200, 200), flags)) {
		/*
                nk_layout_row_dynamic(ctx, 30, 3);

		nk_button_label(ctx, "test");
		nk_button_label(ctx, "test");
		nk_button_label(ctx, "test");
		*/

		nk_end(ctx);
	}

	nk_window_show(ctx, "ah4", gui_isOpen);
}
