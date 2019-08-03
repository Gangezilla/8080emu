//
//  SpaceInvadersMachine.h
//  Space Invaders
//
//  Created by Scott Gangemi on 22/7/19.
//  Copyright © 2019 Scott Gangemi. All rights reserved.
//

#include "8080emu.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#define KEY_COIN 'c'
#define KEY_P1_LEFT 'a'
#define KEY_P1_RIGHT 's'
#define KEY_P1_FIRE ' '
#define KEY_P1_START '1'
#define KEY_PAUSE 'p'

@interface SpaceInvadersMachine : NSObject {
  State8080 *state;
  double lastTimer;
  double nextInterrupt;
  int whichInterrupt;

  NSTimer *emulatorTimer;

  uint8_t shift0;
  uint8_t shift1;
  uint8_t shift_offset;

  uint8_t in_port1;
  BOOL paused;

  uint8_t out_port3, out_port5, last_out_port3, last_out_port5;

  NSSound *ufo;
}

- (double)timeusec;
- (void)ReadFile:(NSString *)filename IntoMemoryAt:(uint32_t)memoffset;
- (id)init;

- (void)doCPU;
- (void)startEmulation;

- (void *)framebuffer;

- (void)KeyDown:(uint8_t)key;
- (void)KeyUp:(uint8_t)key;

@end
