#include <SADXModLoader.h>
#include "multitap.h"
#include "indicator.h"
#include "Carry.h"

DataArray(void*, EntityData2Ptrs, 0x3B36DD0, 8);
DataPointer(ObjectMaster*, TailsAI_ptr, 0x03B2B358);

static bool redirect = false;
static ObjectMaster* LastTailsAI_ptr = nullptr;

void Teleport(uint8_t to, uint8_t from)
{
	if (CharObj1Ptrs[to] == nullptr || CharObj1Ptrs[from] == nullptr)
		return;

	CharObj1Ptrs[from]->Position = CharObj1Ptrs[to]->Position;
	CharObj1Ptrs[from]->Rotation = CharObj1Ptrs[to]->Rotation;

	if (CharObj2Ptrs[from] != nullptr)
		CharObj2Ptrs[from]->Speed = {};

	CharObj1Ptrs[from]->Action = 0;
	CharObj1Ptrs[from]->Status &= ~Status_Attack;
}

extern "C"
{
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

	__declspec(dllexport) void __cdecl Init()
	{
		// Enables WriteAnalogs for controllers >= 2 (3)
		Uint8 patch[3] = { 0x83u, 0xFFu, 0x04u };
		WriteData((void*)0x0040F180, (void*)patch, sizeof(char) * 3);
		
		// Object patches
		WriteData((Uint8*)0x007A4DC4, PLAYER_COUNT); // Spring_Main
		WriteData((Uint8*)0x007A4FF7, PLAYER_COUNT); // SpringB_Main
		WriteData((Uint8*)0x0079F77C, PLAYER_COUNT); // SpringH_Main
		WriteData((Uint8*)0x004418B8, PLAYER_COUNT); // IsPlayerInsideSphere (could probably use a better name!)

		InitSprites();
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_D)
		{
			redirect = !redirect;
			*ControllerPointers[1] = {};
		}

		if (redirect)
		{
			*ControllerPointers[1] = *ControllerPointers[0];
			*ControllerPointers[0] = {};
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (GameState < 4 || GameState > 16 || LoadingFile)
			return;

		if (LastTailsAI_ptr != TailsAI_ptr)
		{
			if (PlayerPtrs[1] != nullptr)
			{
				if (TailsAI_ptr != nullptr)
					Carry_Load(PlayerPtrs[1]);

				LastTailsAI_ptr = TailsAI_ptr;
			}
		}

		InitIndicators();

		for (Uint8 i = 0; i < PLAYER_COUNT; i++)
		{
			if (GetCharObj2(i) != nullptr && ControllerPointers[i]->HeldButtons & Buttons_C)
			{
				int buttons = ControllerPointers[i]->PressedButtons;

				if ((buttons & (Buttons_Up | Buttons_Down | Buttons_Left | Buttons_Right)) == 0)
					continue;

				if (buttons & Buttons_Up)
					Teleport(0, i);
				else if (buttons & Buttons_Down)
					Teleport(1, i);
				else if (buttons & Buttons_Left)
					Teleport(2, i);
				else if (buttons & Buttons_Right)
					Teleport(3, i);

				continue;
			}

			if (i == 0 /*|| i == 1*/)
				continue;

			if (ControllerPointers[i]->HeldButtons & Buttons_Y && GetCharObj2(i) == nullptr)
			{
				void(__cdecl* loadSub)(ObjectMaster*);
				uint8_t charid;
				int buttons = ControllerPointers[i]->PressedButtons;
				bool alt = (ControllerPointers[i]->HeldButtons & Buttons_Z) != 0;

				if (buttons & Buttons_Up)
				{
					loadSub = alt ? Big_Main : Sonic_Main;
					charid = alt ? Characters_Big : Characters_Sonic;
				}
				else if (buttons & Buttons_Down)
				{
					loadSub = alt ? Gamma_Main : Tails_Main;
					charid = alt ? Characters_Gamma : Characters_Tails;
				}
				else if (buttons & Buttons_Left)
				{
					loadSub = alt ? Tikal_Main : Knuckles_Main;
					charid = alt ? Characters_Tikal : Characters_Knuckles;
				}
				else if (buttons & Buttons_Right)
				{
					loadSub = alt ? Eggman_Main : Amy_Main;
					charid = alt ? Characters_Eggman : Characters_Amy;
				}
				else
				{
					continue;
				}

				ObjectMaster* object = LoadObject((LoadObj)7, 1, loadSub);
				object->Data1->CharID = charid;
				object->Data1->CharIndex = i;

				if (charid == Characters_Tails)
				{
					Carry_Load(object);
				}

				CharObj1Ptrs[i] = object->Data1;
				CharObj2Ptrs[i] = ((EntityData2*)object->Data2)->CharacterData;
				PlayerPtrs[i] = object;
				EntityData2Ptrs[i] = object->Data2;
				InitCharacterVars(i, object);

				EnableController(i);

				PutPlayerAtStartPointIGuess(object->Data1);
			}
		}
	}

	__declspec(dllexport) void __cdecl OnControl()
	{
		if (redirect)
			*(float*)0x03B0E7A4 = 0.0f;

		for (int i = 2; i < PLAYER_COUNT; i++)
		{
			if (!IsControllerEnabled(i))
			{
				memset(&Controllers[i], 0, sizeof(ControllerData));
				continue;
			}

			memcpy(&Controllers[i], ControllerPointers[i], sizeof(ControllerData));
		}
	}
}
