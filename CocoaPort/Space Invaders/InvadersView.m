//
//  InvadersView.m
//  Space Invaders
//
//  Created by Scott Gangemi on 22/7/19.
//  Copyright © 2019 Scott Gangemi. All rights reserved.
//

#import "InvadersView.h"

#define RGB_ON  0xFFFFFFFFL
#define RGB_OFF 0x00000000L

@implementation InvadersView

- (void) awakeFromNib
{
    invaders = [[SpaceInvadersMachine alloc] init];
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    buffer8888 = malloc(4 * 224 * 256);
    bitmapCtx = CGBitmapContextCreate(buffer8888, 224, 256, 8, 224*4, colorSpace, kCGImageAlphaNoneSkipFirst);

    // a 16ms time interval to get 60fps
    renderTimer =   [NSTimer timerWithTimeInterval: 0.016
                    target: self
                    selector: @selector(timerFired:)
                    userInfo: nil
                    repeats: YES
                    ];
    
    [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
    [invaders startEmulation];
}

- (void)drawRect:(NSRect)dirtyRect {
    int i, j;

    // Translate the 1-bit space invaders frame buffer into the 32bpp RGB bitmap. 
    // We have to rotate and flip the image as we go.
    unsigned char *b = (unsigned char *)buffer8888;
    unsigned char *fb = [invaders framebuffer];
    for (i = 0; i < 224; i ++)
    {
        for (j = 0; j < 256; j+= 8)
        {
            int p;
            // Read the first 1 bit pixel, divide by 8 because there are 8 pixels in a byte
            unsigned char pix = fb[(i * (256/8)) + j/8];

            // Gives us 8 output vertical pixels, we need to do a vertical flip
            // so j starts at the last line, and advances backwards through the buffer.
            int offset = (255 - j) * (224 * 4) + (i * 4);
            unsigned int * p1 = (unsigned int*)(&b[offset]);
            for (p = 0; p < 8; p ++)
            {
                if (0 != (pix & (1<<p)))
                {
                    *p1 = RGB_ON;
                }
                else {
                    *p1 = RGB_OFF;
                }
                p1 -= 224;
            }
        }
    }

    CGContextRef myContext = [[NSGraphicsContext currentContext] graphicsPort];
    CGImageRef ir = CGBitmapContextCreateImage(bitmapCtx);
    CGContextDrawImage(myContext, self.bounds, ir);
    CGImageRelease(ir);
}

- (void) timerFired: (id) sender
{
    [self setNeedsDisplay: YES];
}

@end
