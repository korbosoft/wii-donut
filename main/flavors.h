#ifndef FLAVORS_H
#define FLAVORS_H

#define FROSTING_FLAVORS 20

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
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		NORMAL
	},
	{
		"Lard Lad",
		{245, 128, 195, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Chocolate",
		{112, 58, 42, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Purple",
		{138, 110, 210, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Lemon",
		{242, 255, 122, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Strawberry",
		{255, 113, 107, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Pumpkin",
		{255, 190, 99, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Key Lime",
		{189, 255, 130, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Blueberry",
		{105, 110, 255, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Caramel",
		{203, 111, 57, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Pistachio",
		{156, 255, 110, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Toxic Waste",
		{0, 255, 0, 255},
		{0, 255, 0, 255},
		NORMAL
	},
	{
		"Glazed",
		{232, 188, 135, 255},
		{232, 188, 135, 255},
		NORMAL
	},
	{
		"Bagel",
		{206, 137, 55, 255},
		{206, 137, 55, 255},
		NORMAL
	},
	{
		"Ghost",
		{255, 255, 255, 192},
		{255, 255, 255, 192},
		GHOSTLY
	},
	{
		"Uranium-235",
		{0, 255, 0, 255},
		{0, 255, 0, 255},
		COLORED_METAL
	},
	{
		"Rainbow",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		RAINBOW
	},
	{
		"Chrome",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		METAL
	},
	{
		"Gold",
		{255, 255, 0, 255},
		{255, 255, 0, 255},
		COLORED_METAL
	},
	{
		"Aluminum",
		{255, 255, 255, 255},
		{255, 255, 255, 255},
		COLORED_METAL
	}
};

#endif
