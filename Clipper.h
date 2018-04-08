#include <utility>
#include "GPoint.h"
#include "GRect.h"
#include "GBitmap.h"

using namespace std;


class Clipper {
public:
    static pair<pair<GPoint, GPoint>*, int> clip(const GPoint*, int, GBitmap);
    static float slope(float, float, float, float);

private:
    static bool ascend(pair<GPoint, GPoint>, pair<GPoint, GPoint>);
    static pair<int, GPoint*> clipTop(int, const GPoint*);
    static pair<int, GPoint*> clipBottom(int, const GPoint*, int);
    static pair<int, GPoint*> clipLeft(int, const GPoint*);
    static pair<int, GPoint*> clipRight(int, const GPoint*, int);
};