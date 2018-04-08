#include "GColor.h"
#include "GRect.h"
#include <iostream>
#include "GCanvas.h"
#include "GBitmap.h"
#include "GPoint.h"
#include <utility>
#include <math.h>
#include "Clipper.h"
#include <stack>
#include "MyShader.h"
#include "GMatrix.h"
#include "GLayer.h"
#include "GFilter.h"
#include "MyBlender.h"
#include "GBlendMode.h"
#include "GPath.h"

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {
        layers = *(new std::stack<GLayer>());
        GRect *startBounds = new GRect();
        startBounds->setWH(fDevice.width(), fDevice.height());

        GPixel *pixels = new GPixel[fDevice.height()*fDevice.width()];
        GBitmap *startBitmap = new GBitmap(fDevice.width(), fDevice.height(), fDevice.rowBytes(), fDevice.pixels(), fDevice.isOpaque());
        
        std::stack<GMatrix> startCTM = *(new std::stack<GMatrix>());
        GMatrix *startMatrix = new GMatrix();
        startCTM.push(*startMatrix);

        GPaint* startPaint = new GPaint();
        
        GLayer* startLayer = new GLayer(startCTM, startBounds, startPaint, startBitmap);
        layers.push(*startLayer);
    }

    void save() {
        cout << "Saving CTM" << endl;
        GMatrix *temp = new GMatrix();
        temp->set6(layers.top().ctm.top()[0], layers.top().ctm.top()[1], layers.top().ctm.top()[2], 
                    layers.top().ctm.top()[3], layers.top().ctm.top()[4], layers.top().ctm.top()[5]);
        layers.top().ctm.push(*temp);
    }

    void restore() {
        if(layers.top().ctm.size() > 1) {
            cout << "Restoring CTM" << endl;
            //Pop the ctm unless its empty
            layers.top().ctm.pop();
            return;

        } else {
            cout << "Restoring Layer" << endl;
            GLayer temp = *(new GLayer(layers.top()));
            
            if(layers.size() > 1) {
                layers.pop();
            }

            //blend with the old layers bitmap
            for(int i = 0; i < temp.bounds->height(); i++) {
                // cout << "asdf" << endl;
                if(temp.paint->getFilter() != nullptr) {
                    //If the paint has a filter, apply it before blending with previous layer

                    temp.paint->getFilter()->filter(temp.bitmap->getAddr(0,i), temp.bitmap->getAddr(0,i), temp.bounds->width());
                    for(int j = 0; j < temp.bitmap->width(); j++) {
                        GPaint test = GPaint();

                        drawPixels(test, temp.bitmap->getAddr(j,i), j, i, 1);
                    }
                } else {
                    for(int j = 0; j < temp.bounds->width(); j++) {
                        // drawPixels(GPaint paint, GPixel *pixel, int x, int y, int count)

                        GPoint mappedPoints[1];
                        GPoint *point = new GPoint();
                        point->set(j,i);
                        layers.top().ctm.top().mapPoints(mappedPoints, point, 1);
                        temp.ctm.top().mapPoints(mappedPoints, point, 1);
                        if(j < 0 || j >= temp.bitmap->width() || i >= temp.bitmap->height() || i < 0)
                            return;
                        drawPixels(*temp.paint, temp.bitmap->getAddr(j,i), j, i, 1);
                    }
                }
            }

        }   
    }

    void drawPath(const GPath& path, const GPaint& paint) {
        //Transform Path
        GPath p = path;
        p.transform(layers.top().ctm.top());

        if(p.countPoints() <= 3) {
            cout << "bad path (<3points)" << endl;
        }

        //Create edges from path
        GPoint *pts = new GPoint[2];
        pair<GPoint, GPoint> *edges = new pair<GPoint, GPoint>[p.countPoints()];
        GPath::Edger edger = *(new GPath::Edger(p));
        for (int i = 0; i < p.countPoints(); ++i) {
            edger.next(pts);
            GPoint a = GPoint::Make(pts[0].x(), pts[0].y());
            GPoint b = GPoint::Make(pts[1].x(), pts[1].y());
            edges[i] = make_pair(a, b);
        }

        //Draw inside bounds of path
        float startY = p.bounds().top();
        float endY = p.bounds().bottom();
        float startX = p.bounds().left();
        float endX = p.bounds().right();
        // if(endY > 120 || endX > 120)
        //     return;
        // cout << startX << "  " << endX << endl;
        // cout << startY << "  " << endY << endl;
        // cout << "---------" << endl;
        // for(int i = 0; i < p.countPoints(); i++) {
        //     // cout << "(" << edges[i].first.x() << ", " << edges[i].first.y() << ") -> (" << edges[i].second.x() << ", " << edges[i].second.y() << endl;
        // }

        // For every row
        for(int y = GRoundToInt(startY); y < GRoundToInt(endY); y++) {
            int winding = 0;
            int idx = 0;
            pair<int, int> *rowsToShade = new pair<int, int>[p.countPoints()];
            
            // For each pixel in the row check if it intersects a line
            for(int x = GRoundToInt(startX); x < GRoundToInt(endX)+1; x++) {                
              for(int edge = 0; edge < p.countPoints(); edge++) {
                //If the current point intersects a line, change winding accordingly
                if(edges[edge].first.y() != edges[edge].second.y() && intersects(GPoint::Make(x, y), edges[edge].first, edges[edge].second)) {
                    int windingWas = winding;
                    if(edges[edge].first.y() < edges[edge].second.y()) {
                        // cout << edges[edge].first.y() << " " << edges[edge].second.y() << endl;
                        winding--;
                    } else if(edges[edge].first.y() > edges[edge].second.y()) {
                        winding++;
                    }

                    // cout << winding << endl;
                    if(winding == 1 && windingWas != 2) {
                        // cout << "start: " << x << endl;
                        rowsToShade[idx].first = GRoundToInt(x);
                    } else if(winding == 0 && windingWas != -1) {
                        rowsToShade[idx].second = GRoundToInt(x);
                        // cout << "end: " << x << endl;
                        idx++;
                    }
                }

                // cout << winding;

              }
            }


            // cout << endl;

            // rowsToShade[0].first = startX;
            // rowsToShade[0].second = endX;

            //Shade all the subrows for this y value
            for(int i = 0; i <= idx; i++) {
                float leftX = rowsToShade[i].first;
                float rightX = rowsToShade[i].second;
                int size = GRoundToInt(rightX) - GRoundToInt(leftX);
                cout << leftX << " " << rightX << endl;

                if(size <= 0) {
                    break;
                }

                GPixel *storage = new GPixel[size];

                if(paint.getShader()) {
                    //If the paint has a shader, use that to shade this row of pixels

                    //Idk why I did this but it crashes without
                    if(!paint.getShader()->setContext(layers.top().ctm.top())) {
                        cout << "set context failed" << endl;
                        return;
                    }

                    paint.getShader()->shadeRow(GRoundToInt(leftX), y, GRoundToInt(rightX) - GRoundToInt(leftX), storage);
                } else {
                    //Otherwise draw the paints color over the bitmap from left to right
                    for(int x = 0; x < size; x++) {
                        GColor p = paint.getColor();
                        if((p.fA < 0 || p.fA > 1)
                           || (p.fR < 0 || p.fR > 1)
                           || (p.fG < 0 || p.fG > 1)
                           || (p.fB < 0 || p.fB > 1)) {
                            return;
                        } else {
    //                        cout << "- " << p.fA << " " << p.fR << " " << p.fG << " " << p.fB << endl;

                            if(p.fA == 0) {
                                p.fR = 0;
                                p.fG = 0;
                                p.fB = 0;
                            }
                            storage[x] = GPixel_PackARGB(p.fA*255, p.fR*p.fA*255, p.fG*p.fA*255, p.fB*p.fA*255);
                        }

                    }
                }

                if(paint.getFilter() != nullptr) {
                    paint.getFilter()->filter(storage, storage, size);
                }

                //Draw
                drawPixels(paint, storage, GRoundToInt(leftX), y, size);
            }
        }

    }

    bool intersects(GPoint a, GPoint b, GPoint c) {
        //If a is not in the boundary of the rect formed by b and c
        if((b.y() < c.y() && ((a.y() < b.y()) || (a.y() > c.y()))) 
            || (b.y() > c.y() && ((a.y() > b.y()) || (a.y() < c.y()))))
            return false;

        if((b.x() < c.x() && ((a.x() < b.x()) || (a.x() > c.x()))) 
            || (b.x() > c.x() && ((a.x() > b.x()) || (a.x() < c.x()))))
            return false;

        int x = GRoundToInt(((c.x()-b.x())/(c.y()-b.y()))*(a.y()-b.y())+b.x());
        if(a.x() == GRoundToInt(b.x()) && GRoundToInt(b.x()) == GRoundToInt(c.x())) {
            // cout << "(" << a.x() << ", " << a.y() << ") ";
            //         cout << "(" << b.x() << ", " << b.y() << ") ";
            //         cout << "(" << c.x() << ", " << c.y() << ") ";
            //         cout << "(" << x << ", " << a.y() << ") " << endl;;
            return true;
        }

        //Calculate the x value 
        if(a.x() >= x && a.x()-1 < x) {
            // cout << "(" << a.x() << ", " << a.y() << ") ";
            // cout << "(" << b.x() << ", " << b.y() << ") ";
            // cout << "(" << c.x() << ", " << c.y() << ") ";
            // cout << "(" << x << ", " << a.y() << ") " << endl;;
            return true;
        }

        return false;

    }

    void concat(const GMatrix& a) {
        // cout << "concat" << endl;
        // cout << a[0] << " " << a[1] << " "<< a[2] << " "<< endl;
        // cout << a[3] << " "<< a[4] << " "<< a[5] << endl;
        // cout << "---------------" << endl;
        layers.top().ctm.top().preConcat(a);
    }


    void drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) override {
        if(count <= 1)
            return;

            // cout << ctm[0] << " " << ctm[1] << " "<< ctm[2] << " "<< endl;
            // cout << ctm[3] << " "<< ctm[4] << " "<< ctm[5] << endl;
            // cout << "===============" << endl;

        GPoint mappedPoints[count+1];
        layers.top().ctm.top().mapPoints(mappedPoints, points, count);

        //First we need to clip the points inside the bounds of the screen
        pair<GPoint, GPoint> *edges;
        int numedges;
        tie(edges, numedges) = Clipper::clip(mappedPoints, count, fDevice);


        //Then we can fill in the inside of the polygon

        //from the lowest y to the highest y
        //draw across the screen line by line
        float startY = edges[0].first.y();
        float endY = edges[numedges-1].second.y();
        pair<GPoint, GPoint> *left = &edges[0];
        pair<GPoint, GPoint> *right = &edges[1];
        float leftSlope = (left->second.x()-left->first.x())/(left->second.y()-left->first.y());
        float rightSlope = (right->second.x()-right->first.x())/(right->second.y()-right->first.y());
        float leftX = left->first.x() + ((startY - left->first.y()) * leftSlope);
        float rightX = right->first.x() + ((startY - right->first.y()) * rightSlope);
        int nextedge = 2;
        for(int y = GRoundToInt(startY); y < GRoundToInt(endY); y++) {
            //If we are past the bottom of the entire polygon, break
//            if(left.second.y() == right.second.y() && left.second.x() == right.second.x()) { break; }
            if(leftX < 0) {
                leftX = 0;
            }
//            cout << "asdf" << fDevice.devWidth() << endl;
            if(rightX > fDevice.width()) {
                rightX = fDevice.width();
            }
//            for(int i = 0; i < count; i++) {
//                cout << edges[i].second.y() << " " << edges[i].second.y() << endl;
//            }
            //If one of our edges runs out, get the next one
            if(y > right->second.y() || y > left->second.y()) {
                if(y > left->second.y()) {
                    left = &edges[nextedge];
                    nextedge++;
                    leftSlope = (left->second.x()-left->first.x())/(left->second.y()-left->first.y());
                    leftX = left->first.x() + ((y - left->first.y()) * leftSlope);
                }

                if(y > right->second.y()) {
                    right = &edges[nextedge];
                    nextedge++;
                    rightSlope = (right->second.x()-right->first.x())/(right->second.y()-right->first.y());
                    rightX = right->first.x() + ((y - right->first.y()) * rightSlope);
                }
            }

//            Fill in pixels left to right
            int size = GRoundToInt(rightX) - GRoundToInt(leftX);
            if(size > 0) {
                GPixel *storage = new GPixel[size];

                if(paint.getShader()) {
                    //If the paint has a shader, use that to shade this row of pixels

                    //Idk why I did this but it crashes without
                    if(!paint.getShader()->setContext(layers.top().ctm.top())) {
                        cout << "set context failed" << endl;
                        return;
                    }

                    paint.getShader()->shadeRow(GRoundToInt(leftX), y, GRoundToInt(rightX) - GRoundToInt(leftX), storage);
                } else {
                    //Otherwise draw the paints color over the bitmap from left to right
                    for(int x = 0; x < size; x++) {
                        GColor p = paint.getColor();
                        if((p.fA < 0 || p.fA > 1)
                           || (p.fR < 0 || p.fR > 1)
                           || (p.fG < 0 || p.fG > 1)
                           || (p.fB < 0 || p.fB > 1)) {
                            return;
                        } else {
    //                        cout << "- " << p.fA << " " << p.fR << " " << p.fG << " " << p.fB << endl;

                            if(p.fA == 0) {
                                p.fR = 0;
                                p.fG = 0;
                                p.fB = 0;
                            }
                            storage[x] = GPixel_PackARGB(p.fA*255, p.fR*p.fA*255, p.fG*p.fA*255, p.fB*p.fA*255);
                        }

                    }
                }

                if(paint.getFilter() != nullptr) {
                    paint.getFilter()->filter(storage, storage, size);
                }

                //Draw
                drawPixels(paint, storage, GRoundToInt(leftX), y, size);
            }

            //compute where we start and end for this y value
            leftX += leftSlope;
            rightX += rightSlope;
        }

    }

    void drawPaint(const GPaint& paint) override {
        GPoint points[4];
        points[0] = GPoint::Make(0, 0);
        points[1] = GPoint::Make(layers.top().bounds->width(), 0);
        points[2] = GPoint::Make(layers.top().bounds->width(), layers.top().bounds->height());
        points[3] = GPoint::Make(0, layers.top().bounds->width());
        drawConvexPolygon(points, 4, paint);

    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
        GPoint points[4];
        points[0] = GPoint::Make(rect.left(), rect.top());
        points[1] = GPoint::Make(rect.right(), rect.top());
        points[2] = GPoint::Make(rect.right(), rect.bottom());
        points[3] = GPoint::Make(rect.left(), rect.bottom());
        drawConvexPolygon(points, 4, paint);
    }

    void drawPixels(GPaint paint, GPixel *pixel, int x, int y, int count) {
        //Draw a row of pixels onto the current bitmap of length = count
        for(int i = 0; i < count; i++) {
            //make sure pixel is inside screen area
            if(x+i < 0 || x+i >= layers.top().bitmap->width()
                || y < 0 || y >= layers.top().bitmap->height()) break;

            GPixel *address = layers.top().bitmap->getAddr(x + i, y);
            GPixel dest = *address;

            const GPixel* src = new GPixel(*pixel);

            int destA = GPixel_GetA(dest);
            int destR = GPixel_GetR(dest);
            int destG = GPixel_GetG(dest);
            int destB = GPixel_GetB(dest);

            //Compute the premul values of the given color
            if((GPixel_GetA(pixel[i]) < 0 || GPixel_GetA(pixel[i]) > 255)
               || (GPixel_GetR(pixel[i]) < 0 || GPixel_GetR(pixel[i]) > 255)
               || (GPixel_GetG(pixel[i]) < 0 || GPixel_GetG(pixel[i]) > 255)
               || (GPixel_GetB(pixel[i]) < 0 || GPixel_GetB(pixel[i]) > 255)) {
                // return;
            }

            int srcA = GPixel_GetA(pixel[i]);
            int srcR = GPixel_GetR(pixel[i]);
            int srcG = GPixel_GetG(pixel[i]);
            int srcB = GPixel_GetB(pixel[i]);

            if(srcR > srcA || srcG > srcA || srcB > srcA || destR > destA || destG > destA || destB > destA){
                // return;
            }

            GPixel* temp = new GPixel();
            GPixel temp2;
            // if(paint.getFilter() != nullptr) {
            //     paint.getFilter()->filter(temp, src, 1);
            //     temp2 = MyBlender::blend(pixel[i], *temp, paint.getBlendMode());
            // } else {
            // }

                temp2 = MyBlender::blend(pixel[i], dest, paint.getBlendMode());
            

            *address = temp2;
        }
    }

private:
    const GBitmap fDevice;
    std::stack<GLayer> layers;
    GRect fBounds;

protected:
    virtual void onSaveLayer(const GRect* bounds, const GPaint& paint) {
        cout << "Saving Layer" << endl;

        if(bounds != nullptr) {
            // Check if new bounds intersect current bounds
            // If not, new rectangle is 0,0,0,0
            // GRect bound;

            // GPoint oldPoints[4] = {GPoint::Make(bounds->left(), bounds->top()), GPoint::Make(bounds->right(), bounds->top()), 
            //                         GPoint::Make(bounds->left(), bounds->bottom()), GPoint::Make(bounds->right(), bounds->bottom())};        

            // GPoint mappedNew[4];
            // oldCTM.mapPoints(mappedNew, oldPoints, 4);
            // GRect newBounds = GRect::MakeLTRB(mappedNew[0].x(), mappedNew[0].y(), mappedNew[3].x(), mappedNew[3].y());

            // GMatrix currentCTM = ctm.top();
            // GPoint mappedOld[4];
            // currentCTM.mapPoints(mappedOld, oldPoints, 4);
            // GRect oldBounds = GRect::MakeLTRB(mappedOld[0].x(), mappedOld[0].y(), mappedOld[3].x(), mappedOld[3].y());

            // GRect bitBounds = GRect::MakeWH(fDevice.width(), fDevice.height());
            // if(oldBounds.intersects(newBounds) && bitBounds.intersects(newBounds)) {
            //     bound = *bounds;
            // } else {
            //     bound = GRect::MakeWH(0,0);
            // }

            // cout << bound.left() << " " << bound.top() << " " << bound.right() << " " << bound.bottom() << endl;
            // cout << bound.left() << " " << bound.top() << " " << bound.right() << " " << bound.bottom() << endl;

            // old = new GLayer(ctm, fBounds, pain, bitmap);
            // fBounds = bound;


            GRect *newBounds = new GRect();
            newBounds->setWH(bounds->width(), bounds->height());

            GPixel *pixels = new GPixel[fDevice.height()*fDevice.width()];
            GBitmap *newBitmap = new GBitmap(fDevice.width(), fDevice.height(), fDevice.rowBytes(), pixels, fDevice.isOpaque());
            
            std::stack<GMatrix> newCTM = *(new std::stack<GMatrix>());
            GMatrix *newMatrix = new GMatrix(layers.top().ctm.top());
            newCTM.push(*newMatrix);

            GPaint* newPaint = new GPaint();
            newPaint->setBlendMode(paint.getBlendMode());
            newPaint->setColor(paint.getColor());
            newPaint->setShader(paint.getShader());
            newPaint->setFilter(paint.getFilter());

            GLayer newLayer = *(new GLayer(newCTM, newBounds, newPaint, newBitmap));

            layers.push(newLayer);

        } else {
            GRect *newBounds = new GRect();
            newBounds->setWH(fDevice.width(), fDevice.height());

            GPixel *pixels = new GPixel[fDevice.height()*fDevice.width()];
            GBitmap *newBitmap = new GBitmap(fDevice.width(), fDevice.height(), fDevice.rowBytes(), pixels, fDevice.isOpaque());
            
            std::stack<GMatrix> newCTM = *(new std::stack<GMatrix>());
            GMatrix *newMatrix = new GMatrix(layers.top().ctm.top());
            newCTM.push(*newMatrix);

            GPaint* newPaint = new GPaint();
            newPaint->setBlendMode(paint.getBlendMode());
            newPaint->setColor(paint.getColor());
            newPaint->setShader(paint.getShader());
            newPaint->setFilter(paint.getFilter());

            GLayer newLayer = *(new GLayer(newCTM, newBounds, newPaint, newBitmap));

            layers.push(newLayer);
        }
    };
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}