//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.cpp: Public API definitions for Result related C methods
//

#include "stdafx.h"
#include "string_utils.h"

#include "ispxinterfaces.h" // for SpxQueryInterface

using namespace Microsoft::CognitiveServices::Speech::Impl;

static_assert((int)Reason_Recognized == (int)Reason::Recognized, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_IntermediateResult == (int)Reason::IntermediateResult, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_NoMatch == (int)Reason::NoMatch, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_InitialSilenceTimeout == (int)Reason::InitialSilenceTimeout, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_InitialBabbleTimeout == (int)Reason::InitialBabbleTimeout, "Reason_* enum values == Reason::* enum values");
static_assert((int)Reason_Canceled == (int)Reason::Canceled, "Reason_* enum values == Reason::* enum values");

SPXAPI Result_GetResultId(SPXRESULTHANDLE hresult, char* pszResultId, uint32_t cchResultId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchResultId == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = PAL::ToString(result->GetResultId());
        auto pszActual = strActual.c_str();
        PAL::strcpy(pszResultId, cchResultId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetRecognitionReason(SPXRESULTHANDLE hresult, Result_RecognitionReason* preason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, preason == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *preason = (Result_RecognitionReason)result->GetReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetText(SPXRESULTHANDLE hresult, char* pszText, uint32_t cchText)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchText == 0);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = PAL::ToString(result->GetText());
        auto pszActual = strActual.c_str();
        PAL::strcpy(pszText, cchText, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetOffset(SPXRESULTHANDLE hresult, uint64_t* offset)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, offset == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *offset = result->GetOffset();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Result_GetDuration(SPXRESULTHANDLE hresult, uint64_t* duration)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, duration == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *duration = result->GetDuration();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_property_bag(SPXRESULTHANDLE hresult, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
