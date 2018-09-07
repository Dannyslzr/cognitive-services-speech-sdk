//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_test_console.cpp: Definitions for the CarbonTestConsole tool C++ class
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "speechapi_c.h"
#include "mock_controller.h"
#include <chrono>
#include <string>
#include <string_utils.h>
#include <file_utils.h>
#include <platform.h>

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech::Translation;

CarbonTestConsole::CarbonTestConsole()
{
}

CarbonTestConsole::~CarbonTestConsole()
{
    TermCarbon();
}

int CarbonTestConsole::Run(const std::vector<std::string>& args)
{
    DisplayConsoleHeader();

    ConsoleArgs consoleArgs;
    if (!ParseConsoleArgs(args, &consoleArgs) || !ValidateConsoleArgs(&consoleArgs))
    {
        DisplayConsoleUsage();
        return -1;
    }

    ProcessConsoleArgs(&consoleArgs);

    return 0;
}

bool CarbonTestConsole::ParseConsoleArgs(const std::vector<std::string>& args, ConsoleArgs* pconsoleArgs)
{
    std::string* pstrNextArg = nullptr;
    bool fNextArgRequired = false;
    bool fShowOptions = false;

    for (size_t i = 1; !fShowOptions && i < args.size(); i++)
    {
        const char *pszArg = args[i].c_str();
        if (PAL::stricmp(pszArg, "--debug") == 0)
        {
            WaitForDebugger();
            fShowOptions = pconsoleArgs->m_fWaitForDebugger || fNextArgRequired;
            pconsoleArgs->m_fWaitForDebugger = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--speech") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--intent") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--translation") == 0)
        {
            fShowOptions = pconsoleArgs->m_strRecognizerType.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<TranslationRecognizer>();
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--commands") == 0)
        {
            fShowOptions = (pconsoleArgs->m_strRecognizerType.length() > 0 && pconsoleArgs->m_strRecognizerType != PAL::GetTypeName<IntentRecognizer>()) || pconsoleArgs->m_fCommandSystem || fNextArgRequired;
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<IntentRecognizer>();
            pconsoleArgs->m_fCommandSystem = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--input", strlen("--input")) == 0)
        {
            fShowOptions = pconsoleArgs->m_audioInput.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_audioInput;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--mockmicrophone") == 0)
        {
            fShowOptions = pconsoleArgs->m_audioInput.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_audioInput = "mockmicrophone";
            pstrNextArg = &pconsoleArgs->m_strMockMicrophoneRealTimePercentage;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--mockwavfile") == 0)
        {
            fShowOptions = pconsoleArgs->m_mockWavFileName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_mockWavFileName;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--mockkws") == 0)
        {
            fShowOptions = pconsoleArgs->m_useMockKws || fNextArgRequired;
            pconsoleArgs->m_useMockKws = true;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--unidec") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseRecoEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseRecoEngineProperty = "CARBON-INTERNAL-UseRecoEngine-Unidec";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--mockrecoengine") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseRecoEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseRecoEngineProperty = "CARBON-INTERNAL-UseRecoEngine-Mock";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--usp") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseRecoEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseRecoEngineProperty = "CARBON-INTERNAL-UseRecoEngine-Usp";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--luisdirect") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseLuEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseLuEngineProperty = "CARBON-INTERNAL-UseLuEngine-LuisDirect";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::stricmp(pszArg, "--mockluengine") == 0)
        {
            fShowOptions = pconsoleArgs->m_strUseLuEngineProperty.length() > 0 || fNextArgRequired;
            pconsoleArgs->m_strUseLuEngineProperty = "CARBON-INTERNAL-UseLuEngine-Mock";
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--endpoint", strlen("--endpoint")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strEndpointUri.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strEndpointUri;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--customSpeechModelId", strlen("--customSpeechModelId")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strCustomSpeechModelId.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strCustomSpeechModelId;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--subscription", strlen("--subscription")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strSubscriptionKey.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strSubscriptionKey;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--region", strlen("--region")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRegion.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRegion;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--intentAppId", strlen("--intentAppId")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strIntentAppId.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strIntentAppId;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--intentNames", strlen("--intentNames")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strIntentNames.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strIntentNames;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--single") == 0)
        {
            fShowOptions = pconsoleArgs->m_fContinuousRecognition || fNextArgRequired;
            pconsoleArgs->m_fRecognizeAsync = true;
            pstrNextArg = nullptr;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--continuous", strlen("--continuous")) == 0)
        {
            fShowOptions = pconsoleArgs->m_fRecognizeAsync || fNextArgRequired;
            pconsoleArgs->m_fContinuousRecognition = true;
            pconsoleArgs->m_continuousRecognitionSeconds = UINT16_MAX;
            pstrNextArg = &pconsoleArgs->m_strContinuousRecognitionSeconds;
            fNextArgRequired = false;
        }
        else if (PAL::strnicmp(pszArg, "--times", strlen("--times")) == 0)
        {
            fShowOptions = fNextArgRequired;
            pconsoleArgs->m_runHowManyTimes = 1;
            pstrNextArg = &pconsoleArgs->m_strHowManyTimes;
            fNextArgRequired = true;
        }
        else if (PAL::strnicmp(pszArg, "--sample", strlen("--sample")) == 0)
        {
            fShowOptions = pconsoleArgs->m_strRunSampleName.length() > 0 || fNextArgRequired;
            pstrNextArg = &pconsoleArgs->m_strRunSampleName;
            fNextArgRequired = true;
        }
        else if (PAL::stricmp(pszArg, "--interactive") == 0)
        {
            fShowOptions = pconsoleArgs->m_fInteractivePrompt || fNextArgRequired;
            pconsoleArgs->m_fInteractivePrompt = true;
            pstrNextArg = NULL;
            fNextArgRequired = false;
        }
        else if (pstrNextArg != NULL)
        {
            fShowOptions = pstrNextArg->length() > 0;
            *pstrNextArg = pszArg;
            pstrNextArg = NULL;
            fNextArgRequired = false;
            continue;
        }
        else
        {
            fShowOptions = true;
        }

        if (fShowOptions) break;

        const char *pszArgInsideThisArg = strpbrk(pszArg, ":=");
        if (pszArgInsideThisArg)
        {
            if (pstrNextArg == NULL)
            {
                fShowOptions = true;
                break;
            }

            fShowOptions = pstrNextArg->length() > 0;
            *pstrNextArg = pszArgInsideThisArg + 1;
            pstrNextArg = NULL;
            fNextArgRequired = false;
        }
    }

    return !(fShowOptions || fNextArgRequired);
}

bool CarbonTestConsole::ValidateConsoleArgs(ConsoleArgs* pconsoleArgs)
{
    auto fValid = true;

    if (pconsoleArgs->m_audioInput.empty() || PAL::stricmp(pconsoleArgs->m_audioInput.c_str(), "microphone") == 0)
    {
        pconsoleArgs->m_useInteractiveMicrophone = true;
        pconsoleArgs->m_audioInput.clear();
    }
    else if (PAL::stricmp(pconsoleArgs->m_audioInput.c_str(), "mockmicrophone") == 0)
    {
        pconsoleArgs->m_useMockMicrophone = true;
        pconsoleArgs->m_audioInput.clear();

        if (!pconsoleArgs->m_strMockMicrophoneRealTimePercentage.empty())
        {
            pconsoleArgs->m_mockMicrophoneRealTimePercentage = (int16_t)atoi(pconsoleArgs->m_strMockMicrophoneRealTimePercentage.c_str());
            fValid = pconsoleArgs->m_mockMicrophoneRealTimePercentage >= 0 && pconsoleArgs->m_mockMicrophoneRealTimePercentage <= 400;
        }
    }
    else if (PAL::access(pconsoleArgs->m_audioInput.c_str(), 0) != 0)
    {
        SPX_DBG_TRACE_ERROR("File does not exist: %s", pconsoleArgs->m_audioInput.c_str());
        fValid = false;
    }

    if (pconsoleArgs->m_fContinuousRecognition && pconsoleArgs->m_strContinuousRecognitionSeconds.length() > 0)
    {
        auto seconds = std::stoi(pconsoleArgs->m_strContinuousRecognitionSeconds.c_str());
        pconsoleArgs->m_continuousRecognitionSeconds = uint16_t(std::min(std::max(seconds, 0), 30));
    }

    if (pconsoleArgs->m_fRecognizeAsync || pconsoleArgs->m_fContinuousRecognition)
    {
        if (pconsoleArgs->m_strRecognizerType.length() == 0)
        {
            pconsoleArgs->m_strRecognizerType = PAL::GetTypeName<SpeechRecognizer>();
        }
    }
    else if (pconsoleArgs->m_strRecognizerType.length() > 0)
    {
        pconsoleArgs->m_fRecognizeAsync = !pconsoleArgs->m_fCommandSystem && !pconsoleArgs->m_fInteractivePrompt;
    }

    if (!pconsoleArgs->m_strHowManyTimes.empty())
    {
        pconsoleArgs->m_runHowManyTimes = (int16_t)atoi(pconsoleArgs->m_strHowManyTimes.c_str());
    }

    return fValid;
}

void CarbonTestConsole::ProcessConsoleArgs(ConsoleArgs* pconsoleArgs)
{
    InitGlobalParameters(pconsoleArgs);

    if (pconsoleArgs->m_strRunSampleName.length() > 0)
    {
        auto count = pconsoleArgs->m_runHowManyTimes;
        while (count-- > 0)
        {
            RunSample(pconsoleArgs->m_strRunSampleName);
            ConsoleWrite(count > 0 ? "\n" : "");
        }
    }

    EnsureInitCarbon(pconsoleArgs);

    if (pconsoleArgs->m_fRecognizeAsync)
    {
        auto count = pconsoleArgs->m_runHowManyTimes;
        while (count-- > 0)
        {
            RecognizeAsync();
            ConsoleWrite(count > 0 ? "\n" : "");
        }
    }

    if (pconsoleArgs->m_fContinuousRecognition)
    {
        ContinuousRecognition(pconsoleArgs->m_continuousRecognitionSeconds);
    }

    if (pconsoleArgs->m_fInteractivePrompt)
    {
        RunInteractivePrompt();
    }
}

void CarbonTestConsole::DisplayConsoleHeader()
{
    ConsoleWriteLine("CarbonX - Carbon Test Console\nCopyright (C) 2017 Microsoft Corporation. All Rights Reserved.\n");
}

void CarbonTestConsole::DisplayConsoleUsage()
{
    ConsoleWriteLine("  carbonx [--speech | --intent | --commands | --translation ] {input} {auth} {additional}");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Input: --input:[microphone | {waveFileName}]");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --input:microphone      Use the default microphone for audio input.");
    ConsoleWriteLine("       --input:{waveFileName}  Use WAV file for audio input.");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Authentication and Service:");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --endpoint:{uri}              Use {uri} as the USP endpoint.");
    ConsoleWriteLine("       --subscription:{key}          Use {key} as the subscription key.");
    ConsoleWriteLine("       --region:{region}             Use {region} as the service region.");
    ConsoleWriteLine("       --customSpeechModelId:{id}    Use {id} as the Custom Speech Model ID.");
    ConsoleWriteLine("");
    ConsoleWriteLine("     Additional:");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --single                Use RecognizeAsync for a single utterance.");
    ConsoleWriteLine("       --continuous:{seconds}  Use [Start/Stop]ContinuousRecognition, waiting");
    ConsoleWriteLine("                               {seconds} in between starting and stopping.");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --sample:{sampleName}   Run the sample named {sampleName}.");
    ConsoleWriteLine("");
    ConsoleWriteLine("       --debug                 Stops execution and waits (max 30s) for debugger.");
    ConsoleWriteLine("       --interactive           Allows interactive Carbon use via console window.");
    ConsoleWriteLine("");
}

void CarbonTestConsole::DisplayConsolePrompt()
{
    ConsoleWrite("carbonx> ");
}

void CarbonTestConsole::ConsoleWrite(const char* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);
    vfprintf(stdout, pszFormat, argptr);
    va_end(argptr);
}

void CarbonTestConsole::ConsoleWriteLine(const char* pszFormat, ...)
{
    std::string format(pszFormat);
    format += "\n";

    va_list argptr;
    va_start(argptr, pszFormat);
    vfprintf(stdout, format.c_str(), argptr);
    va_end(argptr);
}

bool CarbonTestConsole::ConsoleReadLine(std::string& str)
{
    const int cchMax = 1024;
    str.reserve(cchMax);
    str[0] = L'\0';

    char* data = (char*)str.data(); // should use CX17 .data(); VC2017 works fine; might not work cross-platform
    str = fgets(data, cchMax - 1, stdin);

    return str.length() > 0;
}

bool CarbonTestConsole::GetConsoleInput(std::string& str)
{
    DisplayConsolePrompt();

    ConsoleReadLine(str);
    auto lastLF = str.find_last_of(L'\n');

    if (lastLF != std::string::npos)
    {
        str.resize(lastLF);
    }

    return str.length() > 0;
}

void CarbonTestConsole::ProcessConsoleInput(const char* psz)
{
    if (PAL::stricmp(psz, "help") == 0 || PAL::stricmp(psz, "?") == 0)
    {
        ConsoleInput_Help();
    }
    else if (PAL::strnicmp(psz, "help ", strlen("help ")) == 0)
    {
        ConsoleInput_HelpOn(psz + strlen("help "));
    }
    else if (PAL::strnicmp(psz, "sample ", strlen("sample ")) == 0)
    {
        RunSample(psz + strlen("sample "));
    }
    else if (PAL::strnicmp(psz, "factory ", strlen("factory ")) == 0)
    {
        ConsoleInput_Factory(psz + strlen("factory "));
    }
    else if (PAL::strnicmp(psz, "recognizer ", strlen("recognizer ")) == 0)
    {
        ConsoleInput_Recognizer(psz + strlen("recognizer "), m_recognizer);
    }
    else if (PAL::strnicmp(psz, "speech ", strlen("speech ")) == 0)
    {
        ConsoleInput_SpeechRecognizer(psz + strlen("speech "), m_speechRecognizer);
    }
    else if (PAL::strnicmp(psz, "intent ", strlen("intent ")) == 0)
    {
        ConsoleInput_IntentRecognizer(psz + strlen("intent "), m_intentRecognizer);
    }
    else if (PAL::strnicmp(psz, "session ", strlen("session ")) == 0)
    {
        ConsoleInput_Session(psz + strlen("session "));
    }
    else if (PAL::strnicmp(psz, "commandsystem ", strlen("commandsystem ")) == 0)
    {
        ConsoleInput_CommandSystem(psz + strlen("commandsystem "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown command: '%s'.\n\nUse 'HELP' for a list of valid commands.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Help()
{
    ConsoleWriteLine("");
    ConsoleWriteLine("COMMANDs: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    global           Access methods/properties/events on the GLOBAL PARAMETERS object.");
    ConsoleWriteLine("    factory          Access methods/properties/events on the RECOGNIZER FACTORY object.");
    ConsoleWriteLine("    recognizer       Access methods/properties/events on the base RECOGNIZER object.");
    ConsoleWriteLine("    speech           Access methods/properties/events on the SPEECH recognizer object.");
    ConsoleWriteLine("    intent           Access methods/properties/events on the INTENT recognizer object.");
    ConsoleWriteLine("    session          Access methods/properties/events on the SESSION object.");
    ConsoleWriteLine("    commandsystem    Access methods/properties/events on the COMMAND SYSTEM object.");
    ConsoleWriteLine("    sample {name}    Run the sample named 'NAME'.");
    ConsoleWriteLine("    help {command}   Get help w/ 'recognizer', 'speech', 'intent', or 'commandsystem' commands.");
    ConsoleWriteLine("    exit             Exit interactive mode.");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOn(const char* psz)
{
    if (PAL::stricmp(psz, "globa") == 0)
    {
        ConsoleInput_HelpOnGlobal();
    }
    else if (PAL::stricmp(psz, "factory") == 0)
    {
        ConsoleInput_HelpOnFactory();
    }
    else if (PAL::stricmp(psz, "recognizer") == 0)
    {
        ConsoleInput_HelpOnRecognizer();
    }
    else if (PAL::stricmp(psz, "speech") == 0)
    {
        ConsoleInput_HelpOnSpeech();
    }
    else if (PAL::stricmp(psz, "intent") == 0)
    {
        ConsoleInput_HelpOnIntent();
    }
    else if (PAL::stricmp(psz, "session") == 0)
    {
        ConsoleInput_HelpOnSession();
    }
    else if (PAL::stricmp(psz, "commandsystem") == 0)
    {
        ConsoleInput_HelpOnCommandSystem();
    }
}

void CarbonTestConsole::ConsoleInput_HelpOnGlobal()
{
    ConsoleWriteLine("GLOBAL {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string         {name} {value}");
    ConsoleWriteLine("    get string         {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set value          {name} {value}");
    ConsoleWriteLine("    get value          {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool           {name} {value}");
    ConsoleWriteLine("    get bool           {name}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnFactory()
{
    ConsoleWriteLine("FACTORY {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Create Speech Recognizer");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnRecognizer()
{
    ConsoleWriteLine("RECOGNIZER {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnSpeech()
{
    ConsoleWriteLine("SPEECH {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string         {name} {value}");
    ConsoleWriteLine("    get string         {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set value          {name} {value}");
    ConsoleWriteLine("    get value          {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool           {name} {value}");
    ConsoleWriteLine("    get bool           {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnIntent()
{
    ConsoleWriteLine("INTENT {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    IsEnabled");
    ConsoleWriteLine("    Enable");
    ConsoleWriteLine("    Disable");
    ConsoleWriteLine("");
    ConsoleWriteLine("    Recognize");
    ConsoleWriteLine("    StartContinuous");
    ConsoleWriteLine("    StopContinuous");
    ConsoleWriteLine("    StartKeyword");
    ConsoleWriteLine("    StopKeyword");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    SessionStarted     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SessionStopped     {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechStartDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    SpeechEndDetected       {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    IntermediateResult {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    FinalResult        {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("    Canceled           {Connect | Disconnect | DisconnectAll}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnSession()
{
    ConsoleWriteLine("SESSION {method_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    from speech");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set string     {name} {value}");
    ConsoleWriteLine("    get string     {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set number     {name} {value}");
    ConsoleWriteLine("    get number     {name}");
    ConsoleWriteLine("");
    ConsoleWriteLine("    set bool       {name} {value}");
    ConsoleWriteLine("    get bool       {name}");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_HelpOnCommandSystem()
{
    ConsoleWriteLine("COMMANDSYSTEM {method_command | event_command}");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Methods:");
    ConsoleWriteLine("");
    ConsoleWriteLine("    NYI");
    ConsoleWriteLine("");
    ConsoleWriteLine("  Events: ");
    ConsoleWriteLine("");
    ConsoleWriteLine("    NYI");
    ConsoleWriteLine("");
}

void CarbonTestConsole::ConsoleInput_Factory(const char* psz)
{
    if (PAL::strnicmp(psz, "create speech recognizer", strlen("create speech recognizer")) == 0)
    {
        //TODO: Check if required keys are given for other execution paths: this is not the only feature which needs subscriptionKey
        if (!m_subscriptionKey.empty() || !m_endpointUri.empty())
        {
            Factory_CreateSpeechRecognizer(psz + strlen("create speech recognizer"));
        }
        else
        {
            ConsoleWriteLine("\nsubscription ID is required to use this feature. Please re-run with valid subscription ID\n");
        }
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Recognizer(const char* psz, std::shared_ptr<BaseAsyncRecognizer>& recognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        Recognizer_IsEnabled(recognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        Recognizer_Enable(recognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        Recognizer_Disable(recognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        Recognizer_Recognize(recognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(recognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(recognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        Recognizer_StartKeywordRecognition(recognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword ") == 0)
    {
        Recognizer_StopKeywordRecognition(recognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_recognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_recognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_recognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_recognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_IntermediateResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_recognizer->IntermediateResult, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_FinalResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_recognizer->FinalResult, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_recognizer->Canceled, fn);
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_SpeechRecognizer(const char* psz, std::shared_ptr<SpeechRecognizer>& speechRecognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        Recognizer_IsEnabled(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        Recognizer_Enable(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        Recognizer_Disable(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        Recognizer_Recognize(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        Recognizer_StartKeywordRecognition(speechRecognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword") == 0)
    {
        Recognizer_StopKeywordRecognition(speechRecognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_speechRecognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_speechRecognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_speechRecognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_speechRecognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_speechRecognizer->IntermediateResult, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_FinalResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_speechRecognizer->FinalResult, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::SpeechRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_speechRecognizer->Canceled, fn);
    }
    else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    {
        Parameters_SetString(m_speechRecognizer->Parameters, psz + strlen("set string "));
    }
    else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    {
        Parameters_GetString(m_speechRecognizer->Parameters, psz + strlen("get string "));
    }
    else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    {
        Parameters_SetNumber(m_speechRecognizer->Parameters, psz + strlen("set number "));
    }
    else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    {
        Parameters_GetNumber(m_speechRecognizer->Parameters, psz + strlen("get number "));
    }
    else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    {
        Parameters_SetBool(m_speechRecognizer->Parameters, psz + strlen("set bool "));
    }
    else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    {
        Parameters_GetBool(m_speechRecognizer->Parameters, psz + strlen("get bool "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_IntentRecognizer(const char* psz, std::shared_ptr<IntentRecognizer>& intentRecognizer)
{
    if (PAL::stricmp(psz, "isenabled") == 0)
    {
        Recognizer_IsEnabled(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "enable") == 0)
    {
        Recognizer_Enable(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "disable") == 0)
    {
        Recognizer_Disable(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "recognize") == 0)
    {
        Recognizer_Recognize(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "startcontinuous") == 0)
    {
        Recognizer_StartContinuousRecognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "stopcontinuous") == 0)
    {
        Recognizer_StopContinuousRecognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "startkeyword") == 0)
    {
        Recognizer_StartKeywordRecognition(intentRecognizer);
    }
    else if (PAL::stricmp(psz, "stopkeyword") == 0)
    {
        Recognizer_StopKeywordRecognition(intentRecognizer);
    }
    else if (PAL::strnicmp(psz, "sessionstarted ", strlen("sessionstarted ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStartedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstarted "), m_intentRecognizer->SessionStarted, fn);
    }
    else if (PAL::strnicmp(psz, "sessionstopped ", strlen("sessionstopped ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SessionStoppedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("sessionstopped "), m_intentRecognizer->SessionStopped, fn);
    }
    else if (PAL::strnicmp(psz, "speechstartdetected ", strlen("speechstartdetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechStartDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechstartdetected "), m_intentRecognizer->SpeechStartDetected, fn);
    }
    else if (PAL::strnicmp(psz, "speechenddetected ", strlen("speechenddetected ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::Recognizer_SpeechEndDetectedHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("speechenddetected "), m_intentRecognizer->SpeechEndDetected, fn);
    }
    else if (PAL::strnicmp(psz, "intermediateresult ", strlen("intermediateresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("intermediateresult "), m_intentRecognizer->IntermediateResult, fn);
    }
    else if (PAL::strnicmp(psz, "finalresult ", strlen("finalresult ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_FinalResultHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("finalresult "), m_intentRecognizer->FinalResult, fn);
    }
    else if (PAL::strnicmp(psz, "canceled ", strlen("canceled ")) == 0)
    {
        auto fn = std::bind(&CarbonTestConsole::IntentRecognizer_CanceledHandler, this, std::placeholders::_1);
        Recognizer_Event(psz + strlen("canceled "), m_intentRecognizer->Canceled, fn);
    }
    // TODO: RobCh: Intent: Add ParametersCollection to IntentRecognizer
    // else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    // {
    //     Parameters_SetString(m_intentRecognizer->Parameters, psz + strlen("set string "));
    // }
    // else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    // {
    //     Parameters_GetString(m_intentRecognizer->Parameters, psz + strlen("get string "));
    // }
    // else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    // {
    //     Parameters_SetNumber(m_intentRecognizer->Parameters, psz + strlen("set number "));
    // }
    // else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    // {
    //     Parameters_GetNumber(m_intentRecognizer->Parameters, psz + strlen("get number "));
    // }
    // else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    // {
    //     Parameters_SetBool(m_intentRecognizer->Parameters, psz + strlen("set bool "));
    // }
    // else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    // {
    //     Parameters_GetBool(m_intentRecognizer->Parameters, psz + strlen("get bool "));
    // }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::ConsoleInput_CommandSystem(const char*)
{
    // TODO: ROBCH: Implement CarbonTestConsole::ConsoleInput_CommandSystem
}

void CarbonTestConsole::Factory_CreateSpeechRecognizer(const char* psz)
{
    m_speechRecognizer = nullptr;
    m_recognizer = nullptr;
    m_session = nullptr;

    auto factory = !m_endpointUri.empty()
        ? SpeechFactory::FromEndpoint(m_endpointUri, m_subscriptionKey)
        : SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);

    m_speechRecognizer = *psz == '\0'
        ? factory->CreateSpeechRecognizer()
        : factory->CreateSpeechRecognizerWithFileInput(psz + 1);

    auto fn1 = std::bind(&CarbonTestConsole::SpeechRecognizer_FinalResultHandler, this, std::placeholders::_1);
    m_speechRecognizer->FinalResult.Connect(fn1);

    auto fn2 = std::bind(&CarbonTestConsole::SpeechRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
    m_speechRecognizer->IntermediateResult.Connect(fn2);

    m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_speechRecognizer);
    m_session = Session::FromRecognizer(m_speechRecognizer);
}

template <class T>
void CarbonTestConsole::Recognizer_IsEnabled(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWrite("\n%s.IsEnabled == ", name.c_str());
    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s\n", BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Enable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nEnabling %s...", name.c_str());
    recognizer->Enable();
    ConsoleWriteLine("Enabling %s... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s.IsEnabled == %s\n", name.c_str(), BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Disable(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nDisabling %s...", name.c_str());
    recognizer->Disable();
    ConsoleWriteLine("Disabling %s... Done!\n", name.c_str());

    bool enabled = recognizer->IsEnabled();
    ConsoleWriteLine("%s.IsEnabled == %s\n", name.c_str(), BoolToString(enabled).c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeAsync %s...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine("RecognizeAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeAsync %s... Waiting... Done!\n", name.c_str());
}

void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<SpeechRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeAsync %s...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine("RecognizeAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeAsync %s... Waiting... Done!\n", name.c_str());

    ConsoleWriteLine("SpeechRecognitionResult: ResultId=%s; Reason=%d; ErrorDetails=%s; Text=%s",
        result->ResultId.c_str(), result->Reason, result->ErrorDetails.c_str(), result->Text.c_str());
}

void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<IntentRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeAsync %s...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine("RecognizeAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeAsync %s... Waiting... Done!\n", name.c_str());

    auto resultId = result->ResultId;
    auto reason = result->Reason;
    auto text = result->Text;
    auto errorDetails = result->ErrorDetails;

    auto intentId = result->IntentId;
    auto intentJson = result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonResult);

    ConsoleWriteLine("IntentRecognitionResult: ResultId=%s; Reason=%d; Text=%s, ErrorDetails=%s, IntentId=%s, Json=%s",
        resultId.c_str(), reason, text.c_str(), errorDetails.c_str(), intentId.c_str(), intentJson.c_str());
}

void CarbonTestConsole::Recognizer_Recognize(std::shared_ptr<TranslationRecognizer>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nRecognizeAsync %s...", name.c_str());
    auto future = recognizer->RecognizeAsync();
    ConsoleWriteLine("RecognizeAsync %s... Waiting...", name.c_str());
    auto result = future.get();
    ConsoleWriteLine("RecognizeAsync %s... Waiting... Done!\n", name.c_str());

    ConsoleWriteLine("TranslationTextResult: ResultId=%s, ErrorDetails=%s, RecognizedText=%s, TranslationsStatus=%d",
        result->TranslationTextResult::ResultId.c_str(), result->ErrorDetails.c_str(), result->Text.c_str(), (int)result->TranslationStatus);
    for (auto it : result->Translations)
    {
        ConsoleWriteLine("                Translation to %s: %s", it.first.c_str(), it.second.c_str());
    }
}

template <class T>
void CarbonTestConsole::Recognizer_StartContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStartContinuousRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StartContinuousRecognitionAsync();
    ConsoleWriteLine("StartContinuousRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StartContinuousRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_StopContinuousRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStopContinuousRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StopContinuousRecognitionAsync();
    ConsoleWriteLine("StopContinuousRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StopContinuousRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_StartKeywordRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStartKeywordRecognitionAsync %s...", name.c_str());

    auto model = KeywordRecognitionModel::FromFile("kws.table");
    auto future = recognizer->StartKeywordRecognitionAsync(model);

    ConsoleWriteLine("StartKeywordRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StartKeywordRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_StopKeywordRecognition(std::shared_ptr<T>& recognizer)
{
    auto name = PAL::GetTypeName(*recognizer.get());
    ConsoleWriteLine("\nStopKeywordRecognitionAsync %s...", name.c_str());
    auto future = recognizer->StopKeywordRecognitionAsync();
    ConsoleWriteLine("StopKeywordRecognitionAsync %s... Waiting...", name.c_str());
    future.get();
    ConsoleWriteLine("StopKeywordRecognitionAsync %s... Waiting... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Recognizer_Event(const char* psz, EventSignal<T>& recognizerEvent, typename::EventSignal<T>::CallbackFunction callback)
{
    if (PAL::stricmp(psz, "connect") == 0)
    {
        recognizerEvent.Connect(callback);
    }
    else if (PAL::stricmp(psz, "disconnect") == 0)
    {
        recognizerEvent.Disconnect(callback);
    }
    else if (PAL::stricmp(psz, "disconnectal") == 0)
    {
        recognizerEvent.DisconnectAll();
    }
    else
    {
        ConsoleWriteLine("\nUnknown event method: '%s'.\n\nUse 'HELP' for a list of valid commands.", psz);
    }
}

void CarbonTestConsole::ConsoleInput_Session(const char* psz)
{
    if (PAL::stricmp(psz, "from speech") == 0)
    {
        Session_FromSpeechRecognizer();
    }
    // else if (PAL::stricmp(psz, "from intent") == 0)
    // {
    //     Session_FromIntentRecognizer();
    // }
    else if (PAL::strnicmp(psz, "set string ", strlen("set string ")) == 0)
    {
        Parameters_SetString(m_session->Parameters, psz + strlen("set string "));
    }
    else if (PAL::strnicmp(psz, "get string ", strlen("get string ")) == 0)
    {
        Parameters_GetString(m_session->Parameters, psz + strlen("get string "));
    }
    else if (PAL::strnicmp(psz, "set number ", strlen("set number ")) == 0)
    {
        Parameters_SetNumber(m_session->Parameters, psz + strlen("set number "));
    }
    else if (PAL::strnicmp(psz, "get number ", strlen("get number ")) == 0)
    {
        Parameters_GetNumber(m_session->Parameters, psz + strlen("get number "));
    }
    else if (PAL::strnicmp(psz, "set bool ", strlen("set bool ")) == 0)
    {
        Parameters_SetBool(m_session->Parameters, psz + strlen("set bool "));
    }
    else if (PAL::strnicmp(psz, "get bool ", strlen("get bool ")) == 0)
    {
        Parameters_GetBool(m_session->Parameters, psz + strlen("get bool "));
    }
    else
    {
        ConsoleWriteLine("\nUnknown method/event: '%s'.\n\nUse 'HELP' for a list of valid methods/events.\n", psz);
    }
}

void CarbonTestConsole::Session_FromSpeechRecognizer()
{
    auto name = PAL::GetTypeName(*m_speechRecognizer.get());
    ConsoleWriteLine("\nGetting Session from %s...", name.c_str());
    m_session = Session::FromRecognizer(m_speechRecognizer);
    ConsoleWriteLine("Getting Session from %s... Done!\n", name.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetString(T &parameters, const char* psz)
{
    std::string input(psz);
    auto iSpace = input.find(' ');
    if (iSpace != std::string::npos && psz[iSpace + 1] != '\0')
    {
        std::string name(psz, iSpace);
        std::string value(psz + iSpace + 1);
        parameters.SetProperty(name, value);
        ConsoleWriteLine("Set string '%s' to '%s'!\n", name.c_str(), psz + iSpace + 1);
    }
    else
    {
        ConsoleWriteLine("\nInvalid usage: '%s'.\n\nUse 'HELP' for valid usage.\n", psz);
    }
}

template <class T>
void CarbonTestConsole::Parameters_GetString(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get string '%s' : '%s'\n", psz, value.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetNumber(T &parameters, const char* psz)
{
    return Parameters_SetString(parameters, psz);
}

template <class T>
void CarbonTestConsole::Parameters_GetBool(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get bool '%s' : %s\n", psz, value.c_str());
}

template <class T>
void CarbonTestConsole::Parameters_SetBool(T &parameters, const char* psz)
{
    return Parameters_SetString(parameters, psz);
}

template <class T>
void CarbonTestConsole::Parameters_GetNumber(T &parameters, const char* psz)
{
    auto value = parameters.GetProperty(psz);
    ConsoleWriteLine("Get number '%s' : %d\n", psz, value.c_str());
}

bool CarbonTestConsole::ToBool(const char* psz)
{
    return PAL::ToBool(psz);
}

std::string CarbonTestConsole::BoolToString(bool f)
{
    return PAL::BoolToString(f);
}

std::string CarbonTestConsole::ToString(const SpeechRecognitionEventArgs& e)
{
    static_assert(0 == (int)Reason::Recognized, "Reason::* enum values changed!");
    static_assert(1 == (int)Reason::IntermediateResult, "Reason::* enum values changed!");
    static_assert(2 == (int)Reason::NoMatch, "Reason::* enum values changed!");
    static_assert(3 == (int)Reason::InitialSilenceTimeout, "Reason::* enum values changed!");
    static_assert(4 == (int)Reason::InitialBabbleTimeout, "Reason::* enum values changed!");
    static_assert(5 == (int)Reason::Canceled, "Reason::* enum values changed!");

    std::string reasons[] = {
        "Recognized",
        "IntermediateResult",
        "NoMatch",
        "InitialSilenceTimeout",
        "InitialBabbleTimeout",
        "Canceled",
    };

    std::string str;
    str += "SpeechRecognitionEventArgs = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result.ResultId + "'\n";
    str += "    Reason = Reason::" + reasons[(int)e.Result.Reason] + "\n";
    str += "    ErrorDetails = '" + e.Result.ErrorDetails + "'\n";
    str += "    Text = '" + e.Result.Text + "'\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

std::string CarbonTestConsole::ToString(const IntentRecognitionEventArgs& e)
{
    static_assert(0 == (int)Reason::Recognized, "Reason::* enum values changed!");
    static_assert(1 == (int)Reason::IntermediateResult, "Reason::* enum values changed!");
    static_assert(2 == (int)Reason::NoMatch, "Reason::* enum values changed!");
    static_assert(3 == (int)Reason::InitialSilenceTimeout, "Reason::* enum values changed!");
    static_assert(4 == (int)Reason::InitialBabbleTimeout, "Reason::* enum values changed!");
    static_assert(5 == (int)Reason::Canceled, "Reason::* enum values changed!");

    std::string reasons[] = {
        "Recognized",
        "IntermediateResult",
        "NoMatch",
        "InitialSilenceTimeout",
        "InitialBabbleTimeout",
        "Canceled",
    };

    std::string str;
    str += "IntentRecognitionEventArgs = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result.ResultId + "'\n";
    str += "    Reason = Reason::" + reasons[(int)e.Result.Reason] + "\n";
    str += "    ErrorDetails = '" + e.Result.ErrorDetails + "'\n";
    str += "    Text = '" + e.Result.Text + "'\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

std::string CarbonTestConsole::ToString(const TranslationTextResultEventArgs& e)
{
    std::string str;
    str += "TranslationEventArgs<TranslationTextResult> = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    ResultId = '" + e.Result.ResultId + "'\n";
    str += "    RecognizedText = '" + e.Result.Text + "'\n";
    str += "    ErrorDetails = '" + e.Result.ErrorDetails + "'\n";
    for (auto it : e.Result.Translations)
    {
        str += "    Translation to " + it.first + ": " + it.second + ".";
    }
    str += "  } \n";
    str += "} \n";


    return str;
}

std::string CarbonTestConsole::ToString(const TranslationSynthesisResultEventArgs& e)
{
    std::string str;
    str += "TranslationEventArgs<TranslationSynthesisResult> = { \n";
    str += "  SessionId = '" + e.SessionId + "'\n";
    str += "  Result = {\n";
    str += "    SizeOfAudioData = " + std::to_string(e.Result.Audio.size()) + "\n";
    str += "  } \n";
    str += "} \n";

    return str;
}

void CarbonTestConsole::InitGlobalParameters(ConsoleArgs* pconsoleArgs)
{
    m_regionId = R"(westus)";

    if (pconsoleArgs->m_useMockMicrophone)
    {
        SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-Microphone)", true);
        SpxSetMockParameterNumber(R"(CARBON-INTERNAL-MOCK-RealTimeAudioPercentage)", pconsoleArgs->m_mockMicrophoneRealTimePercentage);
        if (!pconsoleArgs->m_mockWavFileName.empty())
        {
            SpxSetMockParameterString(R"(CARBON-INTERNAL-MOCK-WavFileAudio)", pconsoleArgs->m_mockWavFileName.c_str());
            SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-ContinuousAudio)", true);
        }
    }

    if (pconsoleArgs->m_useMockKws)
    {
        SpxSetMockParameterBool(R"(CARBON-INTERNAL-MOCK-SdkKwsEngine)", true);
    }

    if (!pconsoleArgs->m_strUseRecoEngineProperty.empty())
    {
        SpxSetMockParameterBool(pconsoleArgs->m_strUseRecoEngineProperty.c_str(), true);
    }

    if (!pconsoleArgs->m_strUseLuEngineProperty.empty())
    {
        SpxSetMockParameterBool(pconsoleArgs->m_strUseLuEngineProperty.c_str(), true);
    }

    if (!pconsoleArgs->m_strSubscriptionKey.empty())
    {
        m_subscriptionKey = pconsoleArgs->m_strSubscriptionKey;
    }

    if (!pconsoleArgs->m_strRegion.empty())
    {
        m_regionId = pconsoleArgs->m_strRegion;
    }

    if (!pconsoleArgs->m_strEndpointUri.empty())
    {
        m_endpointUri = pconsoleArgs->m_strEndpointUri;
    }

    if (!pconsoleArgs->m_strCustomSpeechModelId.empty())
    {
        m_customSpeechModelId = pconsoleArgs->m_strCustomSpeechModelId;
    }

    if (!pconsoleArgs->m_strIntentAppId.empty())
    {
        m_intentAppId = pconsoleArgs->m_strIntentAppId;
    }

    if (!pconsoleArgs->m_strIntentNames.empty())
    {
        std::string::size_type i, j;
        const std::string &s = pconsoleArgs->m_strIntentNames;

        for (i = j = 0; j = s.find(",", i), j < std::string::npos; i = j + 1)
        {
            if (i < j)
                m_intentNames.push_back(s.substr(i, j - i));
        }
        if (i < s.length())
            m_intentNames.push_back(s.substr(i));
    }
}

void CarbonTestConsole::EnsureInitCarbon(ConsoleArgs* pconsoleArgs)
{
    if (ShouldInitCarbon())
    {
        InitCarbon(pconsoleArgs);
    }
}

void CarbonTestConsole::TermCarbon()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    m_recognizer = nullptr;
    m_speechRecognizer = nullptr;
    m_intentRecognizer = nullptr;
    m_commandSystem = nullptr;
}

void CarbonTestConsole::InitCarbon(ConsoleArgs* pconsoleArgs)
{
    try
    {
        InitRecognizer(pconsoleArgs->m_strRecognizerType, pconsoleArgs->m_audioInput);
        InitCommandSystem();
    }
    catch (std::exception ex)
    {
        TermCarbon();
    }
}

void CarbonTestConsole::InitRecognizer(const std::string& recognizerType, const std::string& wavFileName)
{
    auto factory = !m_endpointUri.empty()
        ? SpeechFactory::FromEndpoint(m_endpointUri, m_subscriptionKey)
        : SpeechFactory::FromSubscription(m_subscriptionKey, m_regionId);

    if (recognizerType == PAL::GetTypeName<SpeechRecognizer>())
    {
        m_speechRecognizer = (wavFileName.length() == 0)
            ? factory->CreateSpeechRecognizer()
            : factory->CreateSpeechRecognizerWithFileInput(wavFileName);

        auto fn1 = std::bind(&CarbonTestConsole::SpeechRecognizer_FinalResultHandler, this, std::placeholders::_1);
        m_speechRecognizer->FinalResult.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::SpeechRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        m_speechRecognizer->IntermediateResult.Connect(fn2);

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_speechRecognizer);
        m_session = Session::FromRecognizer(m_speechRecognizer);
    }
    else if (recognizerType == PAL::GetTypeName<TranslationRecognizer>())
    {
        m_translationRecognizer = factory->CreateTranslationRecognizer("en-us", std::vector<std::string>{"zh-cn"});

        auto fn1 = std::bind(&CarbonTestConsole::TranslationRecognizer_FinalResultHandler, this, std::placeholders::_1);
        m_translationRecognizer->FinalResult.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::TranslationRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        m_translationRecognizer->IntermediateResult.Connect(fn2);

        auto fn3 = std::bind(&CarbonTestConsole::TranslationRecognizer_SynthesisResultHandler, this, std::placeholders::_1);
        m_translationRecognizer->TranslationSynthesisResultEvent.Connect(fn3);

        // Todo: add error handler
        //auto fn4 = std::bind(&CarbonTestConsole::TranslationRecognizer_ErrorHandler, this, std::placeholders::_1);
        //m_translationRecognizer->OnTranslationError.Connect(fn4);
    }
    else if (recognizerType == PAL::GetTypeName<IntentRecognizer>())
    {
        m_intentRecognizer = wavFileName.length() == 0
            ? factory->CreateIntentRecognizer()
            : factory->CreateIntentRecognizerWithFileInput(wavFileName);

        auto fn1 = std::bind(&CarbonTestConsole::IntentRecognizer_FinalResultHandler, this, std::placeholders::_1);
        m_intentRecognizer->FinalResult.Connect(fn1);

        auto fn2 = std::bind(&CarbonTestConsole::IntentRecognizer_IntermediateResultHandler, this, std::placeholders::_1);
        m_intentRecognizer->IntermediateResult.Connect(fn2);

        if (!m_intentAppId.empty())
        {
            auto model = LanguageUnderstandingModel::FromAppId(m_intentAppId);
            for (auto & intentName : m_intentNames)
            {
                m_intentRecognizer->AddIntent(intentName, model, intentName);
            }
        }

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_intentRecognizer);
        m_session = Session::FromRecognizer(m_intentRecognizer);

        m_recognizer = BaseAsyncRecognizer::FromRecognizer(m_intentRecognizer);
        // TODO: RobCh: Intent: Hook up Session::FromRecognizer
        // m_session = Session::FromRecognizer(m_speechRecognizer);
    }
}

void CarbonTestConsole::InitCommandSystem()
{
    // TODO: ROBCH: Implement CarbonTestConsole::InitCommandSystem
}

void CarbonTestConsole::WaitForDebugger()
{
    PAL_WaitForDebugger();
}

void CarbonTestConsole::RecognizeAsync()
{
    if (m_intentRecognizer != nullptr)
    {
        Recognizer_Recognize(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        Recognizer_Recognize(m_speechRecognizer);
    }
    else if (m_translationRecognizer != nullptr)
    {
        Recognizer_Recognize(m_translationRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        Recognizer_Recognize(m_recognizer);
    }
}

void CarbonTestConsole::ContinuousRecognition(uint16_t seconds)
{
    if (m_intentRecognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_intentRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        Recognizer_StopContinuousRecognition(m_intentRecognizer);
    }
    else if (m_speechRecognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_speechRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        Recognizer_StopContinuousRecognition(m_speechRecognizer);
    }
    else if (m_translationRecognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_translationRecognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        Recognizer_StopContinuousRecognition(m_translationRecognizer);
    }
    else if (m_recognizer != nullptr)
    {
        Recognizer_StartContinuousRecognition(m_recognizer);

        ConsoleWrite("Waiting for %d seconds... ", seconds);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ConsoleWriteLine("Done!");

        Recognizer_StopContinuousRecognition(m_recognizer);
    }
}

void CarbonTestConsole::RunSample(const std::string& strSampleName)
{
    if (PAL::stricmp(strSampleName.c_str(), "helloworld") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld with events") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_WithEvents();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld c") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_In_C();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld usp") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Usp");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld unidec") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Unidec");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld mockengine") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_PickEngine("Mock");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld subscription with cris") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Subscription_With_CRIS();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "helloworld french") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        Sample_HelloWorld_Language("fr-fr");
    }
    else if (PAL::stricmp(strSampleName.c_str(), "channel9") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        channel9();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech_intermediate") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech_intermediate();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_speech_continuous") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_speech_continuous();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_intent_continuous") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_intent_continuous();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_kws_speech") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_kws_speech();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_kws_intent") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_kws_intent();
    }
    else if (PAL::stricmp(strSampleName.c_str(), "do_translation") == 0)
    {
        ConsoleWriteLine("Running sample: %s\n", strSampleName.c_str());
        ch9_do_translation();
    }
    else
    {
        ConsoleWriteLine("\nUnknown sample: '%s'.\n", strSampleName.c_str());
    }
}

void CarbonTestConsole::RunInteractivePrompt()
{
    std::string strInput;
    while (GetConsoleInput(strInput))
    {
        if (PAL::stricmp(strInput.c_str(), "exit") == 0) break;
        if (PAL::stricmp(strInput.c_str(), "quit") == 0) break;

        try
        {
            ProcessConsoleInput(strInput.c_str());
        }
        catch (std::exception ex)
        {
            SPX_TRACE_ERROR("CarbonX: Exception caught: %s!", ex.what());
        }
    }
}


int SafeMain(const std::vector<std::string>& args)
{
    SPX_DBG_TRACE_SCOPE("\r\n=== CarbonX (Carbon Test Console) started\n\n", "\r\n=== CarbonX (carbon test console) ended");

    auto unhandledException = []() ->void { SPX_TRACE_ERROR("CarbonX: Unhandled exception!"); };
    std::set_terminate(unhandledException);

    try
    {
        CarbonTestConsole test;
        auto result = test.Run(args);
        return result;
    }
    catch (std::exception e)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread! what=%s", e.what());
        exit(-1);
    }
    catch (...)
    {
        SPX_TRACE_ERROR("CarbonX: Unhandled exception on main thread!");
        exit(-1);
    }
}

#ifdef _MSC_VER

#ifdef _DEBUG
// in case of asserts in debug mode, print the message into stderr and throw exception
int HandleDebugAssert(int,               // reportType  - ignoring reportType, printing message and aborting for all reportTypes
    char *message,     // message     - fully assembled debug user message
    int * returnValue) // returnValue - retVal value of zero continues execution
{
    fprintf(stderr, "C-Runtime: %s\n", message);

    if (returnValue) {
        *returnValue = 0;   // return value of 0 will continue operation and NOT start the debugger
    }

    return 1;            // make sure no message box is displayed
}
#endif

int wmain(int argc, const wchar_t* argv[])
{
#ifdef _DEBUG
    // in case of asserts in debug mode, print the message into stderr and throw exception
    if (_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, HandleDebugAssert) == -1) {
        fprintf(stderr, "_CrtSetReportHook2 failed.\n");
        return -1;
    }
#endif
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(PAL::ToString(argv[i]));
    }

    return SafeMain(args);
}
#else
#include <cstring>
#include <cassert>

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    return SafeMain(args);
}
#endif
