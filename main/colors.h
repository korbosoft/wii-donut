#ifndef COLORS_H
#define COLORS_H
 
static const GXColor LC_WHITE  = {255, 255, 255, 255}; //"liy color"
static const GXColor LC_RED    = {255, 0,   0,   255};
static const GXColor LC_GREEN  = {0,   255, 0,   255};
static const GXColor LC_BLUE   = {0,   0, 255,   255};
static const GXColor LC_YELLOW = {255, 255, 0,   255};
static const GXColor LC_CYAN   = {0,   255, 255, 255};
static const GXColor LC_MAGENTA= {255, 0,   255, 255};
static const GXColor LC_LIGHTGREY={192,192, 192, 255};
static const GXColor LC_LIGHTERGREY={224,224,224, 255};
static const GXColor LC_LIGHTISH={160, 160, 160, 255};
static const GXColor LC_GREY   = {128, 128, 128, 255};
static const GXColor LC_DARKISH ={96,  96,  96,  255};
static const GXColor LC_DARKGREY={64,  64,  64,  255};
static const GXColor LC_DARKER = {32,  32,  32,  255};
static const GXColor LC_DARKDARK={16,16,16,255};
static const GXColor LC_DARKDARKDARK={8,8,8,255};
static const GXColor LC_DARKDARKDARKDARK={4,4,4,255};
static const GXColor LC_DARKDARKDARKDARKDARK={2,2,2,255};
static const GXColor LC_BLACK  = {0,   0,   0,   255};
static const GXColor LC_GILLPURPLE = {39, 3, 50, 255};
static const GXColor LC_GAYSKY = {99, 131, 171, 255};//148 196 255
static const GXColor LC_CAFEAM = {133, 135, 171, 255};
static const GXColor LC_RIMTILT= {180, 180, 196, 255};

GXColor colorMul(GXColor a, GXColor b) {
	float ar, ag, ab, aa, br, bg, bb, ba;
	ar = (float)a.r * 0.003921568627;
	ag = (float)a.g * 0.003921568627;
	ab = (float)a.b * 0.003921568627;
	aa = (float)a.a * 0.003921568627;

	br = (float)b.r * 0.003921568627;
	bg = (float)b.g * 0.003921568627;
	bb = (float)b.b * 0.003921568627;
	ba = (float)b.a * 0.003921568627;
	
	ar *= br;
	ag *= bg;
	ab *= bb;
	aa *= ba;

	ar *= 255.0f;
	ag *= 255.0f;
	ab *= 255.0f;
	aa *= 255.0f;

	return (GXColor){(u8)ar, (u8)ag, (u8)ab, (u8)aa};
}

#endif
