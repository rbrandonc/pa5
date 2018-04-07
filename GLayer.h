#include <utility>
#include "GPoint.h"
#include "GRect.h"
#include "GBitmap.h"

using namespace std;

class GLayer {
public:
	GLayer(std::stack<GMatrix>& matrix, GRect* rect, GPaint* paint, GBitmap* bit) : ctm(matrix), bounds(rect), paint(paint), bitmap(bit) {}

	std::stack<GMatrix> ctm;
	GRect* bounds;
	GPaint* paint;
	GBitmap* bitmap;

private:
   
};