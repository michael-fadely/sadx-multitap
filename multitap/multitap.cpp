#include <SADXModLoader.h>

FunctionPointer(void, sub_414810, (CharObj1*), 0x414810);
FunctionPointer(void, WriteAnalogs, (void), 0x0040F170);
FunctionPointer(void, UpdateController, (int index), 0x0040F070);

DataArray(ControllerData, ControllersStatic, 0x03B0E7F0, 8);
DataArray(ControllerData*, ControllerPointers, 0x03B0E77C, 8);
DataArray(bool, Controller_Enabled, 0x00909FB4, 4);
DataArray(void*, dword_3B36DD0, 0x3B36DD0, 8);

DataPointer(int,	AttackButtons,	0x00909F94);
DataPointer(int,	GrabButtons,	0x00909FA4);
DataPointer(bool,	ControlEnabled,	0x00909FB0);

void __cdecl Control_hook();

extern "C"
{
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
	__declspec(dllexport) void Init()
	{
		WriteJump(Control, Control_hook);
		char patch[3] = { 0x83u, 0xFFu, 0x04u };
		WriteData((void*)0x0040F180, (void*)patch, sizeof(char) * 3);
	}
	__declspec(dllexport) void OnFrame()
	{
		for (uint8_t i = 1; i < 4; i++)
		{
			if (GetCharObj2(i) != nullptr && ControllersRaw[i].HeldButtons & Buttons_D)
			{
				if ((ControllersRaw[i].HeldButtons & (Buttons_Up | Buttons_Down | Buttons_Left | Buttons_Right)) == 0)
					continue;

				CharObj1Ptrs[i]->Position = CharObj1Ptrs[0]->Position;

				int buttons = ControllersRaw[i].HeldButtons;
				int x = 0;

				if (buttons & Buttons_Up)
					x = 0;
				else if (buttons & Buttons_Down)
					x = 1;
				else if (buttons & Buttons_Left)
					x = 2;
				else if (buttons & Buttons_Right)
					x = 3;

				continue;
			}

			if (ControllersRaw[i].HeldButtons & Buttons_Y && GetCharObj2(i) == nullptr)
			{
				void(__cdecl* loadSub)(ObjectMaster*);
				uint8_t charid = 0;
				int buttons = ControllersRaw[i].PressedButtons;

				if (buttons & Buttons_Up)
				{
					loadSub = Sonic_Main;
					charid = Characters_Sonic;
				}
				else if (buttons & Buttons_Down)
				{
					loadSub = Tails_Main;
					charid = Characters_Tails;
				}
				else if (buttons & Buttons_Left)
				{
					loadSub = Knuckles_Main;
					charid = Characters_Knuckles;
				}
				else if (buttons & Buttons_Right)
				{
					loadSub = Amy_Main;
					charid = Characters_Amy;
				}
				else
				{
					continue;
				}

				ObjectMaster* object = LoadObject(7, i + 1, loadSub);
				object->Data1->CharID = charid;
				object->Data1->CharIndex = i;

				CharObj1Ptrs[i] = object->Data1;
				CharObj2Ptrs[i] = object->Data1->Ptr2;	// I know this is in ObjectMaster, but I'm playing it safe since it's not defined.
				PlayerPtrs[i] = object;
				dword_3B36DD0[i] = (void*)object->field_24;

				Controller_Enabled[i] = true;

				sub_414810(object->Data1);
			}
		}
	}
}

void __cdecl Control_hook()
{
	if (IsLevelChaoGarden() == 1)
	{
		AttackButtons = Buttons_B;
		GrabButtons = Buttons_X | Buttons_B;
	}
	else
	{
		AttackButtons = Buttons_X | Buttons_B;
		GrabButtons = Buttons_X | Buttons_B;
	}

	if (!ControlEnabled)
	{
		memset(&ControllersStatic[0], 0, sizeof(ControllerData) * ControllersStatic_Length);
		WriteAnalogs();
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		if (!Controller_Enabled[i])
		{
			memset(&ControllersStatic[i], 0, sizeof(ControllerData));
			continue;
		}

		memcpy(&ControllersStatic[i], ControllerPointers[i], sizeof(ControllerData));
	}

	WriteAnalogs();

	for (int i = 0; i < 4; i++)
		UpdateController(i);
}