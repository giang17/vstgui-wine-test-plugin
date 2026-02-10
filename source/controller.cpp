#include "controller.h"
#include "pluginparamids.h"
#include "editor.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "base/source/fstreamer.h"

#include <cstring>

namespace WineTest {

using namespace Steinberg;
using namespace Steinberg::Vst;

tresult PLUGIN_API Controller::initialize (FUnknown* context)
{
    tresult result = EditControllerEx1::initialize (context);
    if (result != kResultOk)
        return result;

    // Gain parameter (0..1, default 0.5)
    parameters.addParameter (STR16 ("Gain"), nullptr, 0, 0.5,
                             ParameterInfo::kCanAutomate, kGainId);

    // Frequency parameter (0..1 mapped to 20..2000 Hz, default ~0.5 = ~200 Hz)
    parameters.addParameter (STR16 ("Frequency"), STR16 ("Hz"), 0, 0.5,
                             ParameterInfo::kCanAutomate, kFrequencyId);

    // Bypass parameter
    parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
                             ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass, kBypassId);

    return result;
}

tresult PLUGIN_API Controller::setComponentState (IBStream* state)
{
    if (!state)
        return kResultFalse;

    IBStreamer streamer (state, kLittleEndian);
    float savedGain = 0.f;
    if (streamer.readFloat (savedGain) == false)
        return kResultFalse;
    setParamNormalized (kGainId, savedGain);

    float savedFreq = 0.f;
    if (streamer.readFloat (savedFreq) == false)
        return kResultFalse;
    // Convert frequency back to normalized: freq = 20 * 100^norm => norm = log(freq/20) / log(100)
    float normFreq = logf (savedFreq / 20.0f) / logf (100.0f);
    if (normFreq < 0.f) normFreq = 0.f;
    if (normFreq > 1.f) normFreq = 1.f;
    setParamNormalized (kFrequencyId, normFreq);

    int32 savedBypass = 0;
    if (streamer.readInt32 (savedBypass) == false)
        return kResultFalse;
    setParamNormalized (kBypassId, savedBypass ? 1 : 0);

    return kResultOk;
}

IPlugView* PLUGIN_API Controller::createView (const char* name)
{
    if (strcmp (name, Steinberg::Vst::ViewType::kEditor) == 0)
    {
        return new Editor (this);
    }
    return nullptr;
}

} // namespace WineTest
