//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Performs translation on the speech input.
    /// </summary>
    /// <example>
    /// An example to use the translation recognizer from microphone and listen to events generated by the recognizer.
    /// <code>
    /// public async Task TranslationContinuousRecognitionAsync()
    /// {
    ///     // Creates an instance of a speech factory with specified subscription key and service region. 
    ///     // Replace with your own subscription key and service region (e.g., "westus").
    ///     var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
    ///
    ///     // Sets source and target languages.
    ///     string fromLanguage = "en-US";
    ///     <![CDATA[List<string> toLanguages = new List<string>() { "de" };]]>
    ///
    ///     // Sets voice name of synthesis output.
    ///     const string GermanVoice = "de-DE-Hedda";
    ///
    ///     // Creates a translation recognizer using microphone as audio input, and requires voice output.
    ///     using (var recognizer = factory.CreateTranslationRecognizer(fromLanguage, toLanguages, GermanVoice))
    ///     {
    ///         // Subscribes to events.
    ///         recognizer.IntermediateResultReceived += (s, e) =>
    ///         {
    ///             Console.WriteLine($"\nPartial result: recognized in {fromLanguage}: {e.Result.Text}.");
    ///             if (e.Result.TranslationStatus == TranslationStatus.Success)
    ///             {
    ///                 foreach (var element in e.Result.Translations)
    ///                 {
    ///                     Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
    ///                 }
    ///             }
    ///         };
    ///
    ///         recognizer.FinalResultReceived += (s, e) =>
    ///         {
    ///             var result = e.Result;
    ///             if (result.RecognitionStatus == RecognitionStatus.Recognized)
    ///             {
    ///                 Console.WriteLine($"\nFinal result: Status: {result.RecognitionStatus.ToString()}, recognized text in {fromLanguage}: {result.Text}.");
    ///                 if (result.TranslationStatus == TranslationStatus.Success)
    ///                 {
    ///                     foreach (var element in result.Translations)
    ///                     {
    ///                         Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
    ///                     }
    ///                 }
    ///             }
    ///         };
    ///
    ///         recognizer.SynthesisResultReceived += (s, e) =>
    ///         {
    ///             if (e.Result.Status == SynthesisStatus.Success)
    ///             {
    ///                 Console.WriteLine($"Synthesis result received. Size of audio data: {e.Result.Audio.Length}");
    ///             }
    ///             else if (e.Result.Status == SynthesisStatus.SynthesisEnd)
    ///             {
    ///                 Console.WriteLine($"Synthesis result: end of synthesis result.");
    ///             }
    ///             else
    ///             {
    ///                 Console.WriteLine($"Synthesis error. Status: {e.Result.Status.ToString()}, Failure reason: {e.Result.FailureReason}");
    ///             }
    ///         };
    ///
    ///         recognizer.RecognitionErrorRaised += (s, e) =>
    ///         {
    ///             Console.WriteLine($"\nAn error occurred. Status: {e.Status.ToString()}");
    ///         };
    ///
    ///         recognizer.OnSessionEvent += (s, e) =>
    ///         {
    ///             Console.WriteLine($"\nSession event. Event: {e.EventType.ToString()}.");
    ///         };
    ///
    ///         // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    ///         Console.WriteLine("Say something...");
    ///         await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
    ///
    ///         do
    ///         {
    ///             Console.WriteLine("Press Enter to stop");
    ///         } while (Console.ReadKey().Key != ConsoleKey.Enter);
    ///
    ///         // Stops continuous recognition.
    ///         await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
    ///     }
    /// }
    /// </code>
    /// </example>
    public sealed class TranslationRecognizer : Recognizer
    {
        /// <summary>
        /// The event <see cref="IntermediateResultReceived"/> signals that an intermediate recognition result is received.
        /// </summary>
        public event EventHandler<TranslationTextResultEventArgs> IntermediateResultReceived;

        /// <summary>
        /// The event <see cref="FinalResultReceived"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<TranslationTextResultEventArgs> FinalResultReceived;

        /// <summary>
        /// The event <see cref="RecognitionErrorRaised"/> signals that an error occurred during recognition.
        /// </summary>
        public event EventHandler<RecognitionErrorEventArgs> RecognitionErrorRaised;

        /// <summary>
        /// The event <see cref="SynthesisResultReceived"/> signals that a translation synthesis result is received.
        /// </summary>
        public event EventHandler<TranslationSynthesisResultEventArgs> SynthesisResultReceived;

        internal TranslationRecognizer(Internal.TranslationRecognizer recoImpl)
        {
            this.recoImpl = recoImpl;

            intermediateResultHandler = new ResultHandlerImpl(this, isFinalResultHandler: false);
            recoImpl.IntermediateResult.Connect(intermediateResultHandler);

            finalResultHandler = new ResultHandlerImpl(this, isFinalResultHandler: true);
            recoImpl.FinalResult.Connect(finalResultHandler);

            synthesisResultHandler = new SynthesisHandlerImpl(this);
            recoImpl.TranslationSynthesisResultEvent.Connect(synthesisResultHandler);

            errorHandler = new ErrorHandlerImpl(this);
            recoImpl.Canceled.Connect(errorHandler);

            recoImpl.SessionStarted.Connect(sessionStartedHandler);
            recoImpl.SessionStopped.Connect(sessionStoppedHandler);
            recoImpl.SpeechStartDetected.Connect(speechStartDetectedHandler);
            recoImpl.SpeechEndDetected.Connect(speechEndDetectedHandler);

            Parameters = new RecognizerParametersImpl(recoImpl.Parameters);
        }

        internal TranslationRecognizer(Internal.TranslationRecognizer recoImpl, AudioInputStream stream) : this(recoImpl)
        {
            streamInput = stream;
        }

        /// <summary>
        /// Gets the language name that was set when the recognizer was created.
        /// </summary>
        public string SourceLanguage
        {
            get
            {
                return Parameters.Get(TranslationParameterNames.SourceLanguage);
            }
        }

        /// <summary>
        /// Gets target languages for translation that were set when the recognizer was created.
        /// The language is specified in BCP-47 format. The translation will provide translated text for each of language.
        /// </summary>
        public string[] TargetLanguages
        {
            get
            {
                var plainStr = Parameters.Get(TranslationParameterNames.TargetLanguages);
                return plainStr.Split(',');
            }
        }

        /// <summary>
        /// Gets the name of output voice.
        /// </summary>
        public string OutputVoiceName {
            get
            {
                return Parameters.Get(TranslationParameterNames.Voice);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="TranslationRecognizer"/>.
        /// </summary>
        public IRecognizerParameters Parameters { get; internal set; }

        /// <summary>
        /// Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
        /// Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
        /// </summary>
        /// <returns>A task representing the recognition operation. The task returns a value of <see cref="TranslationTextResult"/> </returns>
        /// <example>
        /// Create a translation recognizer, get and print the recognition result
        /// <code>
        /// public async Task TranslationSingleShotRecognitionAsync()
        /// {
        ///     // Creates an instance of a speech factory with specified subscription key and service region. 
        ///     // Replace with your own subscription key and service region (e.g., "westus").
        ///     var factory = SpeechFactory.FromSubscription("YourSubscriptionKey", "YourServiceRegion");
        ///
        ///     string fromLanguage = "en-US";
        ///     <![CDATA[var toLanguages = new List<string>() { "de" };]]>
        ///
        ///     // Creates a translation recognizer.
        ///     using (var recognizer = factory.CreateTranslationRecognizer(fromLanguage, toLanguages))
        ///     {
        ///         // Starts recognizing.
        ///         Console.WriteLine("Say something...");
        ///
        ///         // Performs recognition.
        ///         // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
        ///         // only for single shot recognition like command or query. For long-running recognition, use
        ///         // StartContinuousRecognitionAsync() instead.
        ///         var result = await recognizer.RecognizeAsync();
        ///
        ///         if (result.RecognitionStatus == RecognitionStatus.Recognized)
        ///         {
        ///             Console.WriteLine($"\nFinal result: Status: {result.RecognitionStatus.ToString()}, recognized text: {result.Text}.");
        ///             if (result.TranslationStatus == TranslationStatus.Success)
        ///             {
        ///                 foreach (var element in result.Translations)
        ///                 {
        ///                     Console.WriteLine($"    Translated into {element.Key}: {element.Value}");
        ///                 }
        ///             }
        ///         }
        ///     }
        /// }
        /// </code>
        /// </example>
        public Task<TranslationTextResult> RecognizeAsync()
        {
            return Task.Run(() => { return new TranslationTextResult(this.recoImpl.Recognize()); });
        }

        /// <summary>
        /// Starts recognition and translation on a continous audio stream, until StopContinuousRecognitionAsync() is called.
        /// User must subscribe to events to receive translation results.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartContinuousRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StartContinuousRecognition(); });
        }

        /// <summary>
        /// Stops continuous recognition and translation.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the translation.</returns>
        public Task StopContinuousRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StopContinuousRecognition(); });
        }

        /// <summary>
        /// Starts speech recognition on a continuous audio stream with keyword spotting, until StopKeywordRecognitionAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
        /// </summary>
        /// <param name="model">The keyword recognition model that specifies the keyword to be recognized.</param>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartKeywordRecognitionAsync(KeywordRecognitionModel model)
        {
            return Task.Run(() => { this.recoImpl.StartKeywordRecognition(model.modelImpl); });
        }

        /// <summary>
        /// Stops continuous speech recognition with keyword spotting.
        /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        public Task StopKeywordRecognitionAsync()
        {
            return Task.Run(() => { this.recoImpl.StopKeywordRecognition(); });
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                recoImpl?.Dispose();

                intermediateResultHandler?.Dispose();
                finalResultHandler?.Dispose();
                errorHandler?.Dispose();

                disposed = true;
                base.Dispose(disposing);
            }
        }

        internal readonly Internal.TranslationRecognizer recoImpl;
        private readonly ResultHandlerImpl intermediateResultHandler;
        private readonly ResultHandlerImpl finalResultHandler;
        private readonly SynthesisHandlerImpl synthesisResultHandler;
        private readonly ErrorHandlerImpl errorHandler;
        private bool disposed = false;
        private readonly AudioInputStream streamInput;

        // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        private class ResultHandlerImpl : Internal.TranslationTextEventListener
        {
            public ResultHandlerImpl(TranslationRecognizer recognizer, bool isFinalResultHandler)
            {
                this.recognizer = recognizer;
                this.isFinalResultHandler = isFinalResultHandler;
            }

            public override void Execute(Internal.TranslationTextResultEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                TranslationTextResultEventArgs resultEventArg = new TranslationTextResultEventArgs(eventArgs);
                var handler = isFinalResultHandler ? recognizer.FinalResultReceived : recognizer.IntermediateResultReceived;
                if (handler != null)
                {
                    handler(this.recognizer, resultEventArg);
                }
            }

            private TranslationRecognizer recognizer;
            private bool isFinalResultHandler;
        }

        // Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
        private class ErrorHandlerImpl : Internal.TranslationTextEventListener
        {
            public ErrorHandlerImpl(TranslationRecognizer recognizer)
            {
                this.recognizer = recognizer;
            }

            public override void Execute(Microsoft.CognitiveServices.Speech.Internal.TranslationTextResultEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                var resultEventArg = new RecognitionErrorEventArgs(eventArgs.SessionId, eventArgs.GetResult().Reason, eventArgs.GetResult().ErrorDetails);
                var handler = this.recognizer.RecognitionErrorRaised;

                if (handler != null)
                {
                    handler(this.recognizer, resultEventArg);
                }
            }

            private TranslationRecognizer recognizer;
        }

        // Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        private class SynthesisHandlerImpl : Internal.TranslationSynthesisEventListener
        {
            public SynthesisHandlerImpl(TranslationRecognizer recognizer)
            {
                this.recognizer = recognizer;
            }

            public override void Execute(Internal.TranslationSynthesisResultEventArgs eventArgs)
            {
                if (recognizer.disposed)
                {
                    return;
                }

                var resultEventArg = new TranslationSynthesisResultEventArgs(eventArgs);
                var handler = recognizer.SynthesisResultReceived;
                if (handler != null)
                {
                    handler(this.recognizer, resultEventArg);
                }
            }

            private TranslationRecognizer recognizer;
        }
    }

}
