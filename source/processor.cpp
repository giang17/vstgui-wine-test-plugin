#include "processor.h"
#include "plugincids.h"
#include "pluginparamids.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "base/source/fstreamer.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace WineTest {

using namespace Steinberg;
using namespace Steinberg::Vst;

Processor::Processor ()
{
    setControllerClass (ControllerUID);
}

tresult PLUGIN_API Processor::initialize (FUnknown* context)
{
    tresult result = AudioEffect::initialize (context);
    if (result != kResultOk)
        return result;

    addAudioInput (STR16 ("Stereo In"), SpeakerArr::kStereo);
    addAudioOutput (STR16 ("Stereo Out"), SpeakerArr::kStereo);

    return kResultOk;
}

tresult PLUGIN_API Processor::setActive (TBool state)
{
    if (state)
        phase = 0.0;
    return AudioEffect::setActive (state);
}

tresult PLUGIN_API Processor::setupProcessing (ProcessSetup& newSetup)
{
    sampleRate = newSetup.sampleRate;
    return AudioEffect::setupProcessing (newSetup);
}

tresult PLUGIN_API Processor::canProcessSampleSize (int32 symbolicSampleSize)
{
    if (symbolicSampleSize == kSample32)
        return kResultTrue;
    return kResultFalse;
}

tresult PLUGIN_API Processor::process (ProcessData& data)
{
    // Read parameter changes
    if (IParameterChanges* paramChanges = data.inputParameterChanges)
    {
        int32 numParamsChanged = paramChanges->getParameterCount ();
        for (int32 i = 0; i < numParamsChanged; i++)
        {
            if (IParamValueQueue* paramQueue = paramChanges->getParameterData (i))
            {
                ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
                if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue)
                {
                    switch (paramQueue->getParameterId ())
                    {
                        case kGainId:
                            fGain = (float)value;
                            break;
                        case kFrequencyId:
                            // Map 0..1 to 20..2000 Hz (exponential)
                            fFrequency = 20.0f * powf (100.0f, (float)value);
                            break;
                        case kBypassId:
                            bBypass = (value > 0.5f);
                            break;
                    }
                }
            }
        }
    }

    if (data.numInputs == 0 || data.numOutputs == 0)
        return kResultOk;

    int32 numChannels = data.outputs[0].numChannels;
    int32 numSamples = data.numSamples;
    float** out = (float**)data.outputs[0].channelBuffers32;
    float** in = (float**)data.inputs[0].channelBuffers32;

    if (bBypass)
    {
        for (int32 ch = 0; ch < numChannels; ch++)
        {
            if (in[ch] != out[ch])
                memcpy (out[ch], in[ch], numSamples * sizeof (float));
        }
        return kResultOk;
    }

    // Generate sine and mix with input
    double phaseInc = 2.0 * M_PI * fFrequency / sampleRate;
    for (int32 s = 0; s < numSamples; s++)
    {
        float sine = (float)(sin (phase) * fGain * 0.5);
        phase += phaseInc;
        if (phase >= 2.0 * M_PI)
            phase -= 2.0 * M_PI;

        for (int32 ch = 0; ch < numChannels; ch++)
        {
            out[ch][s] = in[ch][s] + sine;
        }
    }

    data.outputs[0].silenceFlags = 0;
    return kResultOk;
}

tresult PLUGIN_API Processor::setState (IBStream* state)
{
    IBStreamer streamer (state, kLittleEndian);
    float savedGain = 0.f;
    if (streamer.readFloat (savedGain) == false)
        return kResultFalse;
    float savedFreq = 0.f;
    if (streamer.readFloat (savedFreq) == false)
        return kResultFalse;
    int32 savedBypass = 0;
    if (streamer.readInt32 (savedBypass) == false)
        return kResultFalse;

    fGain = savedGain;
    fFrequency = savedFreq;
    bBypass = savedBypass > 0;
    return kResultOk;
}

tresult PLUGIN_API Processor::getState (IBStream* state)
{
    IBStreamer streamer (state, kLittleEndian);
    streamer.writeFloat (fGain);
    streamer.writeFloat (fFrequency);
    streamer.writeInt32 (bBypass ? 1 : 0);
    return kResultOk;
}

} // namespace WineTest
