#include <SADXModLoader.h>

enum TextureIndex
{
	arrow, cpu_1, cpu_2,
	p, p1, p2, p3, p4
};

NJS_TEXNAME multicommon_TEXNAME[] = {
	{ "arrow",	0, 0 },
	{ "cpu_1",	0, 0 },
	{ "cpu_2",	0, 0 },
	{ "p",		0, 0 },
	{ "p1",		0, 0 },
	{ "p2",		0, 0 },
	{ "p3",		0, 0 },
	{ "p4",		0, 0 }
};

NJS_TEXLIST multicommon_TEXLIST = { arrayptrandlength(multicommon_TEXNAME) };

NJS_TEXANIM Indicator_TEXANIM[] = {
	// w,	h,	cx,	cy,	u1,	v1,	u2,		v2,	texid,	attr
	// u2 and v2 must be 0xFF
	{ 24,	16,	12,	16,	0,	0,	0xFF,	0xFF,	arrow,	0 },
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	cpu_1,	0 },
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	cpu_2,	0 },
	{ 24,	24,	24,	40,	0,	0,	0xFF,	0xFF,	p,		0 },
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	p1,		0 },
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	p2,		0 },
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	p3,		0 },
	{ 24,	24,	0,	40,	0,	0,	0xFF,	0xFF,	p4,		0 }
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

void LoadIndicators()
{
	LoadPVM("multicommon", &multicommon_TEXLIST);
}

void DrawElement(Uint32 playerIndex, Uint32 textureIndex)
{
	CharObj1* player = CharObj1Ptrs[playerIndex];

	if (player == nullptr)
		return;

	NJS_SPRITE* sp = &Indicator_SPRITE;
	NJS_VECTOR pos = player->Position;
	Uint8 charid = (MetalSonicFlag) ? Characters_MetalSonic : player->CharID;
	pos.y += PhysicsArray[charid].CollisionSize;
	njProjectScreen(nullptr, &pos, (NJS_POINT2*)&sp->p);

	bool isVisible = sp->p.x - sp->tanim[playerIndex].sx < HorizontalResolution
		&& sp->p.x + sp->tanim[playerIndex].sx > 0
		&& sp->p.y - sp->tanim[playerIndex].sy < VerticalResolution
		&& sp->p.y + sp->tanim[playerIndex].sy > 0;

	if (!isVisible)
	{
		// Do off-screen pointing thing here
		return;
	}
	else
	{
		SetSpriteColor(IsControllerEnabled((Uint8)playerIndex) ? &colors[charid] : &colors[9]);
		Draw2DSprite(&Indicator_SPRITE, textureIndex, -1.0f, NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA, 0);
	}
}

void DrawIndicators()
{
	njSetTexture(&multicommon_TEXLIST);
	njSetTextureNum(arrow);

	for (Uint32 i = 0; i < 4; i++)
		DrawElement(i, arrow);

	for (Uint32 i = 0; i < 4; i++)
	{
		TextureIndex index = IsControllerEnabled((Uint8)i) ? p : cpu_1;
		njSetTextureNum(index);
		DrawElement(i, index);
	}

	for (Uint32 i = 0; i < 4; i++)
	{
		TextureIndex index = IsControllerEnabled((Uint8)i) ? (TextureIndex)(p1 + i) : cpu_2;
		njSetTextureNum(index);
		DrawElement(i, index);
	}
}

