#include <SADXModLoader.h>

FunctionPointer(void, sub_414810, (CharObj1*), 0x414810);

DataArray(ControllerData, ControllersStatic, 0x03B0E7F0, 8);
DataArray(bool, Controller_Enabled, 0x00909FB4, 4);
DataArray(void*, dword_3B36DD0, 0x3B36DD0, 8);

extern "C"
{
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
	__declspec(dllexport) void OnFrame()
	{
		for (uint8_t i = 1; i < 4; i++)
		{
			if (GetCharObj2(i) == nullptr)
			{
				if (ControllersRaw[i].HeldButtons & Buttons_Y)
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
}
