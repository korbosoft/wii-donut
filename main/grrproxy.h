#ifndef GRRPROXY_H
#define GRRPROXY_H

extern Mtx view;

extern void PROXY_3dMode(f32 minDist, f32 maxDist, f32 fov, bool texturemode, bool normalmode, f32 aspect);

extern void PROXY_Camera3dSettings(f32 posx, f32 posy, f32 posz,
                            f32 upx, f32 upy, f32 upz,
                            f32 lookx, f32 looky, f32 lookz);

extern void PROXY_ObjectViewBegin(void);
extern void PROXY_ObjectViewScale(f32 scalx, f32 scaly, f32 scalz);
extern void PROXY_ObjectViewRotate(f32 angx, f32 angy, f32 angz, bool local);
extern void PROXY_ObjectViewTrans(f32 posx, f32 posy, f32 posz);
extern void PROXY_ObjectViewEnd(void);

#endif
