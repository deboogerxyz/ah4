#include "interfaces.h"

#include "keyBinds.h"

bool keyBinds_isDown(int keyCode)
{
	int buttonCode = interfaces.inputSystem->vmt->virtualKeyToButtonCode(interfaces.inputSystem, keyCode);

	return interfaces.inputSystem->vmt->isButtonDown(interfaces.inputSystem, buttonCode);
}
