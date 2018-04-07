#include "GBitmap.h"
#include "GMatrix.h"
#include "MyBlender.h"
#include "GBlendMode.h"
#include <iostream>
using namespace std;

GPixel MyBlender::blend(GPixel src, GPixel dest, GBlendMode mode) {
    int srcA = GPixel_GetA(src);
    int srcR = GPixel_GetR(src);
    int srcG = GPixel_GetG(src);
    int srcB = GPixel_GetB(src);

    int destA = GPixel_GetA(dest);
    int destR = GPixel_GetR(dest);
    int destG = GPixel_GetG(dest);
    int destB = GPixel_GetB(dest);

    // if((GPixel_GetA(src) < 0 || GPixel_GetA(src) > 255)
    //    || (GPixel_GetR(src) < 0 || GPixel_GetR(src) > 255)
    //    || (GPixel_GetG(src) < 0 || GPixel_GetG(src) > 255)
    //    || (GPixel_GetB(src) < 0 || GPixel_GetB(src) > 255)
    //    || (GPixel_GetA(dest) < 0 || GPixel_GetA(dest) > 255)
    //    || (GPixel_GetR(dest) < 0 || GPixel_GetR(dest) > 255)
    //    || (GPixel_GetG(dest) < 0 || GPixel_GetG(dest) > 255)
    //    || (GPixel_GetB(dest) < 0 || GPixel_GetB(dest) > 255)
    //    || (GPixel_GetR(src) > GPixel_GetA(src))
    //    || (GPixel_GetG(src) > GPixel_GetA(src))
    //    || (GPixel_GetB(src) > GPixel_GetA(src))
    //    || (GPixel_GetR(dest) > GPixel_GetA(dest))
    //    || (GPixel_GetG(dest) > GPixel_GetA(dest))
    //    || (GPixel_GetB(dest) > GPixel_GetA(dest))) {
    //     cout << "fuck" << endl;
    //     return GPixel_PackARGB(255, 255, 255, 255);
    // }

    GPixel temp;

    switch(mode) {
        case GBlendMode::kClear:
            temp = Clear(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kSrc:
            temp = Src(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kDst:
            temp = Dst(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kSrcOver:
            temp = SrcOver(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kDstOver:
            temp = DstOver(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kSrcIn:
            temp = SrcIn(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kDstIn:
            temp = DstIn(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kSrcOut:
            temp = SrcOut(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kDstOut:
            temp = DstOut(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kSrcATop:
            temp = SrcATop(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kDstATop:
            temp = DstATop(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        case GBlendMode::kXor:
            temp = Xor(srcA, srcR, srcG, srcB, destA, destR, destG, destB);
            break;
        default:
            break;
    }

    return temp;
}

//    START BLENDING FUNCTIONS
//    kClear,    //!< [0, 0]
GPixel MyBlender::Clear(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    return GPixel_PackARGB(0, 0, 0, 0);
}
//    kSrc,      //!< [Sa, Sc]
GPixel MyBlender::Src(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    return GPixel_PackARGB(srcA, srcR, srcG, srcB);
}
//    kDst,      //!< [Da, Dc]
GPixel MyBlender::Dst(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    return GPixel_PackARGB(destA, destR, destG, destB);
}
//    kSrcOver,  //!< [Sa + Da * (1 - Sa), Sc + Dc * (1 - Sa)]
GPixel MyBlender::SrcOver(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = srcA + (255 - srcA) * destA / 255;
    int blendedR = srcR + (255 - srcA) * destR / 255;
    int blendedG = srcG + (255 - srcA) * destG / 255;
    int blendedB = srcB + (255 - srcA) * destB / 255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kDstOver,  //!< [Da + Sa * (1 - Da), Dc + Sc * (1 - Da)]
GPixel MyBlender::DstOver(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = destA + (srcA * (255 - destA))/255;
    int blendedR = destR + (srcR * (255 - destA))/255;
    int blendedG = destG + (srcG * (255 - destA))/255;
    int blendedB = destB + (srcB * (255 - destA))/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kSrcIn,    //!< [Sa * Da, Sc * Da]
GPixel MyBlender::SrcIn(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = (srcA * destA)/255;
    int blendedR = (srcR * destA)/255;
    int blendedG = (srcG * destA)/255;
    int blendedB = (srcB * destA)/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kDstIn,    //!< [Da * Sa, Dc * Sa]
GPixel MyBlender::DstIn(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = (destA * srcA)/255;
    int blendedR = (destR * srcA)/255;
    int blendedG = (destG * srcA)/255;
    int blendedB = (destB * srcA)/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kSrcOut,   //!< [Sa * (1 - Da), Sc * (1 - Da)]
GPixel MyBlender::SrcOut(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = (srcA * (255 - destA))/255;
    int blendedR = (srcR * (255 - destA))/255;
    int blendedG = (srcG * (255 - destA))/255;
    int blendedB = (srcB * (255 - destA))/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kDstOut,   //!< [Da * (1 - Sa), Dc * (1 - Sa)]
GPixel MyBlender::DstOut(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = (destA * (255 - srcA))/255;
    int blendedR = (destR * (255 - srcA))/255;
    int blendedG = (destG * (255 - srcA))/255;
    int blendedB = (destB * (255 - srcA))/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kSrcATop,  //!< [Da, Sc * Da + Dc * (1 - Sa)]
GPixel MyBlender::SrcATop(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = destA;
    int blendedR = (srcR * destA)/255 + (destR * (255 - srcA))/255;
    int blendedG = (srcG * destA)/255 + (destG * (255 - srcA))/255;
    int blendedB = (srcB * destA)/255 + (destB * (255 - srcA))/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kDstATop,  //!< [Sa, Dc * Sa + Sc * (1 - Da)]
GPixel MyBlender::DstATop(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = srcA;
    int blendedR = (destR * srcA)/255 + (srcR * (255 - destA))/255;
    int blendedG = (destG * srcA)/255 + (srcG * (255 - destA))/255;
    int blendedB = (destB * srcA)/255 + (srcB * (255 - destA))/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}
//    kXor,      //!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + Dc * (1 - Sa)]
GPixel MyBlender::Xor(int srcA, int srcR, int srcG, int srcB, int destA, int destR, int destG, int destB) {
    int blendedA = srcA + destA - 2 * srcA * destA/255;
    int blendedR = srcR * (255 - destA)/255 + destR * (255 - srcA)/255;
    int blendedG = srcG * (255 - destA)/255 + destG * (255 - srcA)/255;
    int blendedB = srcB * (255 - destA)/255 + destB * (255 - srcA)/255;
    return GPixel_PackARGB(blendedA, blendedR, blendedG, blendedB);
}