#include <gccore.h>
#include <stdio.h>
#include <math.h>
#include <grrlib.h>

#include "donut.h"
#include "grrproxy.h"
#include "flavors.h"

#include "shape_lut_bin.h"

#include "rainbow_png.h"
#include "grey_png.h"
#include "metal_png.h"
#include "coloredMetal_png.h"

static GRRLIB_texImg *donutBuffer;
static GRRLIB_texImg *rainbowTex;
static GRRLIB_texImg *greyTex;
static GRRLIB_texImg *metalTex;
static GRRLIB_texImg *colorMetalTex;

void draw_frosting(f32 minor, f32 major, int nsides, int rings, bool filled, u32 col) {
    const f32 ringDelta = 2.0f * M_PI / rings;
    const f32 sideDelta = M_PI / nsides;
    const f32 waveAmp = 0.5f;
    const f32 waveFreq = 10.0f;

    f32 cosTheta = 1.0f;
    f32 sinTheta = 0.0f;
    f32 theta = 0.0f;
    for (int i = 0; i < rings; i++) {
        const f32 theta1 = theta + ringDelta;
        const f32 cosTheta1 = cos(theta1);
        const f32 sinTheta1 = sin(theta1);

        const f32 cutZ0 = waveAmp * sinf(theta * waveFreq);
        const f32 cutZ1 = waveAmp * sinf(theta1 * waveFreq);

        GX_Begin(filled ? GX_TRIANGLESTRIP : GX_LINESTRIP, GX_VTXFMT0, 2 * (nsides + 1));

        f32 phi = 0.0f;
        for (int j = 0; j <= nsides; j++) {
            phi += sideDelta;
            const f32 cosPhi = cosf(phi), sinPhi = sinf(phi);
            const f32 dist = major + minor * cosPhi;

            f32 z = minor * sinPhi;
            if (z < cutZ1) z = cutZ1;

            GX_Position3f32(cosTheta1 * dist, -sinTheta1 * dist, z);
            GX_Normal3f32(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
            GX_Color1u32(col);

            if (z < cutZ0) z = cutZ0;

            GX_Position3f32(cosTheta * dist, -sinTheta * dist, z);
            GX_Normal3f32(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
            GX_Color1u32(col);

        }
        GX_End();

        cosTheta = cosTheta1;
        sinTheta = sinTheta1;
        theta = theta1;
    }
}

void donut_init(void) {
    donutBuffer = GRRLIB_CreateEmptyTexture(DONUT_WIDTH * 2 + DONUT_WIDTH * 2 % 4, DONUT_HEIGHT * 4);
    rainbowTex = GRRLIB_LoadTexturePNG(rainbow_png);
    greyTex = GRRLIB_LoadTexturePNG(grey_png);
    metalTex = GRRLIB_LoadTexturePNG(metal_png);
    colorMetalTex = GRRLIB_LoadTexturePNG(coloredMetal_png);
}

void donut_exit(void) {
    GRRLIB_FreeTexture(donutBuffer);
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

    GX_LoadTexObj(&texObj,      GX_TEXMAP0);
}

void render_frame(float A, float B, Donut flavor) {
    Mtx model, model2, viewreflec;

    guMtxCopy(view, viewreflec);
    viewreflec[0][3] = 0.0f;
    viewreflec[1][3] = 0.0f;
    viewreflec[2][3] = 0.0f;

    guMtxRotRad(model, 'x', A);
    guMtxRotRad(model2, 'z', B);
    guMtxConcat(model2, model, model);
    guMtxTransApply(model, model, 0.0f, 0.0f, -(3.0f / sinf(DegToRad(DONUT_FOV) / 2.0f)));
    guMtxConcat(view,model,model);

    GX_LoadPosMtxImm(model, GX_PNMTX0);
    GX_LoadNrmMtxImm(model, GX_PNMTX0);
    GX_SetCurrentMtx(GX_PNMTX0);

    guMtxConcat(viewreflec, model, model);
    guMtxScaleApply(model, model, 0.5f, -0.5f, 0.0f);
    guMtxTransApply(model, model, 0.5f, 0.5f, 1.0f);
    GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX3x4);

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

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

    GRRLIB_DrawTorus(1, 2, 32, 64, true, RGBA(flavor.bottom.r, flavor.bottom.g, flavor.bottom.b, (flavor.special == GHOSTLY) ? 192 : 255));
    if (flavor.special != GHOSTLY)
        draw_frosting(1, 2, 32, 64, true, RGBA(flavor.top.r, flavor.top.g, flavor.top.b, 255));

    GX_SetViewport(0,0, DONUT_WIDTH * 2, DONUT_HEIGHT * 4, 0, 1);
    GX_SetScissor(0,0, DONUT_WIDTH * 2, DONUT_HEIGHT * 4);
    GRRLIB_Screen2Texture(0, 0, donutBuffer, true);

    char frameBuffer[DONUT_WIDTH * DONUT_HEIGHT * 32];
    char *ptr = frameBuffer;

    printf("\x1b[0;0H");
    for(u8 i = 0; i < DONUT_HEIGHT; i++) {
        for(u8 j = 0; j < DONUT_WIDTH; j++) {
            u16 lutIndex = 0;
            u32 r_sum = 0, g_sum = 0, b_sum = 0;

            for(u8 py = 0; py < 4; py++) {
                for(u8 px = 0; px < 2; px++) {
                    u8 img_x = (j * 2) + px;
                    u8 img_y = (i * 4) + py;

                    u32 col = GRRLIB_GetPixelFromtexImg(img_x, img_y, donutBuffer);

                    u8 r = R(col), g = G(col), b = B(col);

                    u8 y = (r + g + b) / 3;

                    u8 val = (y >> 6) & 0x03;
                    u8 shift = (py * 2 + px) * 2;
                    lutIndex |= (val << shift);

                    r_sum += r; g_sum += g; b_sum += b;
                }
            }
            u8 r_avg = r_sum >> 3;
            u8 g_avg = g_sum >> 3;
            u8 b_avg = b_sum >> 3;
            char c = (char)shape_lut_bin[lutIndex];
            if ((r_avg + g_avg + b_avg) != 0) {
                ptr += sprintf(ptr, "\e[38;2;%i;%i;%im%c", r_avg, g_avg, b_avg, c);
            } else {
                ptr += sprintf(ptr, " ");
            }
        }
        ptr += sprintf(ptr, "\n");
    }
    puts(frameBuffer);
}
