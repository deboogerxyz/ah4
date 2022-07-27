#include "config.h"
#include "keyBinds.h"

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

static void keyBindComboBox(struct nk_context *ctx, const char *name, int *value)
{
	nk_layout_row_dynamic(ctx, 25, 2);
	nk_label(ctx, name, NK_TEXT_LEFT);
	*value = nk_combo(ctx, keyBinds_keys, NK_LEN(keyBinds_keys), *value, 25, nk_vec2(200, 200));
}

static void renderBacktrackTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	nk_checkbox_label(ctx, "Enabled", (nk_bool *)&config.backtrack.enabled);
	nk_property_int(ctx, "Time limit [ms]", 0, &config.backtrack.timeLimit, 400, 1, 1);
}

static void renderMiscTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	nk_checkbox_label(ctx, "Jump bug", (nk_bool *)&config.misc.jumpBug);
	keyBindComboBox(ctx, "Jump bug key bind", &config.misc.jumpBugKeyBind.key);
	nk_checkbox_label(ctx, "Edge jump", (nk_bool *)&config.misc.edgeJump);
	keyBindComboBox(ctx, "Edge jump key bind", &config.misc.edgeJumpKeyBind.key);
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

	if (nk_button_label(ctx, "Save"))
		config_save(buf);
}

void gui_render(struct nk_context *ctx, SDL_Window *window)
{
	int flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR;

	if (nk_begin(ctx, "ah4", nk_rect(0, 0, 600, 500), flags)) {
		nk_layout_row_dynamic(ctx, 50, 3);

		if (nk_button_label(ctx, "Backtrack"))
			currentTab = 0;

		if (nk_button_label(ctx, "Misc"))
			currentTab = 1;

		if (nk_button_label(ctx, "Config"))
			currentTab = 2;

		switch (currentTab) {
		case 0: renderBacktrackTab(ctx); break;
		case 1: renderMiscTab(ctx); break;
		case 2: renderConfigTab(ctx); break;
		}

		nk_end(ctx);
	}

	nk_window_show(ctx, "ah4", gui_isOpen);
}
