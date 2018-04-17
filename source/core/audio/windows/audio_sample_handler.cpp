#include "stdafx.h"
#include <mfapi.h>
#include <mfidl.h>
#include "audio_sample_handler.h"

///////////////////////////////////////////////////////////////////////////////

#define BITS_PER_SAMPLE        16
#define CHANNELS               1
#define SAMPLES_PER_SECOND     16000
#define BLOCK_ALIGN            (BITS_PER_SAMPLE>>3)*CHANNELS
#define AVG_BYTES_PER_SECOND   BLOCK_ALIGN*SAMPLES_PER_SECOND

///////////////////////////////////////////////////////////////////////////////

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


AudioSampleHandler::AudioSampleHandler()
    : AudioSampleHandler({ WAVE_FORMAT_PCM, CHANNELS, SAMPLES_PER_SECOND, AVG_BYTES_PER_SECOND, BLOCK_ALIGN, BITS_PER_SAMPLE })
{
}

AudioSampleHandler::AudioSampleHandler(const WAVEFORMATEX& format)
    : AudioSampleHandler(format, MFAudioFormat_PCM)
{
}

AudioSampleHandler::AudioSampleHandler(const WAVEFORMATEX& format, GUID subType)
    : SampleHandler(MFMediaType_Audio, subType), m_format{format}
{
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
