//
//  CSGLViewController.mm
//  Chilli Source
//  Created by Scott Downie on 09/05/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#ifdef CS_TARGETPLATFORM_IOS

#import <CSBackend/Platform/iOS/Core/Base/CSGLViewController.h>

#import <CSBackend/Platform/iOS/Core/Base/Screen.h>
#import <CSBackend/Platform/iOS/Core/String/NSStringUtils.h>
#import <CSBackend/Platform/iOS/Input/Pointer/PointerSystem.h>
#import <ChilliSource/Core/Base/AppConfig.h>
#import <ChilliSource/Core/Base/Application.h>
#import <ChilliSource/Core/Base/Utils.h>
#import <ChilliSource/Core/String/StringParser.h>
#import <ChilliSource/Rendering/Base/SurfaceFormat.h>

#import <json/json.h>

@implementation CSGLViewController

//-------------------------------------------------------------
//-------------------------------------------------------------
- (id)initWithDelegate:(id)in_delegate
{
    if((self = [super init]))
	{
        EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        BOOL isContextSet = [EAGLContext setCurrentContext:context];
        
        if(context == nil || isContextSet == NO)
        {
            CS_LOG_FATAL("Cannot Create OpenGL ES 2.0 Context");
        }
        
        //TODO: Expose colour and depth format
        GLKView* view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:context];
        [self applySurfaceFormat:view];
        view.drawableMultisample = GLKViewDrawableMultisampleNone;
        view.userInteractionEnabled = YES;
        view.enableSetNeedsDisplay = NO;
        view.delegate = in_delegate;
        
        self.view = view;
        self.delegate = in_delegate;
        
        //Fetch the supported orientations from the plist
        NSMutableSet* supportedOrientationTypes = [[NSMutableSet alloc] initWithCapacity:4];
        supportedOrientations = supportedOrientationTypes;
        
        NSArray* supportedOrientationStrings = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"UISupportedInterfaceOrientations"];
        if(supportedOrientationStrings != nil)
        {
            for(NSString* string in supportedOrientationStrings)
            {
                if([string isEqualToString:@"UIInterfaceOrientationLandscapeLeft"])
                {
                    [supportedOrientationTypes addObject:[NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft]];
                }
                else if([string isEqualToString:@"UIInterfaceOrientationLandscapeRight"])
                {
                    [supportedOrientationTypes addObject:[NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight]];
                }
                else if([string isEqualToString:@"UIInterfaceOrientationPortrait"])
                {
                    [supportedOrientationTypes addObject:[NSNumber numberWithInt:UIInterfaceOrientationPortrait]];
                }
                else if([string isEqualToString:@"UIInterfaceOrientationPortraitUpsideDown"])
                {
                    [supportedOrientationTypes addObject:[NSNumber numberWithInt:UIInterfaceOrientationPortraitUpsideDown]];
                }
            }
        }
        
        return self;
    }

    return nil;
}
//-------------------------------------------------------------
/// Reads the surface format from the App.config file.
///
/// @author I Copland
///
/// @return The surface format.
//-------------------------------------------------------------
 - (CSRendering::SurfaceFormat)readSurfaceFormat
{
    //load the JSON string from file.
    NSString* relativePath = [NSStringUtils newNSStringWithUTF8String:"Shared/App"];
    NSString* fullPath = [[NSBundle mainBundle] pathForResource:relativePath ofType:@"config"];
    [relativePath release];
    NSString* content = [NSString stringWithContentsOfFile:fullPath encoding:NSUTF8StringEncoding error:nil];
    std::string jsonString = [NSStringUtils newUTF8StringWithNSString:content];
    
    //parse the json
    Json::Reader jReader;
    Json::Value root;
    if(!jReader.parse(jsonString, root))
    {
        CS_LOG_FATAL("Could not parse App.config: " + jReader.getFormatedErrorMessages());
    }
   
    return CSCore::ParseSurfaceFormat(root.get("PreferredSurfaceFormat", "rgb565_depth24").asString());
}
//-------------------------------------------------------------
/// Applies the surface format described in the App.config file
/// to the given view.
///
/// @author I Copland
///
/// @param The view to apply the format to.
//-------------------------------------------------------------
- (void)applySurfaceFormat:(GLKView*)in_view
{
    CSRendering::SurfaceFormat preferredFormat = [self readSurfaceFormat];
    
    //apply format
    switch (preferredFormat)
    {
        case CSRendering::SurfaceFormat::k_rgb565_depth24:
        default:
            in_view.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            in_view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            in_view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
        case CSRendering::SurfaceFormat::k_rgb565_depth32:
            in_view.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
            in_view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            in_view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
        case CSRendering::SurfaceFormat::k_rgb888_depth24:
            in_view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
            in_view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            in_view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
        case CSRendering::SurfaceFormat::k_rgb888_depth32:
            in_view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
            in_view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
            in_view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
            break;
    }
}
//-------------------------------------------------------------
/// Called by the OS to query whether the view should be
/// allowed to rotate to the given orientation. The return
/// is based on the values in the plist.
///
/// @author S Downie
///
/// @param Interface orientation to rotate to
///
/// @return Whether we support the given orientation
//-------------------------------------------------------------
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)in_interfaceOrientation
{
	return [supportedOrientations containsObject:[NSNumber numberWithInt:in_interfaceOrientation]];
}
//-------------------------------------------------------------
/// Called by the OS immediately before a view rotation
/// occurs. This allows use to notify the engine of the orientation
/// change
///
/// @author S Downie
///
/// @param New orientation
/// @param Time in seconds of orientation animation
//-------------------------------------------------------------
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)in_toInterfaceOrientation duration:(NSTimeInterval)in_duration
{
    if(CSCore::Application::Get() != nullptr)
    {
        CSBackend::iOS::Screen* screen = CSCore::Application::Get()->GetSystem<CSBackend::iOS::Screen>();
        screen->OnOrientationChanged(in_toInterfaceOrientation);
    }
}
//-------------------------------------------------------------
/// Called by the OS when one or more touches start on the view
///
/// @author S Downie
///
/// @param Set of touches
/// @param Event
//-------------------------------------------------------------
- (void)touchesBegan:(NSSet*)in_touches withEvent:(UIEvent*)in_event
{
    CSBackend::iOS::PointerSystem* pointerSystem = CSCore::Application::Get()->GetSystem<CSBackend::iOS::PointerSystem>();
    CS_ASSERT(pointerSystem, "touchesBegan callback requires the pointer system.");
    
    for (UITouch* touch in in_touches)
    {
        pointerSystem->OnTouchBegan(touch);
    }
}
//-------------------------------------------------------------
/// Called by the OS when one or more touches are cancelled
///
/// @author S Downie
///
/// @param Set of touches
/// @param Event
//-------------------------------------------------------------
- (void)touchesCancelled:(NSSet*)in_touches withEvent:(UIEvent*)in_event
{
    CSBackend::iOS::PointerSystem* pointerSystem = CSCore::Application::Get()->GetSystem<CSBackend::iOS::PointerSystem>();
    CS_ASSERT(pointerSystem, "touchesCancelled callback requires the pointer system.");
    
    for (UITouch* touch in in_touches)
    {
        pointerSystem->OnTouchEnded(touch);
    }
}
//-------------------------------------------------------------
/// Called by the OS when one or more touches end on the view
///
/// @author S Downie
///
/// @param Set of touches
/// @param Event
//-------------------------------------------------------------
- (void)touchesEnded:(NSSet*)in_touches withEvent:(UIEvent*)in_event
{
    CSBackend::iOS::PointerSystem* pointerSystem = CSCore::Application::Get()->GetSystem<CSBackend::iOS::PointerSystem>();
    CS_ASSERT(pointerSystem, "touchesEnded callback requires the pointer system.");
    
	for (UITouch* touch in in_touches)
    {
        pointerSystem->OnTouchEnded(touch);
    }
}
//-------------------------------------------------------------
/// Called by the OS when one or more touches that have been
/// started are then moved
///
/// @author S Downie
///
/// @param Set of touches
/// @param Event
//-------------------------------------------------------------
- (void)touchesMoved:(NSSet*)in_touches withEvent:(UIEvent*)in_event
{
    CSBackend::iOS::PointerSystem* pointerSystem = CSCore::Application::Get()->GetSystem<CSBackend::iOS::PointerSystem>();
    CS_ASSERT(pointerSystem, "touchesEnded callback requires the pointer system.");
    
    for (UITouch* touch in in_touches)
    {
        pointerSystem->OnTouchMoved(touch);
    }
}
//-------------------------------------------------------------
/// Dealloc
///
/// @author S Downie
//-------------------------------------------------------------
- (void) dealloc
{
    GLKView* view = (GLKView*)self.view;
    [view.context release];
    view.context = nil;
    [EAGLContext setCurrentContext:nil];
    [view release];
    
    [supportedOrientations release];
    
    [super dealloc];
}

@end

#endif
