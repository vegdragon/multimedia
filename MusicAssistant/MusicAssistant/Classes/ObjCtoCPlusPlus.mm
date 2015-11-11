#import "ObjCtoCPlusPlus.h"

#import "AQRecorder.h"
#import "AQPlayer.h"

void interruptionListener(	void *	inClientData,
                          UInt32	inInterruptionState);

void propListener(	void *                  inClientData,
                  AudioSessionPropertyID	inID,
                  UInt32                  inDataSize,
                  const void *            inData);

@implementation AQRecorderObjC
+ (void) initRecorder {
    
#if IOS7
    if ([[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryRecord error:nil])
    {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(interruptionListener:) name:AVAudioSessionInterruptionNotification object:nil];
        if ([[AVAudioSession sharedInstance] setActive:YES error:NULL])
        {
            //go on
        }
    };
    
#else
    OSStatus error = AudioSessionInitialize(NULL, NULL, interruptionListener, (__bridge  void *)(self));
    if (error) {
        printf("ERROR INITIALIZING AUDIO SESSION! %d, %x\n", (int)error, (int)error);
        switch (error) {
            case kAudioSessionAlreadyInitialized:
                break;
            case kAudioSessionInitializationError:
                break;
            case kAudioSessionUnsupportedPropertyError:
                break;
            case kAudioSessionBadPropertySizeError:
                break;
            case kAudioSessionNotActiveError:
                break;
            case kAudioServicesNoHardwareError:
                break;
            case kAudioSessionNoCategorySet:
                break;
            case kAudioSessionIncompatibleCategory:
                break;
            case kAudioSessionUnspecifiedError:
                break;
                
            default:
                break;
        }
    }
    else
    {
        UInt32 category = kAudioSessionCategory_PlayAndRecord;
        error = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        if (error) printf("couldn't set audio category!");
        
        error = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, propListener, NULL);
        if (error) printf("ERROR ADDING AUDIO SESSION PROP LISTENER! %d\n", (int)error);
        UInt32 inputAvailable = 0;
        UInt32 size = sizeof(inputAvailable);
        
        // we do not want to allow recording if input is not available
        error = AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &size, &inputAvailable);
        if (error) printf("ERROR GETTING INPUT AVAILABILITY! %d\n", (int)error);
        
        // we also need to listen to see if input availability changes
        error = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioInputAvailable, propListener, NULL);
        if (error) printf("ERROR ADDING AUDIO SESSION PROP LISTENER! %d\n", (int)error);
        
        error = AudioSessionSetActive(true); 
        if (error) printf("AudioSessionSetActive (true) failed");
    }
#endif

}

+ (void) startRecord {
    AQRecorder::getInstance()->StartRecord(CFSTR("recordedFile.caf"));
}

+ (void) stopRecord {
    AQRecorder::getInstance()->StopRecord();
}

@end


#pragma mark AudioSession listeners
void interruptionListener(	void *	inClientData,
                          UInt32	inInterruptionState)
{
//    SpeakHereController *THIS = (SpeakHereController*)inClientData;
    if (inInterruptionState == kAudioSessionBeginInterruption)
    {
        if (AQRecorder::getInstance()->IsRunning()) {
            AQRecorder::getInstance()->StopRecord();
        }
//        else if (THIS->player->IsRunning()) {
//            //the queue will stop itself on an interruption, we just need to update the UI
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"playbackQueueStopped" object:THIS];
//            THIS->playbackWasInterrupted = YES;
//        }
//    }
//    else if ((inInterruptionState == kAudioSessionEndInterruption) && THIS->playbackWasInterrupted)
//    {
//        // we were playing back when we were interrupted, so reset and resume now
//        THIS->player->StartQueue(true);
//        [[NSNotificationCenter defaultCenter] postNotificationName:@"playbackQueueResumed" object:THIS];
//        THIS->playbackWasInterrupted = NO;
//    }
    }
}

void propListener(	void *                  inClientData,
                  AudioSessionPropertyID	inID,
                  UInt32                  inDataSize,
                  const void *            inData)
{
    
    //    ViewController *THIS = (ViewController*)inClientData;
    if (inID == kAudioSessionProperty_AudioRouteChange)
    {
        CFDictionaryRef routeDictionary = (CFDictionaryRef)inData;
        //CFShow(routeDictionary);
        CFNumberRef reason = (CFNumberRef)CFDictionaryGetValue(routeDictionary, CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
        SInt32 reasonVal;
        CFNumberGetValue(reason, kCFNumberSInt32Type, &reasonVal);
        if (reasonVal != kAudioSessionRouteChangeReason_CategoryChange)
        {
            CFStringRef oldRoute = (CFStringRef)CFDictionaryGetValue(routeDictionary, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
            if (oldRoute)
            {
                printf("old route:\n");
                CFShow(oldRoute);
            }
            else
                printf("ERROR GETTING OLD AUDIO ROUTE!\n");
            
            CFStringRef newRoute;
            UInt32 size; size = sizeof(CFStringRef);
            OSStatus error = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &newRoute);
            if (error) printf("ERROR GETTING NEW AUDIO ROUTE! %d\n", error);
            else
            {
                printf("new route:\n");
                CFShow(newRoute);
            }
            //
            //            if (reasonVal == kAudioSessionRouteChangeReason_OldDeviceUnavailable)
            //            {
            //                if (THIS->player->IsRunning()) {
            //                    [THIS pausePlayQueue];
            //                    [[NSNotificationCenter defaultCenter] postNotificationName:@"playbackQueueStopped" object:THIS];
            //                }
            //            }
            //
            // stop the queue if we had a non-policy route change
            if (AQRecorder::getInstance()->IsRunning()) {
                AQRecorder::getInstance()->StopRecord();
            }
        }
    }
    //    else if (inID == kAudioSessionProperty_AudioInputAvailable)
    //    {
    //        if (inDataSize == sizeof(UInt32)) {
    //            UInt32 isAvailable = *(UInt32*)inData;
    //            // disable recording if input is not available
    //            THIS->btn_record.enabled = (isAvailable > 0) ? YES : NO;
    //        }
    //    }
}