#include <memory>
#include "GPixel.h"
#include "GShader.h"

/**
 *  GShaders create colors to fill whatever geometry is being drawn to a GCanvas.
 */
class MyGradientShader: public GShader {
public:
//    virtual ~MyShader() {}
    MyGradientShader(GPoint p0, GPoint p1, const GColor* colors, int count);

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque();

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix& ctm);

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    void shadeRow(int x, int y, int count, GPixel row[]);

private:
    GMatrix local;
    GMatrix inverse;
    GBitmap bitmap;
    GPoint p0;
    GPoint p1;
    const GColor* colors;
    int count;

    int pinX(float);
    int pinY(float);
};

/**
 *  Return a subclass of GShader that draws the specified bitmap and the inverse of a local matrix.
 *  Returns null if the either parameter is invalid.
 */
std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor[], int count);