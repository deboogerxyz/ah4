#include <SDL2/SDL_mouse.h>

#include "hacks/skinChanger.h"

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
		SDL_ShowCursor(!gui_isOpen);
	}
}

static void setStyle(struct nk_context *ctx)
{
	struct nk_color table[NK_COLOR_COUNT];

	table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
	table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 240);
	table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 220);
	table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
	table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
	table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
	table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
	table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
	table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
	table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
	table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
	table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
	table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
	table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
	table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
	table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
	table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
	table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
	table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 220);

	nk_style_from_table(ctx, table);
}

static void colorAPicker(struct nk_context *ctx, struct nk_colorf *colorA)
{
	nk_layout_row_static(ctx, 25, 200, 1);
	if (nk_combo_begin_color(ctx, nk_rgba_cf(*colorA), nk_vec2(nk_widget_width(ctx), 400))) {
		nk_layout_row_dynamic(ctx, 120, 1);
		*colorA = nk_color_picker(ctx, *colorA, NK_RGBA);

		nk_layout_row_dynamic(ctx, 25, 1);
		colorA->r = nk_propertyf(ctx, "#R:", 0, colorA->r, 1.0f, 0.01f, 0.005f);
		colorA->g = nk_propertyf(ctx, "#G:", 0, colorA->g, 1.0f, 0.01f, 0.005f);
		colorA->b = nk_propertyf(ctx, "#B:", 0, colorA->b, 1.0f, 0.01f, 0.005f);
		colorA->a = nk_propertyf(ctx, "#A:", 0, colorA->a, 1.0f, 0.01f, 0.005f);

		nk_combo_end(ctx);
	}
}

static void keyBind(struct nk_context *ctx, const char *name, KeyBind *keyBind)
{
	nk_layout_row_dynamic(ctx, 25, 3);
	nk_label(ctx, name, NK_TEXT_LEFT);
	keyBind->key = nk_combo(ctx, keyBinds_keys, NK_LEN(keyBinds_keys), keyBind->key, 25, nk_vec2(200, 200));
	keyBind->mode = nk_combo(ctx, keyBinds_modes, NK_LEN(keyBinds_modes), keyBind->mode, 25, nk_vec2(200, 200));
}

void renderLegitbotTab(struct nk_context *ctx)
{
	const char *categories[] = {"Pistols", "Rifles", "AWP", "Scout", "SMGs", "Shotguns"};

	for (int i = 0; i < LegitbotCategory_Len; i++)
		if (nk_tree_push_id(ctx, NK_TREE_NODE, categories[i], NK_MINIMIZED, i)) {
			nk_checkbox_label(ctx, "Enabled", &config.legitbot[i].enabled);
			nk_checkbox_label(ctx, "Silent", &config.legitbot[i].silent);
			nk_checkbox_label(ctx, "Visible check", &config.legitbot[i].visibleCheck);
			nk_checkbox_label(ctx, "Smoke check", &config.legitbot[i].smokeCheck);
			nk_checkbox_label(ctx, "Flash check", &config.legitbot[i].flashCheck);
			if (i > 0 && i < 4)
				nk_checkbox_label(ctx, "Scope check", &config.legitbot[i].scopeCheck);
			nk_property_float(ctx, "#FOV:", 0.0f, &config.legitbot[i].fov, 255.0f, 0.025f, 0.025f);
			if (!config.legitbot[i].silent)
				nk_property_float(ctx, "#Smooth:", 1.0f, &config.legitbot[i].smooth, 100.0f, 0.1f, 0.1f);

			const char *bones[] = {"Head", "Neck", "Sternum", "Chest", "Stomach", "Pelvis"};

			for (int j = 0; j < 6; j++)
				nk_checkbox_label(ctx, bones[j], &config.legitbot[i].bones[j]);

			nk_tree_pop(ctx);
		}
}

static void renderBacktrackTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	nk_checkbox_label(ctx, "Enabled", &config.backtrack.enabled);
	nk_property_int(ctx, "Time limit [ms]", 0, &config.backtrack.timeLimit, 400, 1, 1);
}

static void renderGlowTab(struct nk_context *ctx)
{
	const char *categories[] = {"Enemies", "Teammates", "Dropped C4", "Planted C4", "Projectiles", "Dropped weapons"};

	for (int i = 0; i < GlowCategory_Len; i++)
		if (nk_tree_push_id(ctx, NK_TREE_NODE, categories[i], NK_MINIMIZED, i)) {
			nk_checkbox_label(ctx, "Enabled", &config.glow[i].enabled);
			if (i <= GlowCategory_Teammates)
				nk_checkbox_label(ctx, "Health based", &config.glow[i].healthBased);
			colorAPicker(ctx, (struct nk_colorf *)&config.glow[i].colorA);

			nk_tree_pop(ctx);
		}
}

static void renderVisualsTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	nk_checkbox_label(ctx, "Disable post-processing", &config.visuals.disablePostProcessing);
	nk_checkbox_label(ctx, "Disable shadows", &config.visuals.disableShadows);
	nk_checkbox_label(ctx, "Force crosshair", &config.visuals.forceCrosshair);
	nk_checkbox_label(ctx, "Grenade prediction", &config.visuals.grenadePrediction);
	nk_checkbox_label(ctx, "Reveal radar", &config.visuals.revealRadar);
	nk_checkbox_label(ctx, "Reveal ranks", &config.visuals.revealRanks);
	nk_checkbox_label(ctx, "Reveal Overwatch", &config.visuals.revealOverwatch);
}

static void renderMiscTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	nk_checkbox_label(ctx, "Anti AFK kick", &config.misc.antiAfkKick);
	nk_checkbox_label(ctx, "Bunny hop", &config.misc.bunnyHop);
	nk_checkbox_label(ctx, "Jump bug", &config.misc.jumpBug);
	keyBind(ctx, "Jump bug key bind", &config.misc.jumpBugKeyBind);
	nk_checkbox_label(ctx, "Edge jump", &config.misc.edgeJump);
	keyBind(ctx, "Edge jump key bind", &config.misc.edgeJumpKeyBind);
	nk_checkbox_label(ctx, "Fix movement", &config.misc.fixMovement);
}

static void renderSkinsTab(struct nk_context *ctx)
{
	nk_layout_row_dynamic(ctx, 25, 1);
	if (nk_button_label(ctx, "Force update"))
		skinChanger_forceUpdate();
}

static bool confirmationPopUp(struct nk_context *ctx, const char *confirmText, bool *popupActive)
{
	bool result = false;

	if (!popupActive || !*popupActive)
		return false;

	if (nk_popup_begin(ctx, NK_POPUP_DYNAMIC, "Confirmation pop up", NK_WINDOW_NO_SCROLLBAR, nk_rect(100, 100, 200, 100))) {
		nk_layout_row_dynamic(ctx, 25, 1);
		nk_label(ctx, "Are you sure?", NK_TEXT_CENTERED);

		nk_layout_row_dynamic(ctx, 30, 2);
		if (nk_button_label(ctx, confirmText)) {
			result = true;
			*popupActive = false;
			nk_popup_close(ctx);
		}

		if (nk_button_label(ctx, "Cancel")) {
			*popupActive = false;
			nk_popup_close(ctx);
		}

		nk_popup_end(ctx);
	} else {
		*popupActive = false;
	}

	return result;
}

static void renderConfigTab(struct nk_context *ctx)
{
	char **configs = 0;
	int len = config_getConfigs(&configs);
	static char buf[256];

	nk_layout_row_dynamic(ctx, 250, 1);
	if (nk_group_begin(ctx, "Config", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_DYNAMIC)) {
		nk_layout_row_dynamic(ctx, 250, 1);
		if (nk_group_begin(ctx, "Configs", 0)) {
			nk_layout_row_static(ctx, 18, 100, 1);
			for (int i = 0; i < len; i++)
				if (nk_select_label(ctx, configs[i], NK_TEXT_LEFT, !strncmp(configs[i], buf, 256)))
					strncpy(buf, configs[i], 255);

			nk_group_end(ctx);
		}
		nk_group_end(ctx);
	}

	while (len)
		free(configs[--len]);
	free(configs);

	nk_layout_row_dynamic(ctx, 25, 1);
	nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, buf, sizeof(buf), nk_filter_ascii);

	static bool loadPopupActive = false;
	if (*buf && nk_button_label(ctx, "Load"))
		loadPopupActive = true;

	if (confirmationPopUp(ctx, "Load", &loadPopupActive))
		config_load(buf);

	static bool savePopupActive = false;
	if (*buf && nk_button_label(ctx, "Save"))
		savePopupActive = true;

	if (confirmationPopUp(ctx, "Save", &savePopupActive))
		config_save(buf);

	static bool resetPopupActive = false;
	if (nk_button_label(ctx, "Reset"))
		resetPopupActive = true;

	if (confirmationPopUp(ctx, "Reset", &resetPopupActive))
		config_reset();

	if (nk_button_label(ctx, "Open config directory"))
		config_openDirectory();
}

static struct nk_style_button setButtonStyle(struct nk_context *ctx, int tab)
{
	struct nk_style_button oldStyle = ctx->style.button;

	if (tab != currentTab)
		return oldStyle;

	ctx->style.button.normal = oldStyle.active;
	ctx->style.button.hover = oldStyle.active;

	return oldStyle;
}

void gui_render(struct nk_context *ctx, SDL_Window *window)
{
	int flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_NO_SCROLLBAR;

	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	const float w = 600;
	const float h = 500;

	float x = (float)windowWidth / 2 - w / 2;
	float y = (float)windowHeight / 2 - h / 2;

	setStyle(ctx);

	if (nk_begin(ctx, "ah4", nk_rect(x, y, w, h), flags)) {
		nk_layout_row_dynamic(ctx, 50, 7);

		struct nk_style_button oldStyle = setButtonStyle(ctx, 0);
		if (nk_button_label(ctx, "Legitbot"))
			currentTab = 0;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 1);
		if (nk_button_label(ctx, "Backtrack"))
			currentTab = 1;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 2);
		if (nk_button_label(ctx, "Glow"))
			currentTab = 2;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 3);
		if (nk_button_label(ctx, "Visuals"))
			currentTab = 3;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 4);
		if (nk_button_label(ctx, "Misc"))
			currentTab = 4;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 5);
		if (nk_button_label(ctx, "Skins"))
			currentTab = 5;
		ctx->style.button = oldStyle;

		oldStyle = setButtonStyle(ctx, 6);
		if (nk_button_label(ctx, "Config"))
			currentTab = 6;
		ctx->style.button = oldStyle;

		switch (currentTab) {
		case 0: renderLegitbotTab(ctx); break;
		case 1: renderBacktrackTab(ctx); break;
		case 2: renderGlowTab(ctx); break;
		case 3: renderVisualsTab(ctx); break;
		case 4: renderMiscTab(ctx); break;
		case 5: renderSkinsTab(ctx); break;
		case 6: renderConfigTab(ctx); break;
		}

		nk_end(ctx);
	}
}
