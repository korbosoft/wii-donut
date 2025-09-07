#ifndef FLAVORS_H
#define FLAVORS_H

#include <gccore.h>
#include <stdlib.h>

#define FLAVORS 6

typedef struct {
	u8 r;
	u8 g;
	u8 b;
} RGB;

typedef struct {
	bool lolcat : 1;
	bool inverted : 1;
	bool radiates : 1;
} _Flags;

typedef struct {
	char name[17];
	RGB top;
	RGB bottom;
	_Flags flags;
} Donut;

float rand_float() { return (float)rand() / (float)RAND_MAX; }

const Donut flavors[FLAVORS] = {
	{
		"Classic",
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
};

#endif
