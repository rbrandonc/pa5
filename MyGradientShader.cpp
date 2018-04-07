#include "GBitmap.h"
#include "GMatrix.h"
#include "MyGradientShader.h"
#include <iostream>
using namespace std;

MyGradientShader::MyGradientShader(GPoint p0, GPoint p1, const GColor* colors, int count) : p0(p0), p1(p1), colors(colors), count(count) {}

// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool MyGradientShader::isOpaque() {
//    bitmap.computeIsOpaque();
    return bitmap.isOpaque();
}

// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool MyGradientShader::setContext(const GMatrix& ctm) {
    GMatrix tmp;
    ctm.invert(&tmp);
    inverse.setConcat(local, tmp);

    //TODO: shouldn't always return true
    return true;
}

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void MyGradientShader::shadeRow(int x, int y, int count, GPixel row[]) {
    const float dx = inverse[GMatrix::SX];
    const float dy = inverse[GMatrix::KY];
    GPoint loc = inverse.mapXY(x + 0.5f, y + 0.5f);
    
    for (int i = 0; i < count; ++i) {
        // row[i] = *bitmap.getAddr(loc.fX, loc.fY);
        loc.fX += dx;
        loc.fY += dy;
    }
}

int MyGradientShader::pinX(float x) {
    if(x < 0) return 0;
    if(x >= bitmap.width()-1) return bitmap.width() -1;
    return GRoundToInt(x);
}

int MyGradientShader::pinY(float y) {
    if(y < 0) return 0;
    if(y >= bitmap.height()-1) return bitmap.height() -1;
    return GRoundToInt(y);
}

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor* colors, int count) {
    return std::unique_ptr<GShader>(new MyGradientShader(p0, p1, colors, count));
}