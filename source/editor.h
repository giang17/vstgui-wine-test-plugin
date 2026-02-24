#pragma once

#include "public.sdk/source/vst/vstguieditor.h"
#include "vstgui/lib/controls/icontrollistener.h"

namespace WineTest {

class Editor : public Steinberg::Vst::VSTGUIEditor, public VSTGUI::IControlListener
{
public:
    Editor (void* controller);

    bool PLUGIN_API open (void* parent, const VSTGUI::PlatformType& platformType) SMTG_OVERRIDE;
    void PLUGIN_API close () SMTG_OVERRIDE;

    // IControlListener
    void valueChanged (VSTGUI::CControl* pControl) SMTG_OVERRIDE;

private:
    static const int kEditorWidth = 620;
    static const int kEditorHeight = 560;

    VSTGUI::CView* animWaveform = nullptr;
};

} // namespace WineTest
