//
//  MyInputView.m
//  InputTest
//
//  Created by leo on 13-5-24.
//  Copyright (c) 2013年 leo. All rights reserved.
//

#import "MyInputView.h"

@implementation MyInputView
@synthesize InputDelegate;
@synthesize InsertTextCallback;
@synthesize DeleteBackward;

- (id)initWithFrame:(CGRect)frame
{
    
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
   // self.returnKeyType=UIReturnKeyGo;
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

#pragma mark -
#pragma mark UIKeyInput Protocol Methods

- (BOOL)hasText {
    return NO;
}

- (void)insertText:(NSString *)theText {
    NSLog(@"insertText %@ ",theText);
    if (mMarkText != nil) {
        mMarkText = nil ;
    }
    
    if (InsertTextCallback) {
        (*InsertTextCallback)([theText cStringUsingEncoding:NSUTF8StringEncoding]);
    }

    
}

- (void)deleteBackward {
    if (mMarkText != nil) {
        mMarkText = nil ;
    }
    if (DeleteBackward) {
        (*DeleteBackward)();
    }
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}


#pragma mark -
#pragma mark UITextInput Protocol Methods

- (NSString *)textInRange:(UITextRange *)range {
    return @"";
}
- (void)replaceRange:(UITextRange *)range withText:(NSString *)text {
}
- (void) setSelectedTextRange: (UITextRange *) range {
}
- (UITextRange *) markedTextRange {
    return nil;
}
- (NSDictionary *) markedTextStyle {
    return nil;
}
- (void) setMarkedTextStyle: (NSDictionary *) style {
}
- (void)setMarkedText:(NSString *)markedText selectedRange:(NSRange)selectedRange {
    if (markedText == mMarkText) {
        return;
    }
    if (nil != mMarkText)
    {
        mMarkText = nil;
    }
    mMarkText = markedText;
    
}
- (void) unmarkText {
    if (mMarkText == nil) {
        return;
    }
    if (InsertTextCallback) {
        (*InsertTextCallback)([mMarkText cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    mMarkText = nil;
}
- (UITextPosition *) endOfDocument {
    //DOWN KEY
    
    NSLog(@"Down");
    return nil;
}
- (UITextPosition *) beginningOfDocument {
    //UP KEY
    
    NSLog(@"UP");
    return nil;
}
- (UITextRange *)textRangeFromPosition:(UITextPosition *)fromPosition toPosition:(UITextPosition *)toPosition{
    return nil;
}
- (UITextPosition *)positionFromPosition:(UITextPosition *)position offset:(NSInteger)offset{
    return nil;
}
- (UITextPosition *)positionFromPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset {
    return nil;
}
- (NSComparisonResult) comparePosition: (UITextPosition *)position toPosition: (UITextPosition *)other {
    return NSOrderedSame;
}
- (NSInteger) offsetFromPosition: (UITextPosition *)from toPosition: (UITextPosition *)toPosition {
    return 0;
}
//- (void) setInputDelegate: (id <UITextInputDelegate>) delegate {}

- (id <UITextInputDelegate>) inputDelegate {
    return self.InputDelegate;
}
- (id <UITextInputTokenizer>) tokenizer {
    return nil;
}
- (UITextPosition *)positionWithinRange:(UITextRange *)range farthestInDirection:(UITextLayoutDirection)direction {
    return nil;
}
- (UITextRange *) characterRangeByExtendingPosition: (UITextPosition *) position inDirection: (UITextLayoutDirection) direction {
    return nil;
}
- (UITextWritingDirection) baseWritingDirectionForPosition: (UITextPosition *)position inDirection: (UITextStorageDirection)direction {
    return 0;
}
- (void) setBaseWritingDirection: (UITextWritingDirection)writingDirection forRange:(UITextRange *)range {
}
- (CGRect) firstRectForRange: (UITextRange *) range {
    return CGRectZero;
}
- (CGRect) caretRectForPosition: (UITextPosition *) position  {
    return CGRectZero;
}
- (UITextPosition *) closestPositionToPoint: (CGPoint)point {
    return nil;
}
- (UITextPosition *) closestPositionToPoint: (CGPoint)point withinRange: (UITextRange *) range {
    return nil;
}
- (UITextRange *) characterRangeAtPoint: (CGPoint)point {
    return nil;
}
- (UITextRange *) selectedTextRange {
    return [[UITextRange alloc]init];
}

@end
