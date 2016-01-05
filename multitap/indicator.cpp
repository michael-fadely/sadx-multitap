#include <SADXModLoader.h>

FunctionPointer(void, Draw2DSprite, (NJS_SPRITE *_sp, Int n, Float pri, Uint32 attr, char zfunc_type), 0x00404660);
FunctionPointer(void, SetSpriteColor, (NJS_ARGB *a1), 0x00402F40);
FunctionPointer(Sint32, njLoadTexture, (NJS_TEXLIST *texlist), 0x0077FC80);
FunctionPointer(Bool, IsControllerEnabled, (Uint8 index), 0x0040EFD0);
FunctionPointer(char, IsVisible, (NJS_VECTOR *v), 0x004CD730);

NJS_TEXNAME multicommon_texname[] = {
	{ "arrow",	NJD_TEXATTR_BOTH, 0 },
	{ "cpu_1",	NJD_TEXATTR_BOTH, 0 },
	{ "cpu_2",	NJD_TEXATTR_BOTH, 0 },
	{ "p",		NJD_TEXATTR_BOTH, 0 },
	{ "p1",		NJD_TEXATTR_BOTH, 0 },
	{ "p2",		NJD_TEXATTR_BOTH, 0 },
	{ "p3",		NJD_TEXATTR_BOTH, 0 },
	{ "p4",		NJD_TEXATTR_BOTH, 0 },
};

NJS_TEXLIST multicommon_texlist = { arrayptrandlength(multicommon_texname) };

NJS_TEXANIM multicommon_texanim[] = {
	{ 24,	16,	12,	16,	0,	0,	0xFF,	0xFF,	0,	0 },	// arrow
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	1,	0 },	// cpu_1
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	2,	0 },	// cpu_2
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	3,	0 },	// p
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	4,	0 },	// p1
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	5,	0 },	// p2
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	6,	0 },	// p3
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	7,	0 }		// p4
};

NJS_SPRITE multicommon_sprite = { { 0.f, 0.f, 0.f }, 1.f, 1.f, 0, &multicommon_texlist, multicommon_texanim };

NJS_ARGB colors[] = {
	{ 1.000f, 0.000f, 0.000f, 1.000f }, // Sonic
	{ 1.000f, 0.500f, 0.000f, 0.000f }, // Eggman
	{ 1.000f, 1.000f, 0.804f, 0.000f }, // Tails
	{ 1.000f, 1.000f, 0.063f, 0.000f }, // Knuckles
	{ 1.000f, 1.000f, 0.545f, 0.322f }, // Tikal
	{ 1.000f, 1.000f, 0.545f, 0.741f }, // Amy
	{ 1.000f, 0.545f, 0.545f, 0.545f }, // Gamma
	{ 1.000f, 0.451f, 0.192f, 0.804f }, // Big
	{ 1.000f, 0.000f, 1.000f, 1.000f }, // Metal Sonic
	{ 0.750f, 0.500f, 0.500f, 0.500f }
};

enum AnimIndex
{
	arrow, cpu_1, cpu_2,
	p, p1, p2, p3, p4
};

void LoadIndicators()
{
	LoadPVM("multicommon", &multicommon_texlist);
}

void DrawElement(Uint32 i, Uint32 n)
{
	CharObj1* player = CharObj1Ptrs[i];

	if (player == nullptr)
		return;

	NJS_SPRITE* sp = &multicommon_sprite;
	NJS_VECTOR pos = player->Position;
	Uint8 charid = (MetalSonicFlag) ? Characters_MetalSonic : player->CharID;
	pos.y += PhysicsArray[charid].CollisionSize;
	njProjectScreen(nullptr, &pos, (NJS_POINT2*)&sp->p);

	bool isVisible = sp->p.x - sp->tanim[i].sx < HorizontalResolution
		|| sp->p.x + sp->tanim[i].sx > 0
		|| sp->p.y - sp->tanim[i].sy < VerticalResolution
		|| sp->p.y + sp->tanim[i].sy > 0;

	if (!isVisible)
	{
		// Do off-screen pointing thing here
		return;
	}
	else
	{
		SetSpriteColor(IsControllerEnabled(i) ? &colors[charid] : &colors[9]);
		Draw2DSprite(&multicommon_sprite, n, -1.0f, NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA, 0);
	}
}

void DrawIndicators()
{
	njSetTexture(&multicommon_texlist);
	njSetTextureNum(arrow);	// arrow

	for (Uint32 i = 0; i < 4; i++)
		DrawElement(i, arrow);

	for (Uint32 i = 0; i < 4; i++)
	{
		AnimIndex a = IsControllerEnabled(i) ? p : cpu_1;
		njSetTextureNum(a);
		DrawElement(i, a);
	}

	for (Uint32 i = 0; i < 4; i++)
	{
		AnimIndex a = IsControllerEnabled(i) ? (AnimIndex)(p1 + i) : cpu_2;
		njSetTextureNum(a);
		DrawElement(i, a);
	}
}

