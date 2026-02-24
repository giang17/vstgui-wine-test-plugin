#include "editor.h"
#include "pluginparamids.h"
#include "waveformviews.h"

#include "vstgui/lib/cframe.h"
#include "vstgui/lib/cviewcontainer.h"
#include "vstgui/lib/ccolor.h"
#include "vstgui/lib/cfont.h"
#include "vstgui/lib/cvstguitimer.h"
#include "vstgui/lib/controls/ctextlabel.h"
#include "vstgui/lib/controls/ctextedit.h"
#include "vstgui/lib/platform/platformfactory.h"
#include "vstgui/lib/platform/win32/win32factory.h"

using namespace VSTGUI;

namespace WineTest {

static const CColor kBgColor = CColor (40, 40, 40, 255);
static const CColor kWhite = CColor (255, 255, 255, 255);
static const CColor kRed = CColor (255, 80, 80, 255);
static const CColor kBlue = CColor (80, 120, 255, 255);
static const CColor kGreen = CColor (80, 200, 80, 255);
static const CColor kBlack = CColor (0, 0, 0, 255);
static const CColor kGray = CColor (180, 180, 180, 255);
static const CColor kLabelBg = CColor (60, 60, 60, 255);

//------------------------------------------------------------------------
// SimpleKnobView — custom-drawn knob control (no bitmap needed)
//------------------------------------------------------------------------
class SimpleKnobView : public CControl
{
public:
    SimpleKnobView (const CRect& r, IControlListener* listener, int32_t tag)
        : CControl (r, listener, tag)
    {
        setMin (0.f);
        setMax (1.f);
        setValue (0.5f);
    }

    void draw (CDrawContext* context) override
    {
        auto r = getViewSize ();
        context->setFillColor (kLabelBg);
        context->drawRect (r, kDrawFilled);

        auto cx = r.getCenter ().x;
        auto cy = r.getCenter ().y;
        auto radius = std::min (r.getWidth (), r.getHeight ()) * 0.4;

        // Draw knob circle
        CRect knobRect (cx - radius, cy - radius, cx + radius, cy + radius);
        context->setFillColor (kGray);
        context->drawEllipse (knobRect, kDrawFilled);
        context->setFrameColor (kWhite);
        context->setLineWidth (1.5);
        context->drawEllipse (knobRect, kDrawStroked);

        // Draw value indicator line
        double angle = (0.75 + getValue () * 1.5) * M_PI; // 225° to 495° (135°)
        auto ex = cx + radius * 0.75 * cos (angle);
        auto ey = cy + radius * 0.75 * sin (angle);
        context->setFrameColor (kRed);
        context->setLineWidth (2.0);
        context->drawLine (CPoint (cx, cy), CPoint (ex, ey));

        setDirty (false);
    }

    CMouseEventResult onMouseDown (CPoint& where, const CButtonState& buttons) override
    {
        if (buttons.isLeftButton ())
        {
            beginEdit ();
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
            float newVal = getValue () + delta;
            if (newVal < 0.f) newVal = 0.f;
            if (newVal > 1.f) newVal = 1.f;
            setValue (newVal);
            valueChanged ();
            invalid ();
            lastY = where.y;
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    CMouseEventResult onMouseUp (CPoint& where, const CButtonState& buttons) override
    {
        endEdit ();
        return kMouseEventHandled;
    }

    CLASS_METHODS (SimpleKnobView, CControl)
private:
    CCoord lastY = 0;
};

//------------------------------------------------------------------------
// SimpleSliderView — custom-drawn horizontal slider (no bitmap needed)
//------------------------------------------------------------------------
class SimpleSliderView : public CControl
{
public:
    SimpleSliderView (const CRect& r, IControlListener* listener, int32_t tag)
        : CControl (r, listener, tag)
    {
        setMin (0.f);
        setMax (1.f);
        setValue (0.5f);
    }

    void draw (CDrawContext* context) override
    {
        auto r = getViewSize ();
        // Track background
        context->setFillColor (kLabelBg);
        context->drawRect (r, kDrawFilled);
        // Track border
        context->setFrameColor (kWhite);
        context->setLineWidth (1.0);
        context->drawRect (r, kDrawStroked);

        // Filled portion
        CRect filled = r;
        filled.right = filled.left + filled.getWidth () * getValue ();
        context->setFillColor (kRed);
        context->drawRect (filled, kDrawFilled);

        // Handle
        auto hx = r.left + r.getWidth () * getValue ();
        CRect handle (hx - 3, r.top, hx + 3, r.bottom);
        context->setFillColor (kWhite);
        context->drawRect (handle, kDrawFilled);

        setDirty (false);
    }

    CMouseEventResult onMouseDown (CPoint& where, const CButtonState& buttons) override
    {
        if (buttons.isLeftButton ())
        {
            beginEdit ();
            updateValueFromPos (where);
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    CMouseEventResult onMouseMoved (CPoint& where, const CButtonState& buttons) override
    {
        if (buttons.isLeftButton ())
        {
            updateValueFromPos (where);
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    CMouseEventResult onMouseUp (CPoint& where, const CButtonState& buttons) override
    {
        endEdit ();
        return kMouseEventHandled;
    }

    CLASS_METHODS (SimpleSliderView, CControl)

private:
    void updateValueFromPos (const CPoint& where)
    {
        auto r = getViewSize ();
        float newVal = (float)((where.x - r.left) / r.getWidth ());
        if (newVal < 0.f) newVal = 0.f;
        if (newVal > 1.f) newVal = 1.f;
        setValue (newVal);
        valueChanged ();
        invalid ();
    }
};

Editor::Editor (void* controller)
    : VSTGUIEditor (controller)
{
    setRect ({0, 0, kEditorWidth, kEditorHeight});
}

bool PLUGIN_API Editor::open (void* parent, const PlatformType& platformType)
{
    // Disable DirectComposition (not implemented in Wine, causes ~10s timeout)
    if (auto win32Factory = getPlatformFactory ().asWin32Factory ())
        win32Factory->disableDirectComposition ();

    CRect frameSize (0, 0, kEditorWidth, kEditorHeight);
    frame = new CFrame (frameSize, this);
    frame->setBackgroundColor (kBgColor);

    // --- Title ---
    auto titleLabel = new CTextLabel (CRect (20, 10, 450, 35));
    titleLabel->setText ("VSTGUI D2D1 Wine Test Plugin");
    titleLabel->setFontColor (kWhite);
    titleLabel->setBackColor (kBgColor);
    titleLabel->setFrameColor (kBgColor);
    titleLabel->setHoriAlign (kLeftText);
    frame->addView (titleLabel);

    // --- Gain Knob ---
    auto gainLabel = new CTextLabel (CRect (30, 40, 130, 55));
    gainLabel->setText ("Gain");
    gainLabel->setFontColor (kWhite);
    gainLabel->setBackColor (kBgColor);
    gainLabel->setFrameColor (kBgColor);
    gainLabel->setHoriAlign (kCenterText);
    frame->addView (gainLabel);

    auto gainKnob = new SimpleKnobView (CRect (40, 58, 120, 138), this, kGainId);
    frame->addView (gainKnob);

    // --- Frequency Slider ---
    auto freqLabel = new CTextLabel (CRect (150, 40, 350, 55));
    freqLabel->setText ("Frequency (20-2000 Hz)");
    freqLabel->setFontColor (kWhite);
    freqLabel->setBackColor (kBgColor);
    freqLabel->setFrameColor (kBgColor);
    freqLabel->setHoriAlign (kCenterText);
    frame->addView (freqLabel);

    auto freqSlider = new SimpleSliderView (CRect (150, 65, 350, 85), this, kFrequencyId);
    frame->addView (freqSlider);

    // --- Custom Waveform Views (D2D1 rendering tests) ---

    auto sectionLabel = new CTextLabel (CRect (20, 150, 350, 168));
    sectionLabel->setText ("D2D1 Rendering Tests");
    sectionLabel->setFontColor (kGreen);
    sectionLabel->setBackColor (kBgColor);
    sectionLabel->setFrameColor (kBgColor);
    sectionLabel->setHoriAlign (kLeftText);
    frame->addView (sectionLabel);

    // Custom knob (arc drawing test)
    frame->addView (new CustomKnobView (CRect (380, 40, 500, 160)));

    // Test lines
    frame->addView (new TestLinesView (CRect (20, 175, 200, 275)));

    // Shapes
    frame->addView (new ShapesView (CRect (210, 175, 400, 310)));

    // Nine-part rectangle
    frame->addView (new NinePartRectView (CRect (410, 175, 590, 260)));

    // Waveforms
    frame->addView (new SineWaveformView (CRect (20, 285, 200, 330)));
    frame->addView (new SquareWaveformView (CRect (210, 315, 400, 355)));
    frame->addView (new TriangleWaveformView (CRect (410, 270, 590, 310)));

    // Stress tests
    frame->addView (new StressWaveformView (CRect (20, 340, 290, 375), 800, 2.0f, kRed));
    frame->addView (new StressWaveformView (CRect (300, 340, 590, 375), 400, 8.0f, kBlue));
    frame->addView (new ZigzagStressView (CRect (20, 385, 290, 420), 40, 3.0f, kWhite));
    frame->addView (new StressWaveformView (CRect (20, 430, 590, 475), 400, 1.5f, kGreen));

    // --- Live Animation (memory leak test) ---
    auto animLabel = new CTextLabel (CRect (20, 482, 600, 497));
    animLabel->setText ("Live Animation (60fps, 200 segments) - memory leak test");
    animLabel->setFontColor (CColor (255, 200, 80, 255));
    animLabel->setBackColor (kBgColor);
    animLabel->setFrameColor (kBgColor);
    animLabel->setHoriAlign (kLeftText);
    frame->addView (animLabel);

    animWaveform = new AnimatedWaveformView (CRect (20, 500, 600, 550), 200, 2.0f, kGreen);
    frame->addView (animWaveform);
    static_cast<AnimatedWaveformView*> (animWaveform)->start ();

    frame->open (parent, platformType);

    // Delayed redraw for Wine — initial WM_PAINT may show framebuffer garbage
    CFrame* f = frame;
    Call::later ([f] () { f->invalid (); }, 100);

    return true;
}

void PLUGIN_API Editor::close ()
{
    if (animWaveform)
    {
        static_cast<AnimatedWaveformView*> (animWaveform)->stop ();
        animWaveform = nullptr;
    }
    if (frame)
    {
        frame->forget ();
        frame = nullptr;
    }
}

void Editor::valueChanged (CControl* pControl)
{
    if (controller)
    {
        controller->setParamNormalized (pControl->getTag (), pControl->getValue ());
        controller->performEdit (pControl->getTag (), pControl->getValue ());
    }
}

} // namespace WineTest
