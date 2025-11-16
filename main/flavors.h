#ifndef FLAVORS_H
#define FLAVORS_H

#include "color.h"

#define FLAVORS 17

typedef struct {
	char name[17];
	RGB top;
	RGB bottom;
	struct _flags {
		bool lolcat : 1;
		bool ghost : 1;
		bool radiates : 1;
	} flags;
} Donut;

const Donut flavors[FLAVORS] = {
	{
		"Powdered",
		{255, 255, 255},
		{255, 255, 255},
		{false, false, false}
	},
	{
		"Lard Lad",
		{245, 128, 195},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Chocolate",
		{112, 58, 42},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Purple",
		{138, 110, 210},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Lemon",
		{242, 255, 122},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Strawberry",
		{255, 113, 107},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Pumpkin",
		{255, 190, 99},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Key Lime",
		{189, 255, 130},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Blueberry",
		{105, 110, 255},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Caramel",
		{203, 111, 57},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Pistachio",
		{156, 255, 110},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Toxic Waste",
		{0, 255, 0},
		{0, 255, 0},
		{false, false, false}
	},
	{
		"Glazed",
		{232, 188, 135},
		{232, 188, 135},
		{false, false, false}
	},
	{
		"Bagel",
		{206, 137, 55},
		{206, 137, 55},
		{false, false, false}
	},
	{
		"Ghost",
		{255, 255, 255},
		{255, 255, 255},
		{false, true, false}
	},
	{
		"Uranium-235",
		{255, 224, 255},
		{255, 224, 255},
		{false, false, true}
	},
	{
		"Rainbow",
		{255, 0, 255},
		{255, 0, 255},
		{true, false, false}
	}
};

#endif
