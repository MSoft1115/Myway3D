/**
 * Copyright (C) 2011 Jorge Jimenez (jorge@iryoku.com). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holders.
 */


#include "Camera.h"
using namespace std;


#pragma region Useful Macros from DXUT (copy-pasted here as we prefer this to be as self-contained as possible)
#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x) { hr = (x); if (FAILED(hr)) { DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x) { hr = (x); if (FAILED(hr)) { return DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
#endif
#else
#ifndef V
#define V(x) { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x) { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if (p) { delete (p); (p) = NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }
#endif
#pragma endregion


LRESULT Camera::handleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch(msg) {
        case WM_LBUTTONDOWN: {
            POINT point;
            GetCursorPos(&point);
            mousePos = D3DXVECTOR2(float(point.x), float(point.y));
            draggingLeft = true;
            attenuation = 4.0f;
            SetCapture(hwnd);
            return true;
        }
        case WM_LBUTTONUP:
            draggingLeft = false;
            if (wparam & MK_CONTROL)
                attenuation = 0.0f;
            else
                attenuation = 4.0f;
            ReleaseCapture();
            return true;
        case WM_RBUTTONDOWN: {
            POINT point;
            GetCursorPos(&point);
            mousePos = D3DXVECTOR2(float(point.x), float(point.y));
            draggingRight = true;
            SetCapture(hwnd);
            return true;
        }
        case WM_RBUTTONUP: {
            draggingRight = false;
            ReleaseCapture();
            return true;
        }
        case WM_MBUTTONDOWN: {
            POINT point;
            GetCursorPos(&point);
            mousePos = D3DXVECTOR2(float(point.x), float(point.y));
            draggingMiddle = true;
            SetCapture(hwnd);
            return true;
        }
        case WM_MBUTTONUP: {
            draggingMiddle = false;
            ReleaseCapture();
            return true;
        }
        case WM_MOUSEMOVE: {
            POINT point;
            GetCursorPos(&point);
            D3DXVECTOR2 newMousePos = D3DXVECTOR2(float(point.x), float(point.y));
            if (draggingLeft) {
                D3DXVECTOR2 delta = newMousePos - mousePos;
                angularVelocity -= delta;
                mousePos = newMousePos;
            }
            if (draggingMiddle) {
                D3DXVECTOR2 delta = newMousePos - mousePos;
                updatePosition(delta);
                mousePos = newMousePos;
            }
            if (draggingRight) {
                distance += (newMousePos.y - mousePos.y) / 75.0f;
                mousePos = newMousePos;
            }
            return true;
        }
        case WM_MOUSEWHEEL: {
            short value = short(HIWORD(wparam));
            distance -= float(value) / 400.0f;
            return 0;
        }
        case WM_CAPTURECHANGED: {
            if ((HWND) lparam != hwnd) {
                draggingLeft = false;
                draggingMiddle = false;
                draggingRight = false;
            }
            break;
        }
    }
    return 0;
}


void Camera::frameMove(FLOAT elapsedTime) {
    angle += angularVelocity * elapsedTime / 150.0f;
    angularVelocity = angularVelocity / (1.0f + attenuation * elapsedTime);
    build();
}


void Camera::setProjection(float fov, float aspect, float zn, float zf) {
    D3DXMatrixPerspectiveFovLH(&projection, fov, aspect, zn, zf);
}


void Camera::setJitteredProjection(float fov, float aspect, float zn, float zf, const D3DXVECTOR2 &jitter) {
    projection = jitteredPerspective(fov, aspect, zn, zf, jitter.x, jitter.y, 0.0f, 0.0f, 1.0f);
}


void Camera::build() {
    D3DXMatrixTranslation(&view, -panPosition.x, -panPosition.y, distance);

    D3DXMATRIX t;    
    D3DXMatrixRotationX(&t, angle.y);
    view = t * view;

    D3DXMatrixRotationZ(&t, angle.x);
    view = t * view;

    D3DXMATRIX viewInverse;
    float det;
    D3DXMatrixInverse(&viewInverse, &det, &view);

    D3DXVECTOR4 lookAtPosition4 = D3DXVECTOR4(0.0f, 0.0f, distance, 1.0f);
    D3DXVec4Transform(&lookAtPosition4, &lookAtPosition4, &viewInverse);
    lookAtPosition = D3DXVECTOR3(lookAtPosition4);
        
    D3DXVECTOR4 eyePosition4 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
    D3DXVec4Transform(&eyePosition4, &eyePosition4, &viewInverse);
    eyePosition = D3DXVECTOR3(eyePosition4);
}


void Camera::updatePosition(D3DXVECTOR2 delta) {
    delta.x /= viewportSize.x / 2.0f;
    delta.y /= viewportSize.y / 2.0f;

    D3DXMATRIX transform;
    D3DXMatrixTranslation(&transform, 0.0f, 0.0f, distance);
    transform *= projection;
    
    D3DXMATRIX inverse;
    float det;
    D3DXMatrixInverse(&inverse, &det, &transform);

    D3DXVECTOR4 t = D3DXVECTOR4(panPosition.x, panPosition.y, 0.0f, 1.0f);
    D3DXVec4Transform(&t, &t, &transform);
    t.x -= delta.x * t.w;
    t.y += delta.y * t.w;
    D3DXVec4Transform(&t, &t, &inverse);
    panPosition = D3DXVECTOR2(t);
}


D3DXMATRIX Camera::jitteredFrustum(float left, float right, float bottom, float top, float zn, float zf, 
                                   float pixdx, float pixdy, float eyedx, float eyedy, float focus) const {
    float xwsize, ywsize; 
    float dx, dy;

    xwsize = right - left;
    ywsize = top - bottom;

    // Translate the screen space jitter distances into near clipping plane distances:
    dx = -(pixdx * xwsize / viewportSize.x + eyedx * zn / focus);
    dy = -(pixdy * ywsize / viewportSize.y + eyedy * zn / focus);

    D3DXMATRIX transform;
    D3DXMatrixPerspectiveOffCenterLH(&transform, left + dx, right + dx, bottom + dy, top + dy, zn, zf);
    return transform;
}


D3DXMATRIX Camera::jitteredPerspective(float fovy, float aspect, float zn, float zf, 
                                       float pixdx, float pixdy, float eyedx, float eyedy, float focus) const {
    float fov2, left, right, bottom, top;
    fov2 = fovy * 0.5f;

    top = zn * tanf(fov2);
    bottom = -top;
    right = top * aspect;
    left = -right;

    return jitteredFrustum(left, right, bottom, top, zn, zf, pixdx, pixdy, eyedx, eyedy, focus);
}


ostream &operator <<(ostream &os, const Camera &camera) {
    os << camera.distance << endl;
    os << camera.angle.x << " " << camera.angle.y << endl;
    os << camera.panPosition.x << " " << camera.panPosition.y << endl;
    os << camera.angularVelocity.x << " " << camera.angularVelocity.y << endl;
    os << camera.attenuation << endl;
    return os;
}


istream &operator >>(istream &is, Camera &camera) {
    is >> camera.distance;
    is >> camera.angle.x >> camera.angle.y;
    is >> camera.panPosition.x >> camera.panPosition.y;
    is >> camera.angularVelocity.x >> camera.angularVelocity.y;
    is >> camera.attenuation;
    return is;
}
