#ifndef FLAVORS_H
#define FLAVORS_H

#define FROSTING_FLAVORS 18

enum DonutSpecial {
	NORMAL,
	RAINBOW,
	GHOSTLY,
	COLORED_METAL,
	METAL
};

typedef struct {
	char name[17];
	GXColor top;
	GXColor bottom;
	enum DonutSpecial special;
} Donut;

static const Donut frosting[FROSTING_FLAVORS] = {
	{
		"Powdered",
		{255, 255, 255},
		{255, 255, 255},
		NORMAL
	},
	{
		"Lard Lad",
		{245, 128, 195},
		{206, 137, 55},
		NORMAL
	},
	{
		"Chocolate",
		{112, 58, 42},
		{206, 137, 55},
		NORMAL
	},
	{
		"Purple",
		{138, 110, 210},
		{206, 137, 55},
		NORMAL
	},
	{
		"Lemon",
		{242, 255, 122},
		{206, 137, 55},
		NORMAL
	},
	{
		"Strawberry",
		{255, 113, 107},
		{206, 137, 55},
		NORMAL
	},
	{
		"Pumpkin",
		{255, 190, 99},
		{206, 137, 55},
		NORMAL
	},
	{
		"Key Lime",
		{189, 255, 130},
		{206, 137, 55},
		NORMAL
	},
	{
		"Blueberry",
		{105, 110, 255},
		{206, 137, 55},
		NORMAL
	},
	{
		"Caramel",
		{203, 111, 57},
		{206, 137, 55},
		NORMAL
	},
	{
		"Pistachio",
		{156, 255, 110},
		{206, 137, 55},
		NORMAL
	},
	{
		"Toxic Waste",
		{0, 255, 0},
		{0, 255, 0},
		NORMAL
	},
	{
		"Glazed",
		{232, 188, 135},
		{232, 188, 135},
		NORMAL
	},
	{
		"Bagel",
		{206, 137, 55},
		{206, 137, 55},
		NORMAL
	},
	{
		"Ghost",
		{255, 255, 255},
		{255, 255, 255},
		GHOSTLY
	},
	{
		"Uranium-235",
		{128, 255, 128},
		{128, 255, 128},
		COLORED_METAL
	},
	{
		"Rainbow",
		{255, 255, 255},
		{255, 255, 255},
		RAINBOW
	},
	{
		"Metal",
		{255, 255, 255},
		{255, 255, 255},
		METAL
	}
};

#endif
