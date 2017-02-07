//
//  ViewController.h
//  fm_main
//
//  Created by SnailResearch on 13-3-14.
//  Copyright (c) 2013年 SnailResearch. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface ViewController : GLKViewController //<UITextViewDelegate>

//- (void)textViewDidChange:(UITextView *)textView;

@end

@protocol UIKeyInput <UITextInputTraits>

//- (BOOL)hasText;
- (void)insertText:(NSString *)text;
- (void)deleteBackward;

@end