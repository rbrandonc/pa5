#include "GPath.h"
#include "GPoint.h"
#include "GMatrix.h"

GPath& GPath::addRect(const GRect& r, GPath::Direction dir) {
	GPoint points[4];
	if(dir == Direction::kCW_Direction) {
		points[0] = {r.left(), r.top()};
		points[1] = {r.right(), r.top()};
		points[2] = {r.right(), r.bottom()};
		points[3] = {r.left(), r.bottom()};
	} else {
		points[0] = {r.left(), r.top()};
		points[3] = {r.right(), r.top()};
		points[2] = {r.right(), r.bottom()};
		points[1] = {r.left(), r.bottom()};
	}
	
    return addPolygon(points, 4);
}

GPath& GPath::addPolygon(const GPoint pts[], int count) {
	if(count < 1) return *this;

	moveTo(pts[0]);

	for(int i = 1; i < count-1; i++) {
		lineTo(pts[i]);
	}

    return *this;
}

GRect GPath::bounds() const {
    if(countPoints() == 0) return GRect::MakeXYWH(0,0,0,0);

    float left = fPts[0].x();
    float right = fPts[0].x();
    float top = fPts[0].y();
    float bottom = fPts[0].y();

    for(int i = 1; i < countPoints()-1; i++) {
    	if(fPts[i].x() < left) left = fPts[i].x();
    	if(fPts[i].x() > right) right = fPts[i].x();
    	if(fPts[i].y() < top) top = fPts[i].y();
    	if(fPts[i].y() > bottom) bottom = fPts[i].y();
    }

    return GRect::MakeXYWH(left, top, right-left, bottom-top);
}

void GPath::transform(const GMatrix& m) {
	// const GPoint *temp = fPts;
	// m.mapPoints(fPts, temp, countPoints());
}