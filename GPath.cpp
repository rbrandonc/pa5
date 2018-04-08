#include "GPath.h"
#include "GPoint.h"
#include "GMatrix.h"
#include <iostream>

GPath& GPath::addRect(const GRect& r, GPath::Direction dir) {
	// std::cout << "Add Rect" << std::endl;
	GPoint *points = new GPoint[4];
	// std::cout << r.left() << " " << r.right() << " " << r.top() << " " << r.bottom() << std::endl;
	if(dir == Direction::kCW_Direction) {
		points[0] = *(new GPoint({r.left(), r.top()}));
		points[1] = *(new GPoint({r.right(), r.top()}));
		points[2] = *(new GPoint({r.right(), r.bottom()}));
		points[3] = *(new GPoint({r.left(), r.bottom()}));
	} else {
		points[0] = *(new GPoint({r.left(), r.top()}));
		points[3] = *(new GPoint({r.right(), r.top()}));
		points[2] = *(new GPoint({r.right(), r.bottom()}));
		points[1] = *(new GPoint({r.left(), r.bottom()}));
	}
	
    return addPolygon(points, 4);
}

GPath& GPath::addPolygon(const GPoint pts[], int count) {
	// std::cout << "Add Poly" << std::endl;

	if(count < 1) return *this;


	moveTo(pts[0]);
	// std::cout << count << std::endl;
		// std::cout << "(" << pts[0].x() << ", " << pts[0].y() << "), ";

	for(int i = 1; i < count; i++) {
		// std::cout << "(" << pts[i].x() << ", " << pts[i].y() << "), ";

		lineTo(pts[i]);
	}

	// std::cout << std::endl;

    return *this;
}

GRect GPath::bounds() const {
    if(countPoints() == 0) return GRect::MakeXYWH(0,0,0,0);

    float left = fPts[0].x();
    float right = fPts[0].x();
    float top = fPts[0].y();
    float bottom = fPts[0].y();

    for(int i = 1; i < countPoints(); i++) {
    	if(fPts[i].x() < left) left = fPts[i].x();
    	if(fPts[i].x() > right) right = fPts[i].x();
    	if(fPts[i].y() < top) top = fPts[i].y();
    	if(fPts[i].y() > bottom) bottom = fPts[i].y();
    }

    return GRect::MakeXYWH(left, top, right-left, bottom-top);
}

void GPath::transform(const GMatrix& m) {
	// std::cout << "Transform" << std::endl;
	const GPoint *temp = &fPts[0];
	GPoint *temp2 = new GPoint[countPoints()];

	m.mapPoints(temp2, temp, countPoints());
	// std::cout << fPts[0].x() << " " << fPts[0].y() << std::endl;
	fPts = *(new std::vector<GPoint>(&temp2[0], &temp2[countPoints()]));
	// std::cout << fPts[0].x() << " " << fPts[0].y() << std::endl;

}