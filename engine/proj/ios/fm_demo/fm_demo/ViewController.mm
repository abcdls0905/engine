//
//  ViewController.m
//  fm_main
//
//  Created by SnailResearch on 13-3-14.
//  Copyright (c) 2013年 SnailResearch. All rights reserved.
//
//#define DRAW_CUBE
#import "ViewController.h"
#include "app.h"
#include "i_input.h"

extern ICore* g_pCorePtr;

IInputManager* g_inputmgr;

int   g_fullscreen_w;
int   g_fullscreen_h;


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//#define DRAW_CUBE
// Uniform index.
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

const char* GetRootPath()
{
    NSString * bundlPath = [[NSBundle mainBundle] bundlePath];
    static char gamePath[1024];
    strcpy(gamePath, [bundlPath UTF8String]);
    strcat(gamePath, "/bin/");
    return gamePath;
}

void showKeyBoard();
void hideKeyBoard();

GLfloat gCubeVertexData[216] =
{
    // Data layout for each line below is:
    // positionX, positionY, positionZ,     normalX, normalY, normalZ,
    0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,          1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    
    0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,
    
    -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,
    
    -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,
    
    0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    
    0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f
};


ViewController * sta_Instance_ViewController;


@interface ViewController () {
    GLuint _program;
    
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix3 _normalMatrix;
    float _rotation;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;


- (void) showIME;

- (void) hideIME;


- (void)setupGL;
- (void)tearDownGL;

- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation ViewController



- (void)viewDidLoad
{
    [super viewDidLoad];
    
    sta_Instance_ViewController = self;
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    UITextView * input_ = [[UITextView alloc]initWithFrame:self.view.frame];
    [input_ setDelegate:self];
    
    [input_ setTag:2013];
    [input_ setAlpha:0.0];
    [self.view addSubview:input_];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(shouldChangeCharactersInRange:) name:UIKeyboardDidChangeFrameNotification object:nil];
    
    
    
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.multipleTouchEnabled = YES;
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    [self loadShaders];
    
    self.effect = [[GLKBaseEffect alloc] init];
    self.effect.light0.enabled = GL_TRUE;
    self.effect.light0.diffuseColor = GLKVector4Make(1.0f, 0.4f, 0.4f, 1.0f);
#ifdef DRAW_CUBE
    glEnable(GL_DEPTH_TEST);
    
    glGenVertexArraysOES(1, &_vertexArray);
    glBindVertexArrayOES(_vertexArray);
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(GLKVertexAttribNormal);
    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
#else
    CVarList arg;
    int w = [UIScreen mainScreen].bounds.size.width* [UIScreen mainScreen].scale;
    int h = [UIScreen mainScreen].bounds.size.height * [UIScreen mainScreen].scale;
    App::Inst().Init(GetRootPath(), arg, h, w,1.0);
    
    g_fullscreen_w = w>h?w:h;
    g_fullscreen_h = w<=h?w:h;
    
    g_inputmgr = (IInputManager*)g_pCorePtr->GetInterface("CInputManager");
#endif
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    App::Inst().Shutdown();
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteVertexArraysOES(1, &_vertexArray);
    
    self.effect = nil;
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
#ifdef DRAW_CUBE
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 100.0f);
    
    self.effect.transform.projectionMatrix = projectionMatrix;
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -4.0f);
    baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, _rotation, 0.0f, 1.0f, 0.0f);
    
    // Compute the model view matrix for the object rendered with GLKit
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    self.effect.transform.modelviewMatrix = modelViewMatrix;
    
    // Compute the model view matrix for the object rendered with ES2
    modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 1.5f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    _normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
    _modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    
    _rotation += self.timeSinceLastUpdate * 0.5f;
#else
    App::Inst().OnFrame();
#endif
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
#ifdef DRAW_CUBE
    glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glBindVertexArrayOES(_vertexArray);
  
    // Render the object with GLKit
    [self.effect prepareToDraw];

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Render the object again with ES2
    glUseProgram(_program);
    
    glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
#else
    App::Inst().OnRender();
#endif
}

#pragma mark -  OpenGL ES 2 shader compilation

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    _program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname]) {
        NSLog(@"Failed to compile vertex shader");
        return NO;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname]) {
        NSLog(@"Failed to compile fragment shader");
        return NO;
    }
    
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(_program, GLKVertexAttribPosition, "position");
    glBindAttribLocation(_program, GLKVertexAttribNormal, "normal");
    
    // Link program.
    if (![self linkProgram:_program]) {
        NSLog(@"Failed to link program: %d", _program);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_program) {
            glDeleteProgram(_program);
            _program = 0;
        }
        
        return NO;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(_program, "modelViewProjectionMatrix");
    uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_program, "normalMatrix");
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(_program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(_program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return YES;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source) {
        NSLog(@"Failed to load vertex shader");
        return NO;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return NO;
    }
    
    return YES;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    int touchnum = [touches count] ;
    FmVec2 vec2TouchData[10];
    int nindex = 0;
    for(UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        CGPoint prepoint = [touch previousLocationInView:self.view];
        if(nindex < 10 )
        {
            point.x *= [UIScreen mainScreen].scale;
            point.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(point.x/g_fullscreen_w,point.y/g_fullscreen_h);
         
            prepoint.x *= [UIScreen mainScreen].scale;
            prepoint.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(prepoint.x/g_fullscreen_w,prepoint.y/g_fullscreen_h);
        }
    }
    
    ITouchEvent* pTouchEvent = g_inputmgr->CreateTouchEvent(PERSISTID(),NULL, Touch_Began,vec2TouchData,touchnum);
    g_inputmgr->SendInputEvent(pTouchEvent);
    g_inputmgr->DeleteTouchEvent(pTouchEvent);
    
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    int touchnum = [touches count] ;
    FmVec2 vec2TouchData[10];
    int nindex = 0;
    for(UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        CGPoint prepoint = [touch previousLocationInView:self.view];
        if(nindex < 10 )
        {
            point.x *= [UIScreen mainScreen].scale;
            point.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(point.x/g_fullscreen_w,point.y/g_fullscreen_h);
            
            prepoint.x *= [UIScreen mainScreen].scale;
            prepoint.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(prepoint.x/g_fullscreen_w,prepoint.y/g_fullscreen_h);
        }
    }
    
    ITouchEvent* pTouchEvent = g_inputmgr->CreateTouchEvent(PERSISTID(),NULL, Touch_Cancel,vec2TouchData,touchnum);
    g_inputmgr->SendInputEvent(pTouchEvent);
    g_inputmgr->DeleteTouchEvent(pTouchEvent);
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    int touchnum = [touches count] ;
    FmVec2 vec2TouchData[10];
    int nindex = 0;
    for(UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        CGPoint prepoint = [touch previousLocationInView:self.view];
        if(nindex < 10 )
        {
            point.x *= [UIScreen mainScreen].scale;
            point.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(point.x/g_fullscreen_w,point.y/g_fullscreen_h);
            
            prepoint.x *= [UIScreen mainScreen].scale;
            prepoint.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(prepoint.x/g_fullscreen_w,prepoint.y/g_fullscreen_h);
        }
    }
    
    ITouchEvent* pTouchEvent = g_inputmgr->CreateTouchEvent(PERSISTID(),NULL, Touch_Ended,vec2TouchData,touchnum);
    g_inputmgr->SendInputEvent(pTouchEvent);
    g_inputmgr->DeleteTouchEvent(pTouchEvent);
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    int touchnum = [touches count] ;
    FmVec2 vec2TouchData[10];
    int nindex = 0;
    for(UITouch *touch in touches)
    {
        CGPoint point = [touch locationInView:self.view];
        CGPoint prepoint = [touch previousLocationInView:self.view];
        if(nindex < 10 )
        {
            point.x *= [UIScreen mainScreen].scale;
            point.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(point.x/g_fullscreen_w,point.y/g_fullscreen_h);
            
            prepoint.x *= [UIScreen mainScreen].scale;
            prepoint.y *= [UIScreen mainScreen].scale;
            vec2TouchData[nindex++] =
            FmVec2(prepoint.x/g_fullscreen_w,prepoint.y/g_fullscreen_h);
        }
    }
    
    ITouchEvent* pTouchEvent = g_inputmgr->CreateTouchEvent(PERSISTID(),NULL, Touch_Moved,vec2TouchData,touchnum);
    g_inputmgr->SendInputEvent(pTouchEvent);
    g_inputmgr->DeleteTouchEvent(pTouchEvent);
}

- (void) showIME
{
    //打开键盘
    [[self.view viewWithTag:2013] becomeFirstResponder];
}

- (void) hideIME
{
    //打开键盘
    [[self.view viewWithTag:2013] resignFirstResponder];
}

//键盘显示回调
- (void)keyboardWillShow:(NSNotification *)notification {
    
    /*
     Reduce the size of the text view so that it's not obscured by the keyboard.
     Animate the resize so that it's in sync with the appearance of the keyboard.
     */
    
    NSDictionary *userInfo = [notification userInfo];
    
    // Get the origin of the keyboard when it's displayed.
    NSValue* aValue = [userInfo objectForKey:UIKeyboardFrameEndUserInfoKey];
    
    // Get the top of the keyboard as the y coordinate of its origin in self's view's coordinate system. The bottom of the text view's frame should align with the top of the keyboard's final position.
    //键盘大小，需要根据这个来移动界面
    CGRect keyboardRect = [aValue CGRectValue];
    // Animate the resize of the text view's frame in sync with the keyboard's appearance.
    //[self moveInputBarWithKeyboardHeight:keyboardRect.size.height withDuration:animationDuration];
}


//键盘消失回调
- (void)keyboardWillHide:(NSNotification *)notification {
    

    
    //[self moveInputBarWithKeyboardHeight:0.0 withDuration:animationDuration];
}

// 输入回调
/*
- (void)textViewDidChange:(UITextView *)textView
{
    const char * cszText = [[textView text] cStringUsingEncoding:NSUTF8StringEncoding];
    printf("input text = > %s",cszText);
    
    int result;
    for(int i = 0; cszText[i]!=0; i++)
        App::Inst().OnMsg(WM_CHAR,   cszText[i], 0, result);
}*/

- (void)insertText:(NSString *)text
{
    const char * pszText = [text cStringUsingEncoding:NSUTF8StringEncoding];
    int n = 3;
}

- (void)deleteBackward
{
    int n = 3;
}
@end

void showKeyBoard()
{
    [sta_Instance_ViewController showIME];
}

void hideKeyBoard()
{
    [sta_Instance_ViewController hideIME];
}
