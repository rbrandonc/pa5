#include "GBitmap.h"
#include "GMatrix.h"
#include "MyShader.h"
#include <iostream>
using namespace std;

MyShader::MyShader(const GBitmap& bit, const GMatrix& mat) : local(mat), bitmap(bit) {
    // cout << "h: " << bitmap.height() << " w: " << bitmap.width() << endl;
    // cout << local[0] << " " << local[1] << " "<< local[2] << " "<< endl;
    // cout << local[3] << " "<< local[4] << " "<< local[5] << endl;
    for(int i = 0; i < bitmap.width(); i++) {
        // cout << GPixel_GetA(*bitmap.getAddr(i, 0)) << endl;
    }
}

// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool MyShader::isOpaque() {
    // bitmap.computeIsOpaque();
    return bitmap.isOpaque();
}

// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool MyShader::setContext(const GMatrix& ctm) {
   // cout << ctm[0] << " " << ctm[1] << " "<< ctm[2] << " "<< endl;
   // cout << ctm[3] << " "<< ctm[4] << " "<< ctm[5] << endl;
   // cout << "------" << endl;
    GMatrix tmp;
    ctm.invert(&tmp);
    inverse.setConcat(local, tmp);
    // if(tmp.invert(&inverse)) {
       // cout << inverse[0] << " " << inverse[1] << " "<< inverse[2] << " "<< endl;
       // cout << inverse[3] << " "<< inverse[4] << " "<< inverse[5] << endl;
       // cout << "--------------------" << endl;
        return true;
    // }
    // return false;
}

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void MyShader::shadeRow(int x, int y, int count, GPixel row[]) {
    const float dx = inverse[GMatrix::SX];
    const float dy = inverse[GMatrix::KY];
    GPoint loc = inverse.mapXY(x + 0.5f, y + 0.5f);
    
    for (int i = 0; i < count; ++i) {
        row[i] = *bitmap.getAddr(loc.fX, loc.fY);
        loc.fX += dx;
        loc.fY += dy;
    }
}

int MyShader::pinX(float x) {
    if(x < 0) return 0;
    if(x >= bitmap.width()-1) return bitmap.width() -1;
    return GRoundToInt(x);
}

int MyShader::pinY(float y) {
    if(y < 0) return 0;
    if(y >= bitmap.height()-1) return bitmap.height() -1;
    return GRoundToInt(y);
}

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bit, const GMatrix& localInv) {
    return std::unique_ptr<GShader>(new MyShader(bit, localInv));
}
