#include <grrlib.h>

#include "donut.h"

Mtx view;
static guVector _PROXY_cam = {0.0f, 0.0f, 0.0f};
static guVector _PROXY_up = {0.0f, 1.0f, 0.0f};
static guVector _PROXY_look = {0.0f, 0.0f, -100.0f};
static guVector _GRRaxisx = {1.0f, 0.0f, 0.0f};
static guVector _GRRaxisy = {0.0f, 1.0f, 0.0f};
static guVector _GRRaxisz = {0.0f, 0.0f, 1.0f};
static Mtx _ObjTransformationMtx;

void PROXY_Camera3dSettings(f32 posx, f32 posy, f32 posz,
    f32 upx, f32 upy, f32 upz,
    f32 lookx, f32 looky, f32 lookz) {

   _PROXY_cam.x = posx;
   _PROXY_cam.y = posy;
   _PROXY_cam.z = posz;

   _PROXY_up.x = upx;
   _PROXY_up.y = upy;
   _PROXY_up.z = upz;

   _PROXY_look.x = lookx;
   _PROXY_look.y = looky;
   _PROXY_look.z = lookz;
}

void PROXY_3dMode(f32 minDist, f32 maxDist, f32 fov, bool texturemode, bool normalmode, f32 aspect) {
    Mtx44 m;

    GRRLIB_3dMode(minDist, maxDist, fov, texturemode, normalmode);
    guLookAt(view, &_PROXY_cam, &_PROXY_up, &_PROXY_look);
    guPerspective(m, fov, aspect, minDist, maxDist);
    GX_LoadProjectionMtx(m, GX_PERSPECTIVE);
}

void PROXY_ObjectViewBegin(void) {
    guMtxIdentity(_ObjTransformationMtx);
}

void PROXY_ObjectViewScale(f32 scalx, f32 scaly, f32 scalz) {
    Mtx m;

    guMtxIdentity(m);
    guMtxScaleApply(m, m, scalx, scaly, scalz);

    guMtxConcat(m, _ObjTransformationMtx, _ObjTransformationMtx);
}

void PROXY_ObjectViewRotate(f32 angx, f32 angy, f32 angz, bool local) {
    Mtx m;
    Mtx rx, ry, rz;

    guMtxIdentity(m);
    guMtxRotAxisDeg(rx, &_GRRaxisx, angx);
    guMtxRotAxisDeg(ry, &_GRRaxisy, angy);
    guMtxRotAxisDeg(rz, &_GRRaxisz, angz);
    guMtxConcat(ry, rx, m);
    guMtxConcat(m, rz, m);
    if (local) {
        guMtxConcat(_ObjTransformationMtx, m, _ObjTransformationMtx);
    } else {
        guMtxConcat(m, _ObjTransformationMtx, _ObjTransformationMtx);
    }
}

void PROXY_ObjectViewTrans(f32 posx, f32 posy, f32 posz) {
    Mtx m;

    guMtxIdentity(m);
    guMtxTransApply(m, m, posx, posy, posz);

    guMtxConcat(m, _ObjTransformationMtx, _ObjTransformationMtx);
}

void PROXY_ObjectViewEnd(void) {
    Mtx mv, mvi;

    guMtxConcat(view, _ObjTransformationMtx, mv);
    GX_LoadPosMtxImm(mv, GX_PNMTX0);

    guMtxInverse(mv, mvi);
    guMtxTranspose(mvi, mv);
    GX_LoadNrmMtxImm(mv, GX_PNMTX0);
}
