#include "config.h"

#include "gui.h"

static int currentTab = 0;

bool gui_isOpen = 1;

void gui_handleToggle(struct nk_context *ctx)
{
	struct nk_input *in = &ctx->input;

	if (nk_input_is_key_released(in, NK_KEY_TEXT_START)) {
		gui_isOpen ^= 1;
		ctx->style.cursor_visible = gui_isOpen;
	}
}

static void renderConfigTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 500, 1);
	if (nk_group_begin(ctx, "Config", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_DYNAMIC)) {
		nk_layout_row_dynamic(ctx, 25, 1);

		static char buf[256];
		nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, buf, sizeof(buf), nk_filter_ascii);

		if (nk_button_label(ctx, "Load"))
			config_load(buf);

		nk_group_end(ctx);
	}
}

void gui_render(struct nk_context *ctx, SDL_Window *window)
{
	int flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR;

	if (nk_begin(ctx, "ah4", nk_rect(0, 0, 600, 500), flags)) {
		nk_layout_row_dynamic(ctx, 50, 1);

		if (nk_button_label(ctx, "Config"))
			currentTab = 0;

		switch (currentTab) {
		case 0: renderConfigTab(ctx);
		}

		nk_end(ctx);
	}

	nk_window_show(ctx, "ah4", gui_isOpen);
}
