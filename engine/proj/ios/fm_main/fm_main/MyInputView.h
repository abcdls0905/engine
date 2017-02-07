//
//  MyInputView.h
//  InputTest
//
//  Created by leo on 13-5-24.
//  Copyright (c) 2013年 leo. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef void(* ONINSERTTEXT)(const char *);
typedef void(* ONDELETEBACKWARD)(void);

@interface MyInputView : UIView <UITextInput,UIKeyInput>
{
    NSString * mMarkText;
}

@property(nonatomic,retain) id <UITextInputDelegate>  InputDelegate;
@property(nonatomic,assign) ONINSERTTEXT InsertTextCallback;
@property(nonatomic,assign) ONDELETEBACKWARD DeleteBackward;

@end
