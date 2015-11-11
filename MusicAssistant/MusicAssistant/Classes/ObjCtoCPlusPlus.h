//
//  ObjCtoCPlusPlus.h
//  Performance_Console
//
//  Created by Gian Luigi Romita on 12/06/14.
//  Copyright (c) 2014 Gian Luigi Romita. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AQRecorderObjC : NSObject
+ (void) initRecorder;
+ (void) startRecord;
+ (void) stopRecord;
@end