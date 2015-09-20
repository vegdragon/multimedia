/*
 
    File: AQRecorder.mm
Abstract: n/a
 Version: 2.4

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
Inc. ("Apple") in consideration of your agreement to the following
terms, and your use, installation, modification or redistribution of
this Apple software constitutes acceptance of these terms.  If you do
not agree with these terms, please do not use, install, modify or
redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, Apple grants you a personal, non-exclusive
license, under Apple's copyrights in this original Apple software (the
"Apple Software"), to use, reproduce, modify and redistribute the Apple
Software, with or without modifications, in source and/or binary forms;
provided that if you redistribute the Apple Software in its entirety and
without modifications, you must retain this notice and the following
text and disclaimers in all such redistributions of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may
be used to endorse or promote products derived from the Apple Software
without specific prior written permission from Apple.  Except as
expressly stated in this notice, no other rights or licenses, express or
implied, are granted by Apple herein, including but not limited to any
patent rights that may be infringed by your derivative works or by other
works in which the Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2009 Apple Inc. All Rights Reserved.

 
*/

#include "AQRecorder.h"

//**********************
// pitch table
//**********************
static char pitchName[][10] = {
    "C#0","D0","D#0","E0","F0","F#0","G0","A0","A#0",
    "B0","C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1",
    "A1","A#1","B1","C2","C#2","D2","D#2","E2","F2","F#2",
    "G2","G#2","A2","A#2","B2","C3","C#3","D3","C4","C#4",
    "D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
    "C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
    "C6"
};

static double pitchFrequency[] = {
    34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27,
    61.73, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47,
    130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
    261.63, 277.18, 293.66, 523.25, 554.36, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,
    1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.21, 1760.00, 1864.65, 1975.33,
    2093.00,
};

// search algorithm
int searchFrequencyInArray(double frequency, int startIdx, int endIdx)
{
    if (abs(endIdx-startIdx)<=1)
    {
        return endIdx;
    }
    else if (frequency < pitchFrequency[startIdx + ((endIdx-startIdx)>>1)])
    {
        return searchFrequencyInArray(frequency, startIdx, startIdx + ((endIdx-startIdx)>>1));
    }
    else
    {
        return searchFrequencyInArray(frequency, startIdx + ((endIdx-startIdx)>>1), endIdx);
    }
}

// return pitch name according to frequency
const char * dywapitch_getPitchName(double frequency)
{
    int idx = -1;
    const int countOfFrequencies = sizeof(pitchFrequency)/sizeof(double);
    int a, b, c;
    
    if (frequency>pitchFrequency[countOfFrequencies-1]
        || frequency<pitchFrequency[0])
    {
        return NULL;
    }
    
    idx = searchFrequencyInArray(frequency, 0, countOfFrequencies-1);
    a = abs(frequency - pitchFrequency[idx]);
    b = idx>=countOfFrequencies-1 ? 0xffff:abs(frequency - pitchFrequency[idx+1]);
    c = idx<=0 ? 0xffff:abs(frequency - pitchFrequency[idx-1]);
    
    if (a>b)
    {
        if (b>c) {
            idx = idx - 1;
        }
        else {
            idx = idx + 1;
        }
    }
    else
    {
        if (a>c) {
            idx = idx + 1;
        }
    }
    
    return pitchName[idx];
}

// ____________________________________________________________________________________
// Determine the size, in bytes, of a buffer necessary to represent the supplied number
// of seconds of audio data.
int AQRecorder::ComputeRecordBufferSize(const AudioStreamBasicDescription *format, float seconds)
{
    int packets, frames, bytes = 0;
    try {
        frames = (int)ceil(seconds * format->mSampleRate);
        
        if (format->mBytesPerFrame > 0)
            bytes = frames * format->mBytesPerFrame;
        else {
            UInt32 maxPacketSize;
            if (format->mBytesPerPacket > 0)
                maxPacketSize = format->mBytesPerPacket;    // constant packet size
            else {
                UInt32 propertySize = sizeof(maxPacketSize);
                XThrowIfError(AudioQueueGetProperty(mQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
                                                 &propertySize), "couldn't get queue's maximum output packet size");
            }
            if (format->mFramesPerPacket > 0)
                packets = frames / format->mFramesPerPacket;
            else
                packets = frames;    // worst-case scenario: 1 frame in a packet
            if (packets == 0)        // sanity check
                packets = 1;
            bytes = packets * maxPacketSize;
        }
    } catch (CAXException e) {
        char buf[256];
        fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
        return 0;
    }    
    return bytes;
}

#define USE_MUSIC_ANALYZER
#ifdef USE_MUSIC_ANALYZER
void AQRecorder::MyInputBufferHandler(  void *                                inUserData,
                                      AudioQueueRef                        inAQ,
                                      AudioQueueBufferRef                    inBuffer,
                                      const AudioTimeStamp *                inStartTime,
                                      UInt32                                inNumPackets,
                                      const AudioStreamPacketDescription*    inPacketDesc)
{
    AQRecorder *aqr = (AQRecorder *)inUserData;
    PitchDetector   pitchDetector;
    NoteDesc        noteDesc;
    
    // printf("inNumPackets = %d\n", inNumPackets);
    
    pitchDetector.init();
    
    try {
        // [AS] compute pitch
        if (inNumPackets > 0) {
            // SamplesNode sn;
            /*
            for (int j = 0;j<inNumPackets; j++) {
                // printf("j=%d, inNumPackets = %d\n", j, inNumPackets);
                short *samples16 = (short *)inBuffer->mAudioData;
                sn.samples[j] = (float)samples16[j];
            }
             */
            aqr->mMusicAnalyzer->enqueueSamples((short*)inBuffer->mAudioData, inNumPackets);
            
            /*
            aqr->currentPitch = pitchDetector.detectPitch(&sn);
            // printf("[%f ]", aqr->currentPitch);
            
            if (0 ==aqr->currentPitch)
            {
                printf("[ ]");
            }
            else
            {
                // printf("Pitch: %f\t\t", aqr->currentPitch);
                // printf("\nPitch Name: %s\n", dywapitch_getPitchName(aqr->currentPitch));
                NSDate *now = [NSDate date];
                printf("\n[%f][%s(%s) - %f][%f\%]",
                       [now timeIntervalSince1970],
                       sn.noteDesc.pitchName,
                       sn.noteDesc.toneName,
                       sn.noteDesc.pitchValue,
                       sn.noteDesc.deviation);
            }
             */
            
        }
        
        // if we're not stopping, re-enqueue the buffe so that it gets filled again
        if (aqr->IsRunning())
            XThrowIfError(AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL), "AudioQueueEnqueueBuffer failed");
    } catch (CAXException e) {
        char buf[256];
        fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
    }
    
    pitchDetector.release();
    
    // printf("Exiting...\n");
}

#else   // USE_MUSIC_ANALYZER
// ____________________________________________________________________________________
// AudioQueue callback function, called when an input buffers has been filled.
void AQRecorder::MyInputBufferHandler(  void *                                inUserData,
                                        AudioQueueRef                        inAQ,
                                        AudioQueueBufferRef                    inBuffer,
                                        const AudioTimeStamp *                inStartTime,
                                        UInt32                                inNumPackets,
                                        const AudioStreamPacketDescription*    inPacketDesc)
{
    AQRecorder *aqr = (AQRecorder *)inUserData;
    try {
        /* don't write to file
        if (inNumPackets > 0) {
            // write packets to file
            XThrowIfError(AudioFileWritePackets(aqr->mRecordFile, FALSE, inBuffer->mAudioDataByteSize,
                                             inPacketDesc, aqr->mRecordPacket, &inNumPackets, inBuffer->mAudioData),
                       "AudioFileWritePackets failed");
            aqr->mRecordPacket += inNumPackets;
        }
         */
        
        // [AS] compute pitch
        if (inNumPackets > 0) {
            // first convert from shorts to doubles
            for (int j = 0; j < inNumPackets; j++) {
                short *samples16 = (short *)inBuffer->mAudioData;
                aqr->currentFloatSamples[j] = (float)samples16[j];
            }
            // compute pitch
            aqr->currentPitch = dywapitch_computepitch(&(aqr->pitchtracker), aqr->currentFloatSamples, 0, inNumPackets);
            printf("Pitch: %f\t\t", aqr->currentPitch);
            extern const char* dywapitch_getPitchName(double);
            printf("Pitch Name: %s\n", dywapitch_getPitchName(aqr->currentPitch));
        }
        
        // if we're not stopping, re-enqueue the buffe so that it gets filled again
        if (aqr->IsRunning())
            XThrowIfError(AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL), "AudioQueueEnqueueBuffer failed");
    } catch (CAXException e) {
        char buf[256];
        fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
    }
}
#endif  // USE_MUSIC_ANALYZER


AQRecorder::AQRecorder():mMusicAnalyzer(NULL)
{
    mIsRunning = false;
    mRecordPacket = 0;
}

AQRecorder::~AQRecorder()
{
    AudioQueueDispose(mQueue, TRUE);
    AudioFileClose(mRecordFile);
    if (mFileName) CFRelease(mFileName);
    // [AS]
    if (currentFloatSamples != NULL) free(currentFloatSamples);
    currentFloatSamples = NULL;
}

// ____________________________________________________________________________________
// Copy a queue's encoder's magic cookie to an audio file.
void AQRecorder::CopyEncoderCookieToFile()
{
    UInt32 propertySize;
    // get the magic cookie, if any, from the converter        
    OSStatus err = AudioQueueGetPropertySize(mQueue, kAudioQueueProperty_MagicCookie, &propertySize);
    
    // we can get a noErr result and also a propertySize == 0
    // -- if the file format does support magic cookies, but this file doesn't have one.
    if (err == noErr && propertySize > 0) {
        Byte *magicCookie = new Byte[propertySize];
        UInt32 magicCookieSize;
        XThrowIfError(AudioQueueGetProperty(mQueue, kAudioQueueProperty_MagicCookie, magicCookie, &propertySize), "get audio converter's magic cookie");
        magicCookieSize = propertySize;    // the converter lies and tell us the wrong size
        
        // now set the magic cookie on the output file
        UInt32 willEatTheCookie = false;
        // the converter wants to give us one; will the file take it?
        err = AudioFileGetPropertyInfo(mRecordFile, kAudioFilePropertyMagicCookieData, NULL, &willEatTheCookie);
        if (err == noErr && willEatTheCookie) {
            err = AudioFileSetProperty(mRecordFile, kAudioFilePropertyMagicCookieData, magicCookieSize, magicCookie);
            XThrowIfError(err, "set audio file's magic cookie");
        }
        delete[] magicCookie;
    }
}

void AQRecorder::SetupAudioFormat(UInt32 inFormatID)
{
    memset(&mRecordFormat, 0, sizeof(mRecordFormat));

    UInt32 size = sizeof(mRecordFormat.mSampleRate);
    XThrowIfError(AudioSessionGetProperty(    kAudioSessionProperty_CurrentHardwareSampleRate,
                                        &size, 
                                        &mRecordFormat.mSampleRate), "couldn't get hardware sample rate");

    size = sizeof(mRecordFormat.mChannelsPerFrame);
    XThrowIfError(AudioSessionGetProperty(    kAudioSessionProperty_CurrentHardwareInputNumberChannels, 
                                        &size, 
                                        &mRecordFormat.mChannelsPerFrame), "couldn't get input channel count");
            
    mRecordFormat.mFormatID = inFormatID;
    if (inFormatID == kAudioFormatLinearPCM)
    {
        // if we want pcm, default to signed 16-bit little-endian
        mRecordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        mRecordFormat.mBitsPerChannel = 16;
        mRecordFormat.mBytesPerPacket = mRecordFormat.mBytesPerFrame = (mRecordFormat.mBitsPerChannel / 8) * mRecordFormat.mChannelsPerFrame;
        mRecordFormat.mFramesPerPacket = 1;
    }
}

void AQRecorder::StartRecord(CFStringRef inRecordFile)
{
    int i, bufferByteSize;
    UInt32 size;
    CFURLRef url;
    
    try {        
        mFileName = CFStringCreateCopy(kCFAllocatorDefault, inRecordFile);

        // specify the recording format
        SetupAudioFormat(kAudioFormatLinearPCM);
        
        // create the queue
        XThrowIfError(AudioQueueNewInput(
                                      &mRecordFormat,
                                      MyInputBufferHandler,
                                      this /* userData */,
                                      NULL /* run loop */, NULL /* run loop mode */,
                                      0 /* flags */, &mQueue), "AudioQueueNewInput failed");
        
        // get the record format back from the queue's audio converter --
        // the file may require a more specific stream description than was necessary to create the encoder.
        mRecordPacket = 0;

        size = sizeof(mRecordFormat);
        XThrowIfError(AudioQueueGetProperty(mQueue, kAudioQueueProperty_StreamDescription,    
                                         &mRecordFormat, &size), "couldn't get queue's format");
        
        /**** [AS] don't save to file
        NSString *recordFile = [NSTemporaryDirectory() stringByAppendingPathComponent: (NSString*)inRecordFile];    
            
        url = CFURLCreateWithString(kCFAllocatorDefault, (CFStringRef)recordFile, NULL);
        
        // create the audio file
        XThrowIfError(AudioFileCreateWithURL(url, kAudioFileCAFType, &mRecordFormat, kAudioFileFlags_EraseFile,
                                          &mRecordFile), "AudioFileCreateWithURL failed");
        CFRelease(url);
        
        // copy the cookie first to give the file object as much info as we can about the data going in
        // not necessary for pcm, but required for some compressed audio
        CopyEncoderCookieToFile();
        ****/
        
        // allocate and enqueue buffers
        bufferByteSize = ComputeRecordBufferSize(&mRecordFormat, kBufferDurationSeconds);    // enough bytes for half a second
        for (i = 0; i < kNumberRecordBuffers; ++i) {
            XThrowIfError(AudioQueueAllocateBuffer(mQueue, bufferByteSize, &mBuffers[i]),
                       "AudioQueueAllocateBuffer failed");
            XThrowIfError(AudioQueueEnqueueBuffer(mQueue, mBuffers[i], 0, NULL),
                       "AudioQueueEnqueueBuffer failed");
        }
        
        // [AS] allow tmp float buffer, and start tracking pitch
        currentFloatSamples = (double *)malloc((bufferByteSize/mRecordFormat.mBytesPerFrame)*sizeof(double));
        dywapitch_inittracking(&pitchtracker);
        
        // start the queue
        mIsRunning = true;
        XThrowIfError(AudioQueueStart(mQueue, NULL), "AudioQueueStart failed");
    }
    catch (CAXException &e) {
        char buf[256];
        fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
    }
    catch (...) {
        fprintf(stderr, "An unknown error occurred\n");
    }
    
    if (NULL==mMusicAnalyzer)
    {
        mMusicAnalyzer = new MusicAnalyzer();
        mMusicAnalyzer->init();
        mMusicAnalyzer->startAnalyzer();
    }

}

void AQRecorder::StopRecord()
{
    // end recording
    mIsRunning = false;
    XThrowIfError(AudioQueueStop(mQueue, true), "AudioQueueStop failed");    
    // a codec may update its cookie at the end of an encoding session, so reapply it to the file now
    CopyEncoderCookieToFile();
    if (mFileName)
    {
        CFRelease(mFileName);
        mFileName = NULL;
    }
    // [AS]
    if (currentFloatSamples != NULL) free(currentFloatSamples);
    currentFloatSamples = NULL;
    
    AudioQueueDispose(mQueue, true);
    AudioFileClose(mRecordFile);
    
    if (NULL!=mMusicAnalyzer)
    {
        mMusicAnalyzer->release();
        delete mMusicAnalyzer;
        mMusicAnalyzer = NULL;
    }
}
