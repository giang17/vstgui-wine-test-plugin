#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace WineTest {

class Processor : public Steinberg::Vst::AudioEffect
{
public:
    Processor ();

    static Steinberg::FUnknown* createInstance (void*)
    {
        return (Steinberg::Vst::IAudioProcessor*)new Processor;
    }

    Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

private:
    float fGain = 1.0f;
    float fFrequency = 440.0f;
    bool bBypass = false;
    double phase = 0.0;
    double sampleRate = 44100.0;
};

} // namespace WineTest
