#include "Carry.h"
#include "multitap.h"

enum class CarryState : Uint8
{
	Invalid,
	Waiting,
	Carrying
};

struct Carry
{
	CarryState state;
	bool dropped;
	ObjectMaster* parent;
	EntityData1* target;
};

static const float RANGE = 16.0f;

inline bool isValidState(EntityData1* entity) { return entity->Action == 15 && !(entity->Status & Status_Ground); }

static void __cdecl Carry_Main(ObjectMaster* object)
{
	Carry* data = (Carry*)object->Data2;

	EntityData1* parent = data->parent->Data1;

	if (data->dropped || !isValidState(parent))
		data->state = CarryState::Invalid;

	switch (data->state)
	{
		case CarryState::Invalid:
			data->target = nullptr;

			if (parent->Status & Status_Ground && data->dropped)
				data->dropped = false;
			else if (isValidState(parent))
				data->state = CarryState::Waiting;

			break;

		case CarryState::Waiting:
		{
			for (Uint8 i = 0; i < PLAYER_COUNT; i++)
			{
				if (i == parent->CharIndex)
					continue;

				auto target = CharObj1Ptrs[i];

				if (!target/* || t1->Status & Status_Ground*/)
					continue;

				NJS_VECTOR position = target->Position;
				njSubVector(&position, &parent->Position);
				float distance = njScalor(&position);

				if (distance <= RANGE)
				{
					data->state = CarryState::Carrying;
					data->target = target;
					target->Action = 1;
					target->Status &= ~Status_Ground;
				}
			}

			break;
		}

		case CarryState::Carrying:
		{
			EntityData1* target = data->target;
			if (target->Status & Status_Ground)
			{
				data->state = CarryState::Invalid;
				data->dropped = true;
				break;
			}

			auto parent_data2 = CharObj2Ptrs[parent->CharIndex];
			auto target_data2 = CharObj2Ptrs[target->CharIndex];

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

static void __cdecl Carry_Delete(ObjectMaster* object)
{
	delete (Carry*)object->Data2;
}

void Carry_Load(ObjectMaster* parent)
{
	auto object = LoadObject((LoadObj)0, 0, Carry_Main);
	if (object == nullptr)
		return;

	object->MainSub = Carry_Main;
	object->DeleteSub = Carry_Delete;

	auto data = new Carry{};
	data->parent = parent;

	object->Data2 = data;
}
