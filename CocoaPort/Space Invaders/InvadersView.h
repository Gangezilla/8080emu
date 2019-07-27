//
//  InvadersView.h
//  Space Invaders
//
//  Created by Scott Gangemi on 22/7/19.
//  Copyright © 2019 Scott Gangemi. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SpaceInvadersMachine.h"

@interface InvadersView : NSView
  {
    NSTimer *renderTimer;
    SpaceInvadersMachine * invaders; // NOT SURE

    CGContextRef bitmapCtx;
    unsigned char *buffer8888;
  }

-(void) timerFired:(id)sender;

@end
