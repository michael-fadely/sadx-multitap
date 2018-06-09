#include "Carry.h"
#include "multitap.h"

// from: https://github.com/nihilus/hexrays_tools/blob/master/code/defs.h
#define _HIBYTE(x)   (*((Uint8*)&(x)+1))

enum class CarryState : Uint8
{
	Invalid,
	Waiting,
	Carrying,
	Dropped
};

struct Carry
{
	CarryState state;
	EntityData1* target;
	float offset;
};

static const float RANGE = 16.0f;
static const short STATUS = Status_Ground | Status_Unknown1;

inline bool isValidState(EntityData1* entity) { return entity->Action == 15 && !(entity->Status & STATUS); }
inline float GetRange(NJS_VECTOR* target, NJS_VECTOR* parent)
{
	auto position = *target;
	njSubVector(&position, parent);
	return njScalor(&position);
}

static void __cdecl Carry_Main(ObjectMaster* object)
{
	Carry* data = (Carry*)object->Data2;
	EntityData1* parent = object->Parent->Data1;

	if (parent == nullptr)
		return;

	if (parent->CollisionInfo != nullptr)
	{
		// Same behavior as 2P Tails.
		// Can't deal damage to P1, can collide with other entities with these flags,
		// can't collide with P1.

		for (short i = 0; i < parent->CollisionInfo->Count; i++)
			_HIBYTE(parent->CollisionInfo->CollisionArray[i].field_2) &= 0xDFu;
	}

#ifdef _DEBUG
	CharObj2* data2 = *(CharObj2**)object->Parent->Data2;
	data2->TailsFlightTime = 0.0f;
#endif

	if (data->state == CarryState::Dropped && !(parent->Status & STATUS))
	{
		auto distance = GetRange(&data->target->Position, &parent->Position);
		if (distance < CharObj2Ptrs[data->target->CharIndex]->PhysicsData.CollisionSize ||
			Controllers[data->target->CharIndex].HeldButtons & AttackButtons)
			return;

		data->state = CarryState::Invalid;
	}
	else if (!isValidState(parent))
	{
		data->state = CarryState::Invalid;
	}

	object->DisplaySub(object);

	switch (data->state)
	{
		case CarryState::Invalid:
			if (!IsControllerEnabled(parent->CharIndex))
				break;

			data->target = nullptr;

			if (isValidState(parent))
				data->state = CarryState::Waiting;

			break;

		case CarryState::Waiting:
		{
			for (Uint8 i = 0; i < PLAYER_COUNT; i++)
			{
				if (i == parent->CharIndex)
					continue;

				auto target = EntityData1Ptrs[i];

				if (!target)
					continue;

				auto distance = GetRange(&target->Position, &parent->Position);
				if (distance <= RANGE && distance >= CharObj2Ptrs[i]->PhysicsData.CollisionSize)
				{
					data->state = CarryState::Carrying;
					data->target = target;
					target->Status &= ~(STATUS | Status_Attack | Status_Ball);
					data->offset = CharObj2Ptrs[i]->PhysicsData.YOff;
				}
			}

			break;
		}

		case CarryState::Carrying:
		{
			auto target = data->target;
			auto parent_data2 = CharObj2Ptrs[parent->CharIndex];
			auto target_data2 = CharObj2Ptrs[target->CharIndex];
			auto pressed = Controllers[target->CharIndex].PressedButtons;
			bool nope = false;

			// TODO: Figure out how to handle jumping off of Tails

			if (target->Status & (STATUS | Status_DoNextAction | Status_Hurt))
			{
				nope = true;
			}
			else if (pressed & AttackButtons)
			{
				target_data2->Speed.y = 0.0f;
				nope = true;
			}

			if (nope)
			{
				data->state = CarryState::Dropped;
				target_data2->PhysicsData.YOff = data->offset;
				data->offset = 0.0f;
				break;
			}

			target->Status &= ~Status_Attack;

			target->Position = parent->Position;
			target->Position.y -= parent_data2->PhysicsData.CollisionSize;
			target->Rotation = parent->Rotation;

			target_data2->Speed = parent_data2->Speed;

			// PhysicsData.YOff only seems to work on Sonic
			switch (static_cast<Characters>(target->CharID))
			{
				case Characters_Sonic:
					target_data2->AnimationThing.Index = 47;
					target_data2->PhysicsData.YOff = 7.0f;
					break;

				case Characters_Knuckles:
					target_data2->AnimationThing.Index = 84;
					break;

				case Characters_Amy:
					target_data2->AnimationThing.Index = 32;
					break;

				default:
					break;
			}

			break;
		}

		default:
			break;
	}
}

static void __cdecl Carry_Display(ObjectMaster* object)
{
#ifdef _DEBUG
	DrawColObj(object->Parent, 0);
#endif
}

static void __cdecl Carry_Delete(ObjectMaster* object)
{
	object->Parent = nullptr;
	delete (Carry*)object->Data2;
	object->Data2 = nullptr;
}

void Carry_Load(ObjectMaster* parent)
{
	auto object = LoadObject((LoadObj)0, 0, Carry_Main);
	if (object == nullptr)
		return;

	object->MainSub    = Carry_Main;
	object->DisplaySub = Carry_Display;
	object->DeleteSub  = Carry_Delete;
	object->Parent     = parent;
	object->Data2      = new Carry {};
}
