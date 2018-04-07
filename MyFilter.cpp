#include <iostream>
#include "GColor.h"
#include "GCanvas.h"
#include "GBlendMode.h"
#include "GFilter.h"
#include "MyBlender.h"
#include "GPixel.h"

class MyFilter : public GFilter {
public:
    MyFilter(const GBlendMode& mode, const GColor& color) : fMode(mode), fColor(color) {}

    bool preservesAlpha() {
        return false;
    };

    void filter(GPixel output[], const GPixel input[], int count) {
        GPixel src = GPixel_PackARGB(fColor.fA*255, fColor.fR*fColor.fA*255, fColor.fG*fColor.fA*255, fColor.fB*fColor.fA*255);

        for(int i = 0; i < count; i++) {
            output[i] = MyBlender::blend(src, input[i], fMode);
        }
    };

private:
    const GBlendMode fMode;
    const GColor fColor;
};

std::unique_ptr<GFilter> GCreateBlendFilter(GBlendMode mode, const GColor& src) {
    return std::unique_ptr<GFilter>(new MyFilter(mode, src));
}