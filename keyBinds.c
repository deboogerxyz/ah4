#include "interfaces.h"

#include "keyBinds.h"

const char *keyBinds_keys[] = {
	"None",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Key pad insert",
	"Key pad end",
	"Key pad down arrow",
	"Key pad page down",
	"Key pad left arrow",
	"Key pad 5",
	"Key pad right arrow",
	"Key pad home",
	"Key pad up arrow",
	"Key pad page up",
	"Key pad slash",
	"Key pad multiply",
	"Key pad minus",
	"Key pad plus",
	"Key pad enter",
	"Key pad delete",
	"[",
	"]",
	";",
	"'",
	"`",
	",",
	".",
	"/",
	"\\",
	"-",
	"=",
	"Enter",
	"Space",
	"Backspace",
	"Tab",
	"Caps lock",
	"Num lock",
	"Escape",
	"Scroll lock",
	"Insert",
	"Delete",
	"Home",
	"End",
	"Page up",
	"Page down",
	"Pause",
	"Shift",
	"Right shift",
	"Alt",
	"Right alt",
	"Control",
	"Right control",
	"Super",
	"Right super",
	"App",
	"Up arrow",
	"Left arrow",
	"Down arrow",
	"Right arrow",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"Caps lock toggle",
	"Num lock toggle",
	"Scroll lock toggle",
	"Left click",
	"Right click",
	"Middle click",
	"Mouse 4",
	"Mouse 5",
	"Mouse wheel up",
	"Mouse wheel down"
};

const char *keyBinds_modes[] = {
	"On hold",
	"On toggle"
};

bool keyBinds_isOn(KeyBind *keyBind)
{
	if (!keyBind)
		return false;

	if (!keyBind->key)
		return true;

	bool down = interfaces.inputSystem->vmt->isButtonDown(interfaces.inputSystem, keyBind->key);

	switch (keyBind->mode) {
	case 0:
		keyBind->active = down;
		break;
	case 1:
		if (!keyBind->lastDown && down)
			keyBind->active ^= 1;
		break;
	}

	keyBind->lastDown = down;

	return keyBind->active;
}
