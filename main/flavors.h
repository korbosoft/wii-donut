#ifndef FLAVORS_H
#define FLAVORS_H

#define FROSTING_FLAVORS 21

enum DonutSpecial {
	NORMAL,
	GHOSTLY,
};

enum DonutTexture {
	NONE,
	RAINBOW,
	COLORED_METAL,
	METAL,
	SPONGE
};

typedef struct {
	char name[17];
	GXColor top;
	GXColor bottom;
	enum DonutSpecial special;
	enum DonutTexture texture;
} Donut;

static const Donut frosting[FROSTING_FLAVORS] = {
	{
		"Powdered",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL,
		NONE
	},
	{
		"Lard Lad",
		{245, 128, 195, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Chocolate",
		{112, 58, 42, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Purple",
		{138, 110, 210, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Lemon",
		{242, 255, 122, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Strawberry",
		{255, 113, 107, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Pumpkin",
		{255, 190, 99, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Key Lime",
		{189, 255, 130, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Blueberry",
		{105, 110, 255, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Caramel",
		{203, 111, 57, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Pistachio",
		{156, 255, 110, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Toxic Waste",
		{0, 255, 0, 255},
		{0, 255, 0, 255},
		NORMAL,
		NONE
	},
	{
		"Glazed",
		{232, 188, 135, 255},
		{232, 188, 135, 255},
		NORMAL,
		NONE
	},
	{
		"Bagel",
		{206, 137, 55, 255},
		{206, 137, 55, 255},
		NORMAL,
		NONE
	},
	{
		"Ghost",
		{255, 255, 255, 192},
		{255, 255, 255, 192},
		GHOSTLY,
		NONE
	},
	{
		"Uranium-235",
		{0, 255, 0, 255},
		{0, 255, 0, 255},
		NORMAL,
		COLORED_METAL
	},
	{
		"Rainbow",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL,
		RAINBOW
	},
	{
		"Chrome",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL,
		METAL
	},
	{
		"Gold",
		{255, 255, 0, 255},
		{255, 255, 0, 255},
		NORMAL,
		COLORED_METAL
	},
	{
		"Aluminum",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL,
		COLORED_METAL
	},
	{
		"Sponge",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL,
		SPONGE
	}
};

#endif
