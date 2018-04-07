#include <memory>
#include "GPixel.h"
#include "GShader.h"
#include "GBlendMode.h"

/**
 *  GShaders create colors to fill whatever geometry is being drawn to a GCanvas.
 */
class MyBlender {
public:
    static GPixel blend(GPixel, GPixel, GBlendMode);

    static GPixel Clear(int, int, int, int, int, int, int, int);

    static GPixel Src(int, int, int, int, int, int, int, int);

    static GPixel Dst(int, int, int, int, int, int, int, int);

    static GPixel SrcOver(int, int, int, int, int, int, int, int);

    static GPixel DstOver(int, int, int, int, int, int, int, int);
 
    static GPixel SrcIn(int, int, int, int, int, int, int, int);

    static GPixel DstIn(int, int, int, int, int, int, int, int);

    static GPixel SrcOut(int, int, int, int, int, int, int, int);

    static GPixel DstOut(int, int, int, int, int, int, int, int);

    static GPixel SrcATop(int, int, int, int, int, int, int, int);

    static GPixel DstATop(int, int, int, int, int, int, int, int);

    static GPixel Xor(int, int, int, int, int, int, int, int);

};