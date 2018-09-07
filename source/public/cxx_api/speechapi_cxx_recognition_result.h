//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_result.h: Public API declarations for RecognitionResult C++ base class and related enum class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_c.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class Reason
{
    /// <summary>
    /// Indicates the result is a phrase that has been successfully recognized.
    /// </summary>
    Recognized,

    /// <summary>
    /// Indicates the result is a hypothesis text that has been recognized.
    /// </summary>
    IntermediateResult,

    /// <summary>
    /// Indicates that speech was detected in the audio stream, but no words from the target language were matched.
    /// Possible reasons could be wrong setting of the target language or wrong format of audio stream.
    /// </summary>
    NoMatch,

    /// <summary>
    /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
    /// </summary>
    InitialSilenceTimeout,

    /// <summary>
    /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
    /// </summary>
    InitialBabbleTimeout,

    /// <summary>
    /// Indicates that an error occurred during recognition. The ErrorDetails in Property contains detailed error reasons.
    /// </summary>
    Canceled
};

/// <summary>
/// Contains detailed information about result of a recognition operation.
/// </summary>
class RecognitionResult
{
private:
    DISABLE_DEFAULT_CTORS(RecognitionResult); 

    PropertyCollection<SPXRESULTHANDLE> m_properties;

public:

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~RecognitionResult()
    {
        ::Recognizer_ResultHandle_Close(m_hresult);
        m_hresult = SPXHANDLE_INVALID;
    };

    /// <summary>
    /// Unique result id.
    /// </summary>
    const std::string& ResultId;

    /// <summary>
    /// Recognition status.
    /// </summary>
    const Speech::Reason& Reason;

    /// <summary>
    /// Normalized text generated by a speech recognition engine from recognized input.
    /// This field is filled if the recognition status (<see cref="Reason"/>) is set to Recognized or IntermediateResult.
    /// </summary>
    const std::string& Text;

    /// <summary>
    /// Duration of the recognized speech.
    /// </summary>
    uint64_t Duration() const { return m_duration; }

    /// <summary>
    /// Offset of the recognized speech.
    /// </summary>
    uint64_t Offset() const { return m_offset; }

    /// <summary>
    /// Collection of additional RecognitionResult properties.
    /// </summary>
    PropertyCollection<SPXRESULTHANDLE>& Properties;

    /// <summary>
    /// In case of an unsuccessful recognition, provides a brief description of an occurred error.
    /// This field is only filled-out if the recognition status (<see cref="Reason"/>) is set to Canceled.
    /// </summary>
    const std::string ErrorDetails;

protected:

    /*! \cond PROTECTED */

    explicit RecognitionResult(SPXRESULTHANDLE hresult) :
        m_properties(hresult, HandleType::RESULT),
        ResultId(m_resultId),
        Reason(m_reason),
        Text(m_text),
        Properties(m_properties),
        ErrorDetails(Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonErrorDetails)),
        Handle(m_hresult),
        m_hresult(hresult)
    {
        PopulateResultFields(hresult, &m_resultId, &m_reason, &m_text);
    }

    const SPXRESULTHANDLE& Handle;

    /*! \endcond */

private:

    void PopulateResultFields(SPXRESULTHANDLE hresult, std::string *presultId, Speech::Reason* preason, std::string* ptext)
    {

        SPX_INIT_HR(hr);

        const size_t maxCharCount = 1024;
        char sz[maxCharCount + 1];

        if (presultId != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetResultId(hresult, sz, maxCharCount));
            *presultId = sz;
        }

        if (preason != nullptr)
        {
            Result_RecognitionReason reason;
            SPX_THROW_ON_FAIL(hr = Result_GetRecognitionReason(hresult, &reason));
            *preason = (Speech::Reason)reason;
        }

        if (ptext != nullptr)
        {
            SPX_THROW_ON_FAIL(hr = Result_GetText(hresult, sz, maxCharCount));
            *ptext = sz;
        }

        SPX_THROW_ON_FAIL(hr = Result_GetOffset(hresult, &m_offset));
        SPX_THROW_ON_FAIL(hr = Result_GetDuration(hresult, &m_duration));
    }

    SPXRESULTHANDLE m_hresult;

    std::string m_resultId;
    Speech::Reason m_reason;
    std::string m_text;
    uint64_t m_offset;
    uint64_t m_duration;
};


} } } // Microsoft::CognitiveServices::Speech
