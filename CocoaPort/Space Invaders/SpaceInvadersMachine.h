//
//  SpaceInvadersMachine.h
//  Space Invaders
//
//  Created by Scott Gangemi on 22/7/19.
//  Copyright © 2019 Scott Gangemi. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "8080emu.h"

@interface SpaceInvadersMachine : NSObject
{
    State8080 *state;
    double lastTimer;
    double nextInterrupt;
    int whichInterrupt;
    
    NSTimer *emulatorTimer;
    
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shift_offset;
}

-(double) timeusec;
-(void) ReadFile:(NSString*)filename IntoMemoryAt:(uint32_t)memoffset;
-(id) init;

-(void) doCPU;
-(void) startEmulation;

-(void *) framebuffer;

@end


