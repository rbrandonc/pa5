#include "GBitmap.h"
#include "GMatrix.h"
#include "MyGradientShader.h"
#include <iostream>
using namespace std;

MyGradientShader::MyGradientShader(GPoint p0, GPoint p1, const GColor* colors, int count) : p0(p0), p1(p1), colors(colors), count(count) {
    // cout << count << endl;
    // for(int i = 0; i < this->count; i++) {
    //         cout << "color " << i << ": " << colors[i].fA << " " << colors[i].fR << " " << colors[i].fG << " " << colors[i].fB << endl;
    // }
}

// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool MyGradientShader::isOpaque() {
//    bitmap.computeIsOpaque();
    return false;
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
	cout << "Shade Row (Gradient)" << endl;
 //    cout << x << " " << y << " (" << p0.x() << ", " << p0.y() << ") (" << p1.x() << ", " << p1.y() << ")" << endl;
    // for(int i = 0; i < this->count; i++) {
    //         cout << "color " << i << ": " << colors[i].fA << " " << colors[i].fR << " " << colors[i].fG << " " << colors[i].fB << endl;;
    // }
    
    float dx = p1.x()-p0.x();
    float dy = p1.y()-p0.y();
    GMatrix grad = GMatrix(dx, -1*dy, p0.x(),
                            dy, dx, p0.y());

    for (float i = 0; i < count; i++) {

        GPoint loc = grad.mapXY((i/count), 0);
        // cout << loc.x() << " " << loc.y() << endl;

        float gradLength = sqrt((p1.x()-p0.x())*(p1.x()-p0.x())+(p1.y()-p0.y())*(p1.y()-p0.y()));
        float locLength = sqrt((loc.x()-p0.x())*(loc.x()-p0.x())+(loc.y()-p0.y())*(loc.y()-p0.y()));
        int colorA = floor(locLength/(gradLength/(this->count-1)));
        // cout << locLength << " " << gradLength << " " << colorA << endl;

        float weight = (locLength/(gradLength/(this->count-1))) - colorA;
        float a = (colors[colorA].fA*255*(1-weight) + colors[colorA+1].fA*255*weight);
        float r = (colors[colorA].fR*colors[colorA].fA*255*(1-weight) + colors[colorA+1].fR*colors[colorA+1].fA*255*weight);
        float g = (colors[colorA].fG*colors[colorA].fA*255*(1-weight) + colors[colorA+1].fG*colors[colorA+1].fA*255*weight);
        float b = (colors[colorA].fB*colors[colorA].fA*255*(1-weight) + colors[colorA+1].fB*colors[colorA+1].fA*255*weight);

        if(r <= a && g <= a && b <= a && a <= 255 && a <= 255 && r <= 255 && b <= 255 && g <= 255 && a >= 0 && r >= 0 && g >= 0 && b >= 0)
            row[(int)i] = GPixel_PackARGB(a, r, g, b);
        else {
            row[(int)i] = GPixel_PackARGB(255, 0, 255, 255);
            cout << "color 1: " << colors[colorA].fA << " " << colors[colorA].fR << " " << colors[colorA].fG << " " << colors[colorA].fB << endl;
            cout << "color 2: " << colors[colorA+1].fA << " " << colors[colorA+1].fR << " " << colors[colorA+1].fG << " " << colors[colorA+1].fB << endl;
            cout << weight << endl;
            cout << a << " " << r << " " << g << " " << b << endl;
        }
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
    GColor *c = new GColor[count];
    for(int i = 0; i < count; i++)
        c[i] = *(new GColor({colors[i].fA, colors[i].fR, colors[i].fG, colors[i].fB}));
    return std::unique_ptr<GShader>(new MyGradientShader(p0, p1, c, count));
}

