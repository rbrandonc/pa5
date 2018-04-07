#include "GMatrix.h"
#include "math.h"
#include <iostream>

void GMatrix::setIdentity() {
    (*this).set6(1, 0, 0, 0, 1, 0);
}

/**
 *  Set this matrix to translate by the specified amounts.
 */
void GMatrix::setTranslate(float tx, float ty) {
    (*this).set6(1, 0, tx, 0, 1, ty);
}

/**
 *  Set this matrix to scale by the specified amounts.
 */
void GMatrix::setScale(float sx, float sy) {
    (*this).set6(sx, 0, 0, 0, sy, 0);
}

/**
 *  Set this matrix to rotate by the specified radians.
 *
 *  Note: since positive-Y goes down, a small angle of rotation will increase Y.
 */
void GMatrix::setRotate(float radians) {
    (*this).set6(cosf(radians), -1*sinf(radians), 0,
               sinf(radians), cosf(radians), 0);
}

/**
 *  Set this matrix to the concatenation of the two specified matrices, such that the resulting
 *  matrix, when applied to points will have the same effect as first applying the primo matrix
 *  to the points, and then applying the secundo matrix to the resulting points.
 *
 *  Pts' = b * a * Pts
 */
void GMatrix::setConcat(const GMatrix& b, const GMatrix& a) {
    (*this).set6((b[0]*a[0]) + (b[1]*a[3]), (b[0]*a[1]) + (b[1]*a[4]), (b[0]*a[2]) + (b[1]*a[5]) + (b[2]),
                 (b[3]*a[0]) + (b[4]*a[3]), (b[3]*a[1]) + (b[4]*a[4]), (b[3]*a[2]) + (b[4]*a[5]) + (b[5]));
}

/*
 *  If this matrix is invertible, return true and (if not null) set the inverse parameter.
 *  If this matrix is not invertible, return false and ignore the inverse parameter.
 */
bool GMatrix::invert(GMatrix* inverse) const {
    if(inverse == nullptr) {
        std::cout << "fuck" << std::endl;
        return false;
    }
    //AE-BD Must be > 0
    GMatrix i = *inverse;
    float det = (fMat[0] * fMat[4]) - (fMat[1] * fMat[3]);
    if(det <= 0) {
        std::cout << "fuck" << std::endl;
        return false;

    }

    float a = fMat[4];
    float b = -1*fMat[1];
    float c = fMat[1]*fMat[5]-fMat[4]*fMat[2];
    float d = -1*fMat[3];
    float e = fMat[0];
    float f = fMat[3]*fMat[2]-fMat[0]*fMat[5];


    if(b == -0.0)
        b = 0;
    if(d == -0.0)
        d = 0;

    (*inverse).set6(a/det, b/det, c/det, d/det, e/det, f/det);
    return true;
}

/**
 *  Transform the set of points in src, storing the resulting points in dst, by applying this
 *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
 *
 *  Note: It is legal for src and dst to point to the same memory (however, they may not
 *  partially overlap). Thus the following is supported.
 *
 *  GPoint pts[] = { ... };
 *  matrix.mapPoints(pts, pts, count);
 */
void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for(int i = 0; i < count; i++) {
        //Ax + By + C
        float x = (fMat[0])*(src[i].x()) + (fMat[1])*(src[i].y()) + (fMat[2]);

        //Dx + Ey + F
        float y = (fMat[3])*(src[i].x()) + (fMat[4])*(src[i].y()) + (fMat[5]);
        dst[i] = GPoint::Make(x, y);
    }
}