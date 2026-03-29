#include <gccore.h>
#include <math.h>
#include <grrlib.h>

#include "donut.h"
#include "flavors.h"
#include "grrproxy.h"
#include "text.h"

#include "shape_lut_bin.h"

#include "metal_png.h"
#include "coloredMetal_png.h"

static GRRLIB_texImg *donutBuffer;
static GRRLIB_texImg *rainbowTex;
static GRRLIB_texImg *greyTex;
static GRRLIB_texImg *metalTex;
static GRRLIB_texImg *colorMetalTex;

void draw_frosting(f32 minor, f32 major, int nsides, int rings, bool filled, u32 col) {
	const f32 ringDelta = 2.0f*M_PI/rings;
	const f32 sideDelta = M_PI/nsides;
	const f32 waveAmp = 0.5f;
	const f32 waveFreq = 10.0f;

	f32 cosTheta = 1.0f;
	f32 sinTheta = 0.0f;
	f32 theta = 0.0f;
	for (int i = 0; i < rings; i++) {
		const f32 theta1 = theta + ringDelta;
		const f32 cosTheta1 = cos(theta1);
		const f32 sinTheta1 = sin(theta1);

		const f32 cutZ0 = waveAmp/2*(waveAmp*2 + sinf(theta*waveFreq));
		const f32 cutZ1 = waveAmp/2*(waveAmp*2 + sinf(theta1*waveFreq));

		GX_Begin(filled ? GX_TRIANGLESTRIP : GX_LINESTRIP, GX_VTXFMT0, 2*(nsides + 1));

		f32 phi = 0.0f;
		for (int j = 0; j <= nsides; j++) {
			const f32 cosPhi = cosf(phi), sinPhi = sinf(phi);
			const f32 dist = major + minor*cosPhi;

			f32 z = minor*sinPhi;
			f32 z1 = z;
			if (z1 < cutZ1) z1 = cutZ1;

			GX_Position3f32(cosTheta1*dist, -sinTheta1*dist, z1);
			GX_Normal3f32(cosTheta1*cosPhi, -sinTheta1*cosPhi, sinPhi);
			GX_Color1u32(col);

			f32 z0 = z;
			if (z0 < cutZ0) z0 = cutZ0;

			GX_Position3f32(cosTheta*dist, -sinTheta*dist, z0);
			GX_Normal3f32(cosTheta*cosPhi, -sinTheta*cosPhi, sinPhi);
			GX_Color1u32(col);

			phi += sideDelta;
		}
		GX_End();

		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
		theta = theta1;
	}
}

GRRLIB_texImg *makeRainbowTex(u16 width, u16 height) {
	GRRLIB_texImg *tex = GRRLIB_CreateEmptyTexture(width, height);
	for (u16 j = 0; j < height; j++) {
		for (u16 i = 0; i < width; i++) {
			f32 x = i - width/2.0f, y = j - height/2.0f;
			f32 p = -(sqrt(x*x + y*y) + 0.5f)*3.0f/height;
			f32 r = sinf(M_PI*p);
			f32 g = sinf(M_PI*(p + (1.0f/3.0f)));
			f32 b = sinf(M_PI*(p + (2.0f/3.0f)));
			r *= r*255.0f;
			g *= g*255.0f;
			b *= b*255.0f;
			GRRLIB_SetPixelTotexImg(i, j, tex, RGBA(r, g, b, 255));
		}
	}
	return tex;
}

void donut_init(void) {
	donutBuffer = GRRLIB_CreateEmptyTexture(DONUT_WIDTH*2 + DONUT_WIDTH*2 % 4, DONUT_HEIGHT*4);
	rainbowTex = makeRainbowTex(256, 256);
	greyTex = GRRLIB_CreateEmptyTexture(1, 1);
	GRRLIB_SetPixelTotexImg(0, 0, greyTex, 0x808080FF);
	metalTex = GRRLIB_LoadTexturePNG(metal_png);
	colorMetalTex = GRRLIB_LoadTexturePNG(coloredMetal_png);
}

void donut_exit(void) {
	GRRLIB_FreeTexture(donutBuffer);
	GRRLIB_FreeTexture(rainbowTex);
	GRRLIB_FreeTexture(greyTex);
	GRRLIB_FreeTexture(metalTex);
	GRRLIB_FreeTexture(colorMetalTex);
}

void setReflectiveTexture(GRRLIB_texImg *tex) {
	GXTexObj texObj;

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
	GX_InitTexObj(&texObj, tex->data, tex->w, tex->h, tex->format, GX_CLAMP, GX_CLAMP, GX_FALSE);
	if (GRRLIB_Settings.antialias == false) {
		GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
		GX_SetCopyFilter(GX_FALSE, rmode->sample_pattern, GX_FALSE, rmode->vfilter);
	} else {
		GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	}

	GX_SetNumTevStages(2);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR1A1 );
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);

	GX_LoadTexObj(&texObj, GX_TEXMAP0);
}

void render_frame(float A, float B, Donut flavor) {
	Mtx model, model2, viewreflect;

	guMtxCopy(view, viewreflect);
	viewreflect[0][3] = 0.0f;
	viewreflect[1][3] = 0.0f;
	viewreflect[2][3] = 0.0f;

	guMtxRotRad(model, 'x', A);
	guMtxRotRad(model2, 'z', B);
	guMtxConcat(model2, model, model);
	guMtxTransApply(model, model, 0.0f, 0.0f, -(3.0f/sinf(DegToRad(DONUT_FOV)/2.0f)));
	guMtxConcat(view,model,model);

	GX_LoadPosMtxImm(model, GX_PNMTX0);
	GX_LoadNrmMtxImm(model, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);

	guMtxConcat(viewreflect, model, model);
	guMtxScaleApply(model, model, 0.5f, -0.5f, 0.0f);
	guMtxTransApply(model, model, 0.5f, 0.5f, 1.0f);
	GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX3x4);

	switch (flavor.special) {
		case RAINBOW:
			setReflectiveTexture(rainbowTex);
			break;
		case METAL:
			setReflectiveTexture(metalTex);
			break;
		case COLORED_METAL:
			setReflectiveTexture(colorMetalTex);
			break;
		default:
			setReflectiveTexture(greyTex);
	}

	GRRLIB_DrawTorus(1, 2, 32, 64, true, RGBA(flavor.bottom.r, flavor.bottom.g, flavor.bottom.b, flavor.bottom.a));
	draw_frosting(1, 2, 32, 64, true, RGBA(flavor.top.r, flavor.top.g, flavor.top.b, flavor.top.a));

	GX_SetViewport(0,0, DONUT_WIDTH*2, DONUT_HEIGHT*4, 0, 1);
	GX_SetScissor(0,0, DONUT_WIDTH*2, DONUT_HEIGHT*4);
	GRRLIB_Screen2Texture(0, 0, donutBuffer, true);

	char frameBuffer[DONUT_WIDTH*DONUT_HEIGHT*20 + 1];
	char *ptr = frameBuffer;
	u8 last_r = -1, last_g = -1, last_b = -1;

	print("\x1b[H");
	for(u8 j = 0; j < DONUT_HEIGHT; j++) {
		for(u8 i = 0; i < DONUT_WIDTH; i++) {
			u16 lutIndex = 0;
			u16 r_avg = 0, g_avg = 0, b_avg = 0;

			for(u8 py = 0; py < 4; py++) {
				for(u8 px = 0; px < 2; px++) {
					u8 img_x = (i*2) + px;
					u8 img_y = (j*4) + py;

					u32 col = GRRLIB_GetPixelFromtexImg(img_x, img_y, donutBuffer);

					u8 r = R(col), g = G(col), b = B(col);

					u32 l = (r*13933 + g*46871 + b*4732); // u16 integer math for relative luminance

					u8 val = (l >> 22) & 0x03;
					u8 shift = (py*2 + px)*2;
					lutIndex |= (val << shift);

					r_avg += r; g_avg += g; b_avg += b;
				}
			}
			// average
			r_avg >>= 3;
			g_avg >>= 3;
			b_avg >>= 3;
			if (r_avg + g_avg + b_avg) {
				if ((last_r != r_avg) || (last_g != g_avg) || (last_b != b_avg)) {
					ptr = stpcpy(ptr, "\x1b[38;2;");
					ptr = u82Str(ptr, r_avg);
					*ptr++ = ';';
					ptr = u82Str(ptr, g_avg);
					*ptr++ = ';';
					ptr = u82Str(ptr, b_avg);
					*ptr++ = 'm';
					last_r = r_avg;
					last_g = g_avg;
					last_b = b_avg;
				}
					*ptr++ = shape_lut_bin[lutIndex];
			} else {
				*ptr++ = ' ';
			}
		}
	}
	*ptr++ = '\0';
	print(frameBuffer);
}
