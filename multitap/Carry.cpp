#include "Carry.h"
#include "multitap.h"

enum class CarryState : Uint8
{
	Initialize,
	Invalid,
	Waiting,
	Carrying,
	Dropped
};

struct Carry
{
	CarryState state;
	EntityData1* target;
};

static const float RANGE = 16.0f;

inline bool isValidState(EntityData1* entity) { return entity->Action == 15 && !(entity->Status & Status_Ground); }
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

	if (data->state == CarryState::Initialize)
	{
		if (parent->CollisionInfo == nullptr)
			return;

		// Same behavior as 2P Tails.
		// Can't deal damage to P1, can collide with other entities with these flags,
		// can't collide with P1.
		for (short i = 0; i < parent->CollisionInfo->Count; i++)
			parent->CollisionInfo->CollisionArray[i].field_2 &= 0xDF00;

		data->state = CarryState::Invalid;
	}

	if (data->state == CarryState::Dropped && !(parent->Status & Status_Ground))
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

				auto target = CharObj1Ptrs[i];

				if (!target)
					continue;

				auto distance = GetRange(&target->Position, &parent->Position);
				if (distance <= RANGE && distance >= CharObj2Ptrs[i]->PhysicsData.CollisionSize)
				{
					data->state = CarryState::Carrying;
					data->target = target;
					target->Status &= ~Status_Ground;
				}
			}

			break;
		}

		case CarryState::Carrying:
		{
			EntityData1* target = data->target;
			auto parent_data2 = CharObj2Ptrs[parent->CharIndex];
			auto target_data2 = CharObj2Ptrs[target->CharIndex];

			if (target->Status & Status_Ground)
			{
				data->state = CarryState::Dropped;
				break;
			}
			if (Controllers[target->CharIndex].PressedButtons & AttackButtons)
			{
				target_data2->Speed.y = 0.0f;
				data->state = CarryState::Dropped;
				break;
			}

			target->Status &= ~Status_Attack;

			target->Position = parent->Position;
			target->Position.y -= parent_data2->PhysicsData.CollisionSize;
			target->Rotation = parent->Rotation;

			target_data2->Speed = parent_data2->Speed;
			break;
		}

		default:
			break;
	}
}

static void __cdecl Carry_Display(ObjectMaster* object)
{
	Carry* data = (Carry*)object->Data2;
	DrawColObj(object->Parent, 0);
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

	object->MainSub = Carry_Main;
	object->DisplaySub = Carry_Display;
	object->DeleteSub = Carry_Delete;
	object->Parent = parent;
	object->Data2 = new Carry{};
}
