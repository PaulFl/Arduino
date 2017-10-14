//
//  AppController.m
//  MacduinoScope
//
//  Created by Gabe Ghearing on 5/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"


@implementation AppController

// Register user defaults in the class initializer. (for most things 0/nil is fine)
+ (void) initialize {
	NSString *path = [[NSBundle mainBundle] pathForResource:@"userdefaults" ofType:@"plist"];
	NSDictionary *appDefaults=[NSDictionary dictionaryWithContentsOfFile:path];
	[[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

@end
