#include <utility>
#include "GPoint.h"
#include "GRect.h"
#include "GBitmap.h"
#include "Clipper.h"

using namespace std;

pair<pair<GPoint, GPoint>*, int> Clipper::clip(const GPoint points[], int count, GBitmap fDevice) {
//      Clip Top
		GPoint* clippedTop;
		int clippedSizeTop;
		tie(clippedSizeTop, clippedTop) = clipTop(count, points);

//      Clip Bottom
		GPoint* clippedBottom;
		int clippedSizeBottom;
		tie(clippedSizeBottom, clippedBottom) = clipBottom(clippedSizeTop, clippedTop, fDevice.height());
		delete(clippedTop);

//        Clip Left
		GPoint* clippedLeft;
		int clippedSizeLeft;
		tie(clippedSizeLeft, clippedLeft) = clipLeft(clippedSizeBottom, clippedBottom);
		delete(clippedBottom);

//        Clip Bottom
		GPoint* clippedRight;
		int clippedSizeRight;
		tie(clippedSizeRight, clippedRight) = clipRight(clippedSizeLeft, clippedLeft, fDevice.width());
		delete(clippedLeft);

		//create edges
		int numedges = 0;
		pair<GPoint, GPoint> *edges = new pair<GPoint, GPoint>[clippedSizeRight];

		for(int i = 0; i < clippedSizeRight; i++) {
			GPoint thispoint = clippedRight[i];
			GPoint nextpoint = clippedRight[(i+1)%clippedSizeRight];

			if(thispoint.y() != nextpoint.y()) {
				if(thispoint.y() < nextpoint.y()) {
					edges[numedges] = make_pair(thispoint, nextpoint);
					numedges++;
				} else {
					edges[numedges] = make_pair(nextpoint, thispoint);
					numedges++;
				}
			}
		}

//      sort edges
		sort(edges, edges + numedges, ascend);


		return make_pair(edges, numedges);
	}

	//Ascending sort for GPoints
	bool Clipper::ascend(pair<GPoint, GPoint> a, pair<GPoint, GPoint> b) {
		if(a.first.y() < b.first.y()) {
			return true;
		} else if (a.first.y() > b.first.y()) {
			return false;
		} else {
			if(a.first.x() < b.first.x()) {
				return true;
			} else if (a.first.x() > b.first.x()) {
				return false;
			} else {
				float slopeA = slope(a.first.x(), a.first.y(), a.second.x(), a.second.y());
				float slopeB = slope(b.first.x(), b.first.y(), b.second.x(), b.second.y());

				return slopeA < slopeB;
			}
		}
	}

	float Clipper::slope(float x, float y, float x2, float y2) {
		return (y2-y)/(x2-x);
	}


//    CLIP TOP
    pair<int, GPoint*> Clipper::clipTop(int count, const GPoint points[]) {
//        cout << "clip top" << endl;
		GPoint *clipped = new GPoint[count+1];
		int next_empty = 0;

		for(int i = 0; i < count; i++) {
			GPoint p1 = points[i];
			GPoint p2 = points[(i+1)%count];

			if(p1.y() < 0 && p2.y() < 0) {
				//both points bad, do nothing
			} else if(p1.y() >= 0 && p2.y() >= 0) {
				//If both points are good, add this one to clipped and move on
				clipped[next_empty] = p1;
				next_empty++;
			} else if(p1.y() >= 0 && p2.y() < 0) {
				//If this point is good and the next one is bad
				//Add this point to clipped, and add the intersection point
				clipped[next_empty] = p1;
				next_empty++;

				clipped[next_empty] = GPoint::Make(p1.x() + (p2.x() - p1.x()) * (0 - p1.y())/(p2.y() - p1.y()), 0);
				next_empty++;
			} else {
				//This point is bad, next is good
				//Add intersection and move on
				clipped[next_empty] = GPoint::Make(p1.x() + (p2.x() - p1.x()) * (0 - p1.y())/(p2.y() - p1.y()), 0);
				next_empty++;
			}
		}

		return make_pair(next_empty, clipped);
	}

//    CLIP BOTTOM
	pair<int, GPoint*> Clipper::clipBottom(int count, const GPoint points[], int boundary) {
//        cout << "clip bottom" << endl;
		GPoint *clipped = new GPoint[count+1];
		int next_empty = 0;

		for(int i = 0; i < count; i++) {
			GPoint p1 = points[i];
			GPoint p2 = points[(i+1)%count];

			if(p1.y() > boundary && p2.y() > boundary) {
				//both points bad, do nothing
			} else if(p1.y() <= boundary && p2.y() <= boundary) {
				//If both points are good, add this one to clipped and move on
				clipped[next_empty] = p1;
				next_empty++;
			} else if(p1.y() <= boundary && p2.y() > boundary) {
				//If this point is good and the next one is bad
				//Add this point to clipped, and add the intersection point
				clipped[next_empty] = p1;
				next_empty++;

				clipped[next_empty] = GPoint::Make(p1.x() + (p2.x() - p1.x()) * (boundary - p1.y())/(p2.y() - p1.y()), boundary);
				next_empty++;
			} else {
				//This point is bad, next is good
				//Add intersection and move on
				clipped[next_empty] = GPoint::Make(p1.x() + (p2.x() - p1.x()) * (boundary - p1.y())/(p2.y() - p1.y()), boundary);
				next_empty++;
			}
		}

		return make_pair(next_empty, clipped);
	}

	//        Clip Left
	pair<int, GPoint*> Clipper::clipLeft(int count, const GPoint points[]) {
//        cout << "clip left" << endl;
		GPoint *clipped = new GPoint[count + 1];
		int next_empty = 0;

		for (int i = 0; i < count; i++) {
			GPoint p1 = points[i];
			GPoint p2 = points[(i + 1) % count];

			if (p1.x() < 0 && p2.x() < 0) {
				//both points bad, do nothing
			} else if (p1.x() >= 0 && p2.x() >= 0) {
				//If both points are good, add this one to clipped and move on
				clipped[next_empty] = p1;
				next_empty++;
			} else if (p1.x() >= 0 && p2.x() < 0) {
				//If this point is good and the next one is bad
				//Add this point to clipped, and add the intersection point
				clipped[next_empty] = p1;
				next_empty++;

				clipped[next_empty] = GPoint::Make(0, ((p2.y()-p1.y())/(p2.x()-p1.x()))*(-1*p1.x())+p1.y());
				next_empty++;
			} else {
				//This point is bad, next is good
				//Add intersection and move on
				clipped[next_empty] = GPoint::Make(0, ((p2.y()-p1.y())/(p2.x()-p1.x()))*(-1*p1.x())+p1.y());
				next_empty++;
			}
		}
		return make_pair(next_empty, clipped);
	}

//        Clip Right
	pair<int, GPoint*> Clipper::clipRight(int count, const GPoint points[], int boundary) {
//        cout << "clip right" << endl;
		GPoint *clipped = new GPoint[count+1];
		int next_empty = 0;

		for(int i = 0; i < count; i++) {
			GPoint p1 = points[i];
			GPoint p2 = points[(i+1)%count];

			if(p1.x() > boundary && p2.x() > boundary) {
				//both points bad, do nothing
			} else if(p1.x() <= boundary && p2.x() <= boundary) {
				//If both points are good, add this one to clipped and move on
				clipped[next_empty] = p1;
				next_empty++;
			} else if(p1.x() <= boundary && p2.x() > boundary) {
				//If this point is good and the next one is bad
				//Add this point to clipped, and add the intersection point
				clipped[next_empty] = p1;
				next_empty++;

				clipped[next_empty] = GPoint::Make(boundary, ((p2.y()-p1.y())/(p2.x()-p1.x()))*(boundary-p1.x())+p1.y());
				next_empty++;
			} else {
				//This point is bad, next is good
				//Add intersection and move on
				clipped[next_empty] = GPoint::Make(boundary, ((p2.y()-p1.y())/(p2.x()-p1.x()))*(boundary-p1.x())+p1.y());
				next_empty++;
			}
		}
		return make_pair(next_empty, clipped);
	}