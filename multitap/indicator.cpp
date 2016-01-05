#include <SADXModLoader.h>

NJS_TEXNAME multicommon_TEXNAME[] = {
	{ "arrow",	NJD_TEXATTR_BOTH, 0 },
	{ "cpu_1",	NJD_TEXATTR_BOTH, 0 },
	{ "cpu_2",	NJD_TEXATTR_BOTH, 0 },
	{ "p",		NJD_TEXATTR_BOTH, 0 },
	{ "p1",		NJD_TEXATTR_BOTH, 0 },
	{ "p2",		NJD_TEXATTR_BOTH, 0 },
	{ "p3",		NJD_TEXATTR_BOTH, 0 },
	{ "p4",		NJD_TEXATTR_BOTH, 0 }
};

NJS_TEXLIST multicommon_TEXLIST = { arrayptrandlength(multicommon_TEXNAME) };

NJS_TEXANIM Indicator_TEXANIM[] = {
	// w,	h,	cx,	cy,	u1,	v1,	u2,		v2,	texid,	attr
	// u2 and v2 must be 0xFF
	{ 24,	16,	12,	16,	0,	0,	0xFF,	0xFF,	0,	0 },	// arrow
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	1,	0 },	// cpu_1
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	2,	0 },	// cpu_2
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	3,	0 },	// p
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	4,	0 },	// p1
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	5,	0 },	// p2
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	6,	0 },	// p3
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	7,	0 }		// p4
};

NJS_SPRITE Indicator_SPRITE = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 0, &multicommon_TEXLIST, Indicator_TEXANIM };

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
	{ 0.750f, 0.500f, 0.500f, 0.500f }	// CPU
};

enum AnimIndex
{
	arrow, cpu_1, cpu_2,
	p, p1, p2, p3, p4
};

void LoadIndicators()
{
	LoadPVM("multicommon", &multicommon_TEXLIST);
}

void DrawElement(Uint32 i, Uint32 n)
{
	CharObj1* player = CharObj1Ptrs[i];

	if (player == nullptr)
		return;

	NJS_SPRITE* sp = &Indicator_SPRITE;
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
		SetSpriteColor(IsControllerEnabled((Uint8)i) ? &colors[charid] : &colors[9]);
		Draw2DSprite(&Indicator_SPRITE, n, -1.0f, NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA, 0);
	}
}

void DrawIndicators()
{
	njSetTexture(&multicommon_TEXLIST);
	njSetTextureNum(arrow);	// arrow

	for (Uint32 i = 0; i < 4; i++)
		DrawElement(i, arrow);

	for (Uint32 i = 0; i < 4; i++)
	{
		AnimIndex a = IsControllerEnabled((Uint8)i) ? p : cpu_1;
		njSetTextureNum(a);
		DrawElement(i, a);
	}

	for (Uint32 i = 0; i < 4; i++)
	{
		AnimIndex a = IsControllerEnabled((Uint8)i) ? (AnimIndex)(p1 + i) : cpu_2;
		njSetTextureNum(a);
		DrawElement(i, a);
	}
}

