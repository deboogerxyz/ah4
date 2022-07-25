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
	char **configs = 0;
	int len = config_getConfigs(&configs);
	static char buf[256];

	nk_layout_row_dynamic(ctx, 250, 1);
	if (nk_group_begin(ctx, "Config", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_DYNAMIC)) {
		static int selected = -1;
		nk_layout_row_dynamic(ctx, 250, 1);
		if (nk_group_begin(ctx, "Configs", 0)) {
			nk_layout_row_static(ctx, 18, 100, 1);
			for (int i = 0; i < len; i++)
				if (nk_select_label(ctx, configs[i], NK_TEXT_LEFT, 0)) {
					selected = (selected == 0) ? -1 : 0;
					strncpy(buf, configs[i], 255);
				}

			nk_group_end(ctx);
		}
		nk_group_end(ctx);
	}

	while (len)
		free(configs[--len]);
	free(configs);

	nk_layout_row_dynamic(ctx, 25, 1);
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, buf, sizeof(buf), nk_filter_ascii);

	if (nk_button_label(ctx, "Load"))
		config_load(buf);
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
