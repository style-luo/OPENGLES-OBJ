//
//  ShowViewController
//  showobj
//
//  Created by styleluo on 16/12/20.
//  Copyright © 2016年 styleluo. All rights reserved.
//


#import "ShowViewController.h"
#import "PhongShader.h"
#import "objload.h"
//#import <vector>
extern int loadObj(const char*  filepathOBJ,const char* filepathMTL,ObjModel & objmodel);
CGFloat distanceBetweenPoints (CGPoint first, CGPoint second) {
    CGFloat deltaX = second.x - first.x;
    CGFloat deltaY = second.y - first.y;
//    std::vector<int>v;
//    v.push_back(12);
    return sqrt(deltaX*deltaX + deltaY*deltaY );
};
@interface ShowViewController ()
{
    float   modelRotateX;
    float   modelRotateY;
    float   modelScale;
    bool  oneTouchBeginFlag;
    bool  twoTouchBeginFlag;
    bool  modelScaleOk;
    int  modelRotateDir;
    CGFloat twoTouchDis;
    ObjModel  model;
    CGPoint oneTouchPoint;
    NSString * modelName;
    GLuint modelBuffer[3];
    GLuint modelIndexBuffer[3];
    NSMutableArray * textures;
}

@property (strong, nonatomic) PhongShader* phongShader;

@end

@implementation ShowViewController

-(void)modelInit
{
    modelRotateX = modelRotateY  = 0.0f;
    modelScale = 1.5f;
    oneTouchBeginFlag = false;
    twoTouchBeginFlag = false;
    modelScaleOk = false;
    modelRotateDir = 0;
    twoTouchDis  = 0.0f;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    textures = [NSMutableArray arrayWithCapacity:0];
    //model Variables init
    modelName =@"zuozi";
    self.view.multipleTouchEnabled = TRUE;
    // Set up context
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    
    // Set up view
    GLKView* glkView = (GLKView *) self.view;
    glkView.context = context;
    
    // OpenGL ES Settings
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // Load shader
    [self loadShader];
    [self loadModel];
    [self modelInit];
}

- (void)loadShader
{
    self.phongShader = [[PhongShader alloc] init];
    glUseProgram(self.phongShader.program);
}



- (void)loadModel
{
    NSString* objPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat: @"%@.obj",modelName] ofType:nil];
    NSString* mtlPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat: @"%@.mtl",modelName] ofType:nil];
    loadObj([objPath cStringUsingEncoding:NSASCIIStringEncoding], [mtlPath cStringUsingEncoding:NSASCIIStringEncoding], model);
    [self modelInit ];
    [self deleteTexture];
    for(int i=0; i<model.materials; i++)
    {
        [self loadTexture:[NSString stringWithUTF8String:model.textures[i]]];
       // break;
    }
    // 设置顶点 纹理 法向量
//    glEnableVertexAttribArray(self.phongShader.aPosition);
//    glEnableVertexAttribArray(self.phongShader.aTexel);
//    glEnableVertexAttribArray(self.phongShader.aNormal);
//    glVertexAttribPointer(self.phongShader.aPosition, 3, GL_FLOAT, GL_FALSE, 0, model.positions);
//    glVertexAttribPointer(self.phongShader.aTexel, 2, GL_FLOAT, GL_FALSE, 0, model.texels);
//    glVertexAttribPointer(self.phongShader.aNormal, 3, GL_FLOAT, GL_FALSE, 0, model.normals);
    glDeleteBuffers(3,modelBuffer);
    glGenBuffers(3, modelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*3, model.positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(self.phongShader.aPosition);
    glVertexAttribPointer(self.phongShader.aPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*2, model.texels, GL_STATIC_DRAW);
    glEnableVertexAttribArray(self.phongShader.aTexel);
    glVertexAttribPointer(self.phongShader.aTexel, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*3, model.normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(self.phongShader.aNormal);
    glVertexAttribPointer(self.phongShader.aNormal, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    //index 版本
//    glDeleteBuffers(3,modelIndexBuffer);
//    glGenBuffers(3, modelIndexBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[0]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*3, model.positions, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(self.phongShader.aPosition);
//    glVertexAttribPointer(self.phongShader.aPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*2, model.texels, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(self.phongShader.aTexel);
//    glVertexAttribPointer(self.phongShader.aTexel, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer[2]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*model.vertices*3, model.normals, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(self.phongShader.aNormal);
//    glVertexAttribPointer(self.phongShader.aNormal, 3, GL_FLOAT, GL_FALSE, 0, NULL);


    
    
}

- (void)setMatrices
{
    // Projection Matrix
    const GLfloat aspectRatio = (GLfloat)(self.view.bounds.size.width) / (GLfloat)(self.view.bounds.size.height);
    const GLfloat fieldView = GLKMathDegreesToRadians(90.0f);
    const GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(fieldView, aspectRatio, 0.1f, 10.0f);
    glUniformMatrix4fv(self.phongShader.uProjectionMatrix, 1, 0, projectionMatrix.m);
    
    // ModelView Matrix
    GLKMatrix4 modelViewMatrix = GLKMatrix4Identity;
    modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, 0.0f, 0.0f, -2.5f);
    modelViewMatrix = GLKMatrix4RotateX(modelViewMatrix, GLKMathDegreesToRadians(modelRotateY));
    modelViewMatrix = GLKMatrix4RotateY(modelViewMatrix, GLKMathDegreesToRadians(modelRotateX));
    modelViewMatrix = GLKMatrix4RotateZ(modelViewMatrix, GLKMathDegreesToRadians(0));
    
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, modelScale, modelScale, modelScale);
    
    glUniformMatrix4fv(self.phongShader.uModelViewMatrix, 1, 0, modelViewMatrix.m);
    
    // Normal Matrix
    // Transform normals from object-space to eye-space
    bool invertible;
    GLKMatrix3 normalMatrix = GLKMatrix4GetMatrix3(GLKMatrix4InvertAndTranspose(modelViewMatrix, &invertible));
    if(!invertible)
        NSLog(@"MV matrix is not invertible");
    glUniformMatrix3fv(self.phongShader.uNormalMatrix, 1, 0, normalMatrix.m);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set matrices
    [self setMatrices];
    
    // Render model
    [self renderModel];
}

- (void)update
{
}
- (void)loadTexture:(NSString*) textureName
{
    if([textureName isEqualToString:@""])
    {
        [textures addObject:[NSNull null]];
        return;
    }
    NSDictionary* options = @{GLKTextureLoaderOriginBottomLeft: @YES};
    NSError* error;
    NSString* path = [[NSBundle mainBundle] pathForResource:textureName ofType:nil];
    NSLog(@"GL Error = %u", glGetError());
    GLKTextureInfo * texture = [GLKTextureLoader textureWithContentsOfFile:path options:options error:&error];
    
    if(texture == nil)
        NSLog(@"Error loading file: %@", [error localizedDescription]);
    [textures addObject:texture];
    
}
- (void)setTexture:(int) idx
{
    if([textures count]<=idx) return ;
    if([textures objectAtIndex:idx] == [NSNull null])
    {
        glDisable(GL_TEXTURE_2D);
        glUniform1i(self.phongShader.hasTexture,false);
        return ;
    }
    glUniform1i(self.phongShader.hasTexture,true);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ((GLKTextureInfo* )[textures objectAtIndex:idx]).name);
    glUniform1i(self.phongShader.uTexture,0);
}
- (void)deleteTexture
{
    for(int i=0;i<[textures count];i++)
    {
        if([textures objectAtIndex:i] ==  [NSNull null])
            continue;
        GLuint name = ((GLKTextureInfo* )[textures objectAtIndex:i]).name;
        glDeleteTextures(1, &name);
    }
    [textures removeAllObjects];
}
- (void)renderModel
{
    int idx =0;
    for(int i=0; i<model.materials; i++)
    {
        [self setTexture:idx];
        idx++;
        glUniform3f(self.phongShader.uDiffuse, model.diffuses[i][0], model.diffuses[i][1], model.diffuses[i][2]);
        glUniform3f(self.phongShader.uSpecular, model.speculars[i][0], model.speculars[i][1], model.speculars[i][2]);
        glDrawArrays(GL_TRIANGLES, model.firsts[i], model.counts[i]);
        
    }
}


- (IBAction)selectModel:(UISegmentedControl *)sender
{
    self.paused = YES;
    int m = (int)sender.selectedSegmentIndex;
    switch(m)
    {
        case 0:
            modelName = @"zuozi";
            break;
            
        case 1:
            modelName = @"chuang";
            break;
    }
    self.paused = NO;
    [self loadModel ];
}
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event //首次在屏幕上检测到触摸时调用
{
   // NSLog(@"touchesBegan");
    NSArray* touchArray = [[event allTouches] allObjects];
    if(touchArray.count==1)
    {
        oneTouchBeginFlag = true;
        UITouch *touch = touchArray[0];
        oneTouchPoint = [touch locationInView:self.view];
    }
    else if(touchArray.count == 2)
    {
        twoTouchBeginFlag = true;
        UITouch *touch1 = touchArray[0];
        CGPoint point1 = [touch1 locationInView:self.view];
        UITouch *touch2 = touchArray[1];
        CGPoint point2 = [touch2 locationInView:self.view];
        twoTouchDis =distanceBetweenPoints(point1,point2);
    }
}


-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event //如果触摸移动到了新的位置则会调用此方法
{
   // NSLog(@"touchesMoved");
    NSArray* touchArray = [[event allTouches] allObjects];
    CGFloat gap =0.2;
    CGFloat rotateSpeed =4;
    if(touchArray.count==1&&oneTouchBeginFlag==true)
    {
        oneTouchBeginFlag = true;
        UITouch *touch = touchArray[0];
        CGPoint touchPoint = [touch locationInView:self.view];
        if((fabs(touchPoint.x- oneTouchPoint.x)>10&&modelRotateDir==0)||(fabs(touchPoint.x- oneTouchPoint.x)>gap&&modelRotateDir == -1))
        {
            modelRotateX+= touchPoint.x- oneTouchPoint.x>0?rotateSpeed:-rotateSpeed;
            modelRotateDir =-1;
            oneTouchPoint = touchPoint;
        }
        if((fabs(touchPoint.y- oneTouchPoint.y)>10&&modelRotateDir==0)||(fabs(touchPoint.y- oneTouchPoint.y)>gap&&modelRotateDir == 1))
        {
            modelRotateY+= touchPoint.y- oneTouchPoint.y>0?rotateSpeed:-rotateSpeed;
            if(modelRotateY>=90)modelRotateY =90;
            if(modelRotateY<=-90) modelRotateY = -90;
            modelRotateDir =1;
            oneTouchPoint = touchPoint;
        }
        
    }
    else if(touchArray.count == 2 && twoTouchBeginFlag == true)
    {
        twoTouchBeginFlag = true;
        UITouch *touch1 = touchArray[0];
        CGPoint point1 = [touch1 locationInView:self.view];
        UITouch *touch2 = touchArray[1];
        CGPoint point2 = [touch2 locationInView:self.view];
        CGFloat tmptwoTouchDis =distanceBetweenPoints(point1,point2);
        if(fabs(tmptwoTouchDis- twoTouchDis)>10||(fabs(tmptwoTouchDis- twoTouchDis)>gap&&modelScaleOk == true))
        {
            modelScaleOk  = true;
            modelScale *=(tmptwoTouchDis- twoTouchDis >0)?1.05:0.94;
            if(modelScale>5.0f)
                modelScale = 5.0f;
            if(modelScale<0.3f)
                modelScale = 0.3f;
            twoTouchDis = tmptwoTouchDis;
        }
    }
    
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event//当触摸离开屏幕调用此方法
{
    modelScaleOk = false;
    modelRotateDir = 0;
    oneTouchBeginFlag = false;
    twoTouchBeginFlag = false;
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event//如系统决定取消
{
    modelScaleOk = false;
    modelRotateDir = 0;
    oneTouchBeginFlag = false;
    twoTouchBeginFlag = false;
    
}
@end

