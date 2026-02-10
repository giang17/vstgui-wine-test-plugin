#pragma once

#include "vstgui/lib/cview.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cgraphicspath.h"
#include "vstgui/lib/ccolor.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace VSTGUI;

//------------------------------------------------------------------------
class TestLinesView : public CView
{
public:
    TestLinesView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        for (int i = 0; i < 5; i++)
        {
            CCoord y = r.top + 10 + i * 15;
            CCoord width = 1.0 + i * 1.0;
            context->setLineWidth (width);
            context->setFrameColor (CColor (255, 255, 255, 255));
            context->drawLine (CPoint (r.left + 5, y), CPoint (r.left + 105, y));
        }

        context->setLineWidth (2.0);
        context->setFrameColor (CColor (80, 120, 255, 255));
        for (int i = 0; i < 5; i++)
        {
            CCoord offset = i * 20;
            context->drawLine (
                CPoint (r.left + 120, r.top + 10 + offset),
                CPoint (r.left + 170, r.top + 70 + offset));
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class SineWaveformView : public CView
{
public:
    SineWaveformView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        if (auto path = owned (context->createGraphicsPath ()))
        {
            int segments = 40;
            CCoord centerY = r.top + r.getHeight () * 0.5;
            CCoord amplitude = r.getHeight () * 0.4;

            path->beginSubpath (CPoint (r.left, centerY));
            for (int i = 1; i <= segments; i++)
            {
                double t = (double)i / segments;
                CCoord px = r.left + t * r.getWidth ();
                CCoord py = centerY - amplitude * sin (t * 2.0 * M_PI);
                path->addLine (CPoint (px, py));
            }

            context->setFrameColor (CColor (255, 80, 80, 255));
            context->setLineWidth (2.0);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class SquareWaveformView : public CView
{
public:
    SquareWaveformView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        if (auto path = owned (context->createGraphicsPath ()))
        {
            CCoord stepWidth = r.getWidth () / 8.0;
            path->beginSubpath (CPoint (r.left, r.bottom));

            for (int i = 0; i < 8; i++)
            {
                CCoord sx = r.left + i * stepWidth;
                CCoord top = r.top;
                CCoord bot = r.bottom;
                if (i % 2 == 0)
                {
                    path->addLine (CPoint (sx, top));
                    path->addLine (CPoint (sx + stepWidth, top));
                }
                else
                {
                    path->addLine (CPoint (sx, bot));
                    path->addLine (CPoint (sx + stepWidth, bot));
                }
            }

            context->setFrameColor (CColor (80, 120, 255, 255));
            context->setLineWidth (2.0);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class TriangleWaveformView : public CView
{
public:
    TriangleWaveformView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        if (auto path = owned (context->createGraphicsPath ()))
        {
            CCoord centerY = r.top + r.getHeight () * 0.5;
            CCoord periodW = r.getWidth () / 4.0;
            CCoord quarterW = periodW / 4.0;
            path->beginSubpath (CPoint (r.left, centerY));

            for (int i = 0; i < 4; i++)
            {
                CCoord bx = r.left + i * periodW;
                path->addLine (CPoint (bx + quarterW, r.top));
                path->addLine (CPoint (bx + quarterW * 2.0, centerY));
                path->addLine (CPoint (bx + quarterW * 3.0, r.bottom));
                path->addLine (CPoint (bx + periodW, centerY));
            }

            context->setFrameColor (CColor (255, 255, 255, 255));
            context->setLineWidth (1.5);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class StressWaveformView : public CView
{
public:
    int segments;
    float strokeWidth;
    CColor color;

    StressWaveformView (const CRect& size, int segs, float sw, CColor col)
        : CView (size), segments (segs), strokeWidth (sw), color (col) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        if (auto path = owned (context->createGraphicsPath ()))
        {
            CCoord centerY = r.top + r.getHeight () * 0.5;
            CCoord amplitude = r.getHeight () * 0.4;
            path->beginSubpath (CPoint (r.left, centerY));

            for (int i = 1; i <= segments; i++)
            {
                double t = (double)i / segments;
                CCoord px = r.left + t * r.getWidth ();
                CCoord py = centerY - amplitude * sin (t * 6.0 * M_PI)
                           + 0.3 * sin (t * 47.0 * M_PI);
                path->addLine (CPoint (px, py));
            }

            context->setFrameColor (color);
            context->setLineWidth (strokeWidth);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class ZigzagStressView : public CView
{
public:
    int segments;
    float strokeWidth;
    CColor color;

    ZigzagStressView (const CRect& size, int segs, float sw, CColor col)
        : CView (size), segments (segs), strokeWidth (sw), color (col) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        if (auto path = owned (context->createGraphicsPath ()))
        {
            path->beginSubpath (CPoint (r.left, r.top));

            for (int i = 1; i <= segments; i++)
            {
                CCoord px = r.left + ((double)i / segments) * r.getWidth ();
                CCoord py = (i % 2 == 0) ? r.top : r.bottom;
                path->addLine (CPoint (px, py));
            }

            context->setFrameColor (color);
            context->setLineWidth (strokeWidth);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class CustomKnobView : public CView
{
public:
    float value = 0.5f;

    CustomKnobView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();
        CCoord centerX = r.left + r.getWidth () * 0.5;
        CCoord centerY = r.top + r.getHeight () * 0.5;
        CCoord radius = std::min (r.getWidth (), r.getHeight ()) * 0.45;

        CRect ellipseRect (centerX - radius, centerY - radius,
                           centerX + radius, centerY + radius);
        context->setFillColor (CColor (180, 180, 180, 255));
        context->drawEllipse (ellipseRect, kDrawFilled);
        context->setFrameColor (CColor (255, 255, 255, 255));
        context->setLineWidth (2.0);
        context->drawEllipse (ellipseRect, kDrawStroked);

        double angle = (120.0 + value * 270.0) * M_PI / 180.0;
        CCoord indX = centerX + (radius - 10) * cos (angle);
        CCoord indY = centerY + (radius - 10) * sin (angle);
        context->setFrameColor (CColor (255, 80, 80, 255));
        context->setLineWidth (3.0);
        context->drawLine (CPoint (centerX, centerY), CPoint (indX, indY));

        if (auto path = owned (context->createGraphicsPath ()))
        {
            CCoord arcRadius = radius - 5;
            double startAngle = 120.0;
            double endAngle = 120.0 + value * 270.0;

            CRect arcRect (centerX - arcRadius, centerY - arcRadius,
                           centerX + arcRadius, centerY + arcRadius);
            path->addArc (arcRect, startAngle, endAngle, true);

            context->setFrameColor (CColor (255, 80, 80, 255));
            context->setLineWidth (4.0);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        setDirty (false);
    }

    CMouseEventResult onMouseDown (CPoint& where, const CButtonState& buttons) override
    {
        if (buttons.isLeftButton ())
        {
            lastY = where.y;
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    CMouseEventResult onMouseMoved (CPoint& where, const CButtonState& buttons) override
    {
        if (buttons.isLeftButton ())
        {
            float delta = (float)(lastY - where.y) * 0.005f;
            value += delta;
            if (value < 0.f) value = 0.f;
            if (value > 1.f) value = 1.f;
            lastY = where.y;
            invalid ();
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

private:
    CCoord lastY = 0;
};

//------------------------------------------------------------------------
class NinePartRectView : public CView
{
public:
    NinePartRectView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        auto r = getViewSize ();
        CCoord cs = 10;

        context->setFillColor (CColor (255, 80, 80, 255));
        context->drawRect (CRect (r.left, r.top, r.left + cs, r.top + cs), kDrawFilled);
        context->drawRect (CRect (r.right - cs, r.top, r.right, r.top + cs), kDrawFilled);
        context->drawRect (CRect (r.left, r.bottom - cs, r.left + cs, r.bottom), kDrawFilled);
        context->drawRect (CRect (r.right - cs, r.bottom - cs, r.right, r.bottom), kDrawFilled);

        context->setFillColor (CColor (80, 120, 255, 255));
        context->drawRect (CRect (r.left + cs, r.top, r.right - cs, r.top + cs), kDrawFilled);
        context->drawRect (CRect (r.left + cs, r.bottom - cs, r.right - cs, r.bottom), kDrawFilled);
        context->drawRect (CRect (r.left, r.top + cs, r.left + cs, r.bottom - cs), kDrawFilled);
        context->drawRect (CRect (r.right - cs, r.top + cs, r.right, r.bottom - cs), kDrawFilled);

        context->setFillColor (CColor (180, 180, 180, 255));
        context->drawRect (CRect (r.left + cs, r.top + cs, r.right - cs, r.bottom - cs), kDrawFilled);

        context->setFrameColor (CColor (255, 255, 255, 255));
        context->setLineWidth (1.0);
        context->drawRect (r, kDrawStroked);

        setDirty (false);
    }
};

//------------------------------------------------------------------------
class ShapesView : public CView
{
public:
    ShapesView (const CRect& size) : CView (size) {}

    void draw (CDrawContext* context) override
    {
        context->setDrawMode (kAntiAliasing);
        auto r = getViewSize ();

        CRect rrRect (r.left, r.top, r.left + 160, r.top + 60);
        if (auto path = owned (context->createGraphicsPath ()))
        {
            CCoord radius = 10;
            path->addRoundRect (rrRect, radius);
            context->setFillColor (CColor (100, 100, 100, 255));
            context->drawGraphicsPath (path, CDrawContext::kPathFilled);
            context->setFrameColor (CColor (255, 255, 255, 255));
            context->setLineWidth (2.0);
            context->drawGraphicsPath (path, CDrawContext::kPathStroked);
        }

        CRect ellipseRect (r.left + 10, r.top + 80, r.left + 110, r.top + 120);
        context->setFillColor (CColor (100, 100, 100, 255));
        context->drawEllipse (ellipseRect, kDrawFilled);
        context->setFrameColor (CColor (255, 255, 255, 255));
        context->setLineWidth (2.0);
        context->drawEllipse (ellipseRect, kDrawStroked);

        context->setFrameColor (CColor (80, 120, 255, 255));
        context->setLineWidth (2.0);
        for (int i = 0; i < 5; i++)
        {
            CCoord cx = r.left + 130 + i * 30;
            CCoord cy = r.top + 80 + i * 8;
            CRect circ (cx - 12, cy - 12, cx + 12, cy + 12);
            context->drawEllipse (circ, kDrawStroked);
        }

        setDirty (false);
    }
};
