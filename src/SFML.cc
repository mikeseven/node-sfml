#include "common.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cstdio>

using namespace v8;

#include <iostream>
using namespace std;

namespace sfml {

#define INIT(T) \
  T##Template = Persistent<FunctionTemplate>::New(FunctionTemplate::New(T));\
  Handle<ObjectTemplate> T##TemplateInstanceTemplate = T##Template->InstanceTemplate();\
  T##TemplateInstanceTemplate->SetInternalFieldCount(1);\
  target->Set(JS_STR( #T ), T##Template->GetFunction());

template<typename T>
void Dispose(Persistent<Value> object, void* parameter) {
  cout << "Disposing object " << parameter << endl;
  ((T*) External::Unwrap(object->ToObject()->GetInternalField(0)))->T::~T();
}

///////////////////////////////////////////////////////////////////////////////
//
// VideoMode
//
///////////////////////////////////////////////////////////////////////////////

Persistent<FunctionTemplate> VideoModeTemplate;
JS_METHOD(VideoMode) {
  HandleScope scope;
  if (args.IsConstructCall()) {
    sf::VideoMode* video_mode;
    Handle<Object> vm = args.This();
    if (args.Length() == 0) {
      video_mode = new sf::VideoMode();
    } else {
      int bits_per_pixel = args[2]->IsUndefined() ? 32 : args[2]->IntegerValue();
      video_mode = new sf::VideoMode(args[0]->IntegerValue(), args[1]->IntegerValue(), bits_per_pixel);
    }
    cout<<"Creating VideoMode "<<video_mode<<endl;
    vm->SetInternalField(0, External::New(video_mode));
    Persistent<Object>::New(vm).MakeWeak(video_mode, Dispose<sf::VideoMode>);
    return scope.Close(vm);
  }

  return scope.Close(ThrowError("Must use new operator"));
}

JS_METHOD(VideoMode_IsValid) {
  HandleScope scope;
  Handle<Boolean> valid = Boolean::New(((sf::VideoMode*) External::Unwrap(args.Holder()->GetInternalField(0)))->IsValid());
  return scope.Close(valid);
}
JS_METHOD(VideoMode_GetDesktopMode) {
  HandleScope scope;
  sf::VideoMode* video_mode = new sf::VideoMode(sf::VideoMode::GetDesktopMode());
  Handle<Object> vm = VideoModeTemplate->InstanceTemplate()->NewInstance();
  cout<<"Creating Desktop VideoMode "<<video_mode<<endl;
  vm->SetInternalField(0, External::New(video_mode));
  Persistent<Object>::New(vm).MakeWeak(video_mode, Dispose<sf::VideoMode>);
  return scope.Close(vm);
}
Handle<Value> VideoMode__Getter_Width(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::VideoMode>(info,0)->Width));
}
void VideoMode__Setter_Width(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::VideoMode>(info,0)->Width = value->IntegerValue();
  scope.Close(value);
}
Handle<Value> VideoMode__Getter_Height(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::VideoMode>(info,0)->Height));
}
void VideoMode__Setter_Height(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::VideoMode>(info,0)->Height = value->IntegerValue();
  scope.Close(value);
}
Handle<Value> VideoMode__Getter_BitsPerPixel(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::VideoMode>(info,0)->BitsPerPixel));
}
void VideoMode__Setter_BitsPerPixel(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::VideoMode>(info,0)->BitsPerPixel = value->IntegerValue();
  scope.Close(value);
}

void VideoMode_init(Handle<Object> target) {
  INIT(VideoMode);
  VideoModeTemplateInstanceTemplate->SetAccessor(JS_STR("Width"), VideoMode__Getter_Width, VideoMode__Setter_Width);
  VideoModeTemplateInstanceTemplate->SetAccessor(JS_STR("Height"), VideoMode__Getter_Height, VideoMode__Setter_Height);
  VideoModeTemplateInstanceTemplate->SetAccessor(JS_STR("BitsPerPixel"), VideoMode__Getter_BitsPerPixel,
      VideoMode__Setter_BitsPerPixel);
  VideoModeTemplateInstanceTemplate->Set("IsValid", FunctionTemplate::New(VideoMode_IsValid));
  VideoModeTemplateInstanceTemplate->Set("GetDesktopMode", FunctionTemplate::New(VideoMode_GetDesktopMode));
}

///////////////////////////////////////////////////////////////////////////////
//
// Texture
//
///////////////////////////////////////////////////////////////////////////////

Persistent<FunctionTemplate> TextureTemplate;
JS_METHOD(Texture) {
  HandleScope scope;
  if (args.IsConstructCall()) {
    sf::Texture* texture = new sf::Texture();
    Handle<Object> vm = args.This();
    cout<<"Creating Texture "<<texture<<endl;
    vm->SetInternalField(0, External::New(texture));
    Persistent<Object>::New(vm).MakeWeak(texture, Dispose<sf::Texture>);
    return scope.Close(vm);
  }

  return scope.Close(ThrowError("Must use new operator"));
}

JS_METHOD(Texture_GetWidth) {
  HandleScope scope;
  return scope.Close(JS_INT(UnwrapField<sf::Texture>(args,0)->GetWidth()));
}

JS_METHOD(Texture_GetHeight) {
  HandleScope scope;
  return scope.Close(JS_INT(UnwrapField<sf::Texture>(args,0)->GetHeight()));
}

JS_METHOD(Texture_LoadFromFile) {
  HandleScope scope;
  String::Utf8Value filename(args[0]->ToString());
  return scope.Close(JS_INT(UnwrapField<sf::Texture>(args,0)->LoadFromFile(*filename)));
}

void Texture_init(Handle<Object> target) {
  INIT(Texture);
  TextureTemplateInstanceTemplate->Set("GetWidth", FunctionTemplate::New(Texture_GetWidth));
  TextureTemplateInstanceTemplate->Set("GetHeight", FunctionTemplate::New(Texture_GetHeight));
  TextureTemplateInstanceTemplate->Set("LoadFromFile", FunctionTemplate::New(Texture_LoadFromFile));
}

///////////////////////////////////////////////////////////////////////////////
//
// RenderWindow
//
///////////////////////////////////////////////////////////////////////////////

Persistent<FunctionTemplate> RenderWindowTemplate;
JS_METHOD(RenderWindow) {
  HandleScope scope;
  if (args.IsConstructCall()) {
    sf::VideoMode *video_mode = static_cast<sf::VideoMode*>(External::Unwrap(args[0]->ToObject()->GetInternalField(0)));
    char* title = args[1]->IsUndefined() ? (char*) "" : *String::Utf8Value(args[1]);
    int style = args[2]->IsUndefined() ? (sf::Style::Resize | sf::Style::Close) : args[2]->IntegerValue();
    sf::RenderWindow *RenderWindow;
    if(!args[3]->IsUndefined()) {
      sf::ContextSettings *settings = static_cast<sf::ContextSettings*>(External::Unwrap(args[3]->ToObject()->GetInternalField(0)));
      RenderWindow = new sf::RenderWindow(*video_mode, title, style, *settings);
    }
    else
      RenderWindow = new sf::RenderWindow(*video_mode, title, style);
    cout<<"Creating RenderWindow "<<RenderWindow<<endl;
    args.This()->SetInternalField(0, External::New(RenderWindow));
    Persistent<Object>::New(args.This()).MakeWeak(RenderWindow, Dispose<sf::RenderWindow>);
    return scope.Close(args.This());
  }

  return scope.Close(ThrowError("Must use new operator"));
}

JS_METHOD(RenderWindow_Close) {
  HandleScope scope;
  cout << "Closing window" << endl;
  UnwrapField<sf::RenderWindow>(args,0)->Close();
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_SetTitle) {
  HandleScope scope;
  String::Utf8Value str(args[0]->ToString());
  UnwrapField<sf::RenderWindow>(args,0)->SetTitle(*str);
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_Display) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->Display();
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_GetWidth) {
  HandleScope scope;
  Handle<Number> width = JS_INT(UnwrapField<sf::RenderWindow>(args,0)->GetWidth());
  return scope.Close(width);
}
JS_METHOD(RenderWindow_GetHeight) {
  HandleScope scope;
  Handle<Number> height = JS_INT(UnwrapField<sf::RenderWindow>(args,0)->GetHeight());
  return scope.Close(height);
}
JS_METHOD(RenderWindow_Draw) {
  HandleScope scope;
  sf::Sprite *sprite = (sf::Sprite*) args[0]->ToObject()->GetPointerFromInternalField(0);
  UnwrapField<sf::RenderWindow>(args,0)->Draw(*sprite);
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_Clear) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->Clear();
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_IsOpen) {
  HandleScope scope;
  return scope.Close(JS_BOOL(UnwrapField<sf::RenderWindow>(args,0)->IsOpen()));
}

JS_METHOD(RenderWindow_EnableVerticalSync) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->EnableVerticalSync(args[0]->BooleanValue());
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_SetFramerateLimit) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->SetFramerateLimit(args[0]->Int32Value());
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_PollEvent) {
  HandleScope scope;

  sf::RenderWindow *window = UnwrapField<sf::RenderWindow>(args,0);
  sf::Event event;
  if (!window->PollEvent(event)) {
    return scope.Close(Undefined());
  }

  Local<Object> evt = Object::New();

  switch (event.Type) {
  case sf::Event::KeyReleased:
  case sf::Event::KeyPressed: {
    sf::Event::KeyEvent kevt=event.Key;
    evt->Set(JS_STR("Type"), JS_STR(event.Type==sf::Event::KeyReleased ? "keyup" : "keydown"));
    evt->Set(JS_STR("Code"), JS_INT(kevt.Code));
    evt->Set(JS_STR("Alt"), JS_BOOL(kevt.Alt));
    evt->Set(JS_STR("Shift"), JS_BOOL(kevt.Shift));
    evt->Set(JS_STR("Control"), JS_BOOL(kevt.Control));
    break;
  }
  case sf::Event::MouseButtonReleased:
  case sf::Event::MouseButtonPressed: {
    sf::Event::MouseButtonEvent mevt=event.MouseButton;
    evt->Set(JS_STR("Type"), JS_STR(event.Type==sf::Event::MouseButtonReleased ? "mousebuttonup" : "mousebuttondown"));
    evt->Set(JS_STR("Button"), JS_INT(mevt.Button));
    evt->Set(JS_STR("X"), JS_INT(mevt.X));
    evt->Set(JS_STR("Y"), JS_INT(mevt.Y));
    break;
  }
  case sf::Event::MouseMoved: {
    sf::Event::MouseMoveEvent mevt=event.MouseMove;
    evt->Set(JS_STR("Type"), JS_STR("mousemove"));
    evt->Set(JS_STR("X"), JS_INT(mevt.X));
    evt->Set(JS_STR("Y"), JS_INT(mevt.Y));
    break;
  }
  case sf::Event::MouseWheelMoved: {
    sf::Event::MouseWheelEvent mevt=event.MouseWheel;
    evt->Set(JS_STR("Type"), JS_STR("mousewheel"));
    evt->Set(JS_STR("Delta"), JS_INT(mevt.Delta));
    evt->Set(JS_STR("X"), JS_INT(mevt.X));
    evt->Set(JS_STR("Y"), JS_INT(mevt.Y));
    break;
  }
  case sf::Event::Resized: {
    sf::Event::SizeEvent mevt=event.Size;
    evt->Set(JS_STR("Type"), JS_STR("resize"));
    evt->Set(JS_STR("Width"), JS_INT(mevt.Width));
    evt->Set(JS_STR("Height"), JS_INT(mevt.Height));
    break;
  }
  case sf::Event::MouseEntered:
    evt->Set(JS_STR("Type"), JS_STR("mouseentered"));
    break;
  case sf::Event::MouseLeft:
    evt->Set(JS_STR("Type"), JS_STR("mouseleft"));
    break;
  case sf::Event::Closed:
    evt->Set(JS_STR("Type"), JS_STR("closed"));
    break;
  default:
    evt->Set(JS_STR("Type"), JS_STR("unknown"));
    evt->Set(JS_STR("Code"), JS_INT(event.Type));
    break;
  }
  return scope.Close(evt);
}

JS_METHOD(RenderWindow_SetActive) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->SetActive();
  return scope.Close(Undefined());
}

JS_METHOD(RenderWindow_SetSize) {
  HandleScope scope;
  UnwrapField<sf::RenderWindow>(args,0)->SetSize(args[0]->Uint32Value(),args[1]->Uint32Value());
  return scope.Close(Undefined());
}
/*
struct event_request {
    v8::Persistent<v8::Function> callback;
    void *window_obj;
    char *error;
    sf::Event event;
};

void EIO_RenderWindow_EventLoop(eio_req *req) {
  event_request *evt_req = (event_request *) req->data;
  sf::RenderWindow *window = (sf::RenderWindow *) evt_req->window_obj;

  bool valid=false;
  do {
    valid = false;
    if(window->IsOpen()) {
      valid = true;
      while(window->PollEvent(evt_req->event)) {
        cout<<"got event "<<evt_req->event.Type<<endl;
        valid=false;
        break;
      }
    }
  } while (valid);

  ev_unref(EV_DEFAULT_UC);

}

int EIO_RenderWindow_EventLoopAfter(eio_req *req)
{
    HandleScope scope;

    event_request *evt_req = (event_request *) req->data;

    //emit the event
    Handle<Value> argv[1];

    Local<Object> evt = Object::New();

    switch (evt_req->event.Type) {
    case sf::Event::Closed:
      evt->Set(JS_STR("Type"), JS_STR("CLOSED"));
      break;
    default:
      evt->Set(JS_STR("Type"), JS_STR("UNKNOWN"));
      evt->Set(JS_STR("Code"), JS_INT(evt_req->event.Type));
      break;
    }
    argv[0]=evt;

    ev_unref(EV_DEFAULT_UC);

    TryCatch try_catch; // don't quite see the necessity of this
    evt_req->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught())
        node::FatalException(try_catch);

    evt_req->callback.Dispose();
    if(evt_req->error) free(evt_req->error);
    delete evt_req;

    return 0;
}

JS_METHOD(RenderWindow_EventLoop)
{
    HandleScope scope;

    if (args.Length() != 1)
        return ThrowError("One argument required - callback function.");

    REQ_FUN_ARG(0,callback);

    sf::RenderWindow *window=UnwrapField<sf::RenderWindow>(args,0);

    event_request *evt_req = new event_request();
    if (!evt_req)
        return ThrowError("Can NOT allocate event_request.");

    evt_req->callback = Persistent<Function>::New(callback);
    evt_req->window_obj=window;
    evt_req->error = NULL;

    cout<<"Queueing EventLoop"<<endl;
    eio_custom(EIO_RenderWindow_EventLoop, EIO_PRI_DEFAULT, EIO_RenderWindow_EventLoopAfter, evt_req);

    ev_ref(EV_DEFAULT_UC);

    return scope.Close(Undefined());
}
*/
void RenderWindow_init(Handle<Object> target) {
  INIT(RenderWindow);
  RenderWindowTemplateInstanceTemplate->Set("SetTitle", FunctionTemplate::New(RenderWindow_SetTitle));
  RenderWindowTemplateInstanceTemplate->Set("Clear", FunctionTemplate::New(RenderWindow_Clear));
  RenderWindowTemplateInstanceTemplate->Set("Close", FunctionTemplate::New(RenderWindow_Close));
  RenderWindowTemplateInstanceTemplate->Set("Display", FunctionTemplate::New(RenderWindow_Display));
  RenderWindowTemplateInstanceTemplate->Set("Draw", FunctionTemplate::New(RenderWindow_Draw));
  RenderWindowTemplateInstanceTemplate->Set("GetHeight", FunctionTemplate::New(RenderWindow_GetHeight));
  RenderWindowTemplateInstanceTemplate->Set("GetWidth", FunctionTemplate::New(RenderWindow_GetWidth));
  RenderWindowTemplateInstanceTemplate->Set("IsOpen", FunctionTemplate::New(RenderWindow_IsOpen));
  RenderWindowTemplateInstanceTemplate->Set("PollEvent", FunctionTemplate::New(RenderWindow_PollEvent));
  RenderWindowTemplateInstanceTemplate->Set("EnableVerticalSync", FunctionTemplate::New(RenderWindow_EnableVerticalSync));
  RenderWindowTemplateInstanceTemplate->Set("SetFramerateLimit", FunctionTemplate::New(RenderWindow_SetFramerateLimit));
  RenderWindowTemplateInstanceTemplate->Set("SetActive", FunctionTemplate::New(RenderWindow_SetActive));
  RenderWindowTemplateInstanceTemplate->Set("SetSize", FunctionTemplate::New(RenderWindow_SetSize));
  //RenderWindowTemplateInstanceTemplate->Set("EventLoop", FunctionTemplate::New(RenderWindow_EventLoop));
}

///////////////////////////////////////////////////////////////////////////////
//
// Sprite
//
///////////////////////////////////////////////////////////////////////////////

Persistent<FunctionTemplate> SpriteTemplate;
JS_METHOD(Sprite) {
  HandleScope scope;
  if (args.IsConstructCall()) {
    sf::Sprite *sprite;
    if (args.Length()) {
      sf::Texture *texture = (sf::Texture*) args[0]->ToObject()->GetPointerFromInternalField(0);
      Persistent<Object>::New(args[0]->ToObject()).ClearWeak(); // texture is not weak anymore since Sprite refers to it
      Handle<Value> rect = args[1];
      if (!rect->IsUndefined() && rect->IsArray()) {
        Handle<Object> arr = rect->ToObject();
        float x = arr->Get(0)->NumberValue();
        float y = arr->Get(1)->NumberValue();
        float w = arr->Get(2)->NumberValue();
        float h = arr->Get(3)->NumberValue();
        sprite = new sf::Sprite(*texture, sf::IntRect(x, y, w, h));
      } else {
        //cout<<"Creating sprite from texture"<<endl;
        sprite = new sf::Sprite(*texture);
      }
    } else {
      sprite = new sf::Sprite();
    }
    cout<<"Creating Sprite "<<sprite<<endl;
    args.Holder()->SetPointerInInternalField(0, sprite);
    Persistent<Object>::New(args.This()).MakeWeak(sprite, Dispose<sf::Sprite>);
    return scope.Close(args.This());
  }
  return scope.Close(ThrowError("Must use new operator"));
}

JS_METHOD(Sprite_SetScale) {
  HandleScope scope;
  float sx = args[0]->NumberValue();
  float sy = args[1]->NumberValue();

  ((sf::Sprite*) External::Unwrap(args.Holder()->GetInternalField(0)))->SetScale(sx, sy);
  return scope.Close(Undefined());
}

void Sprite_init(Handle<Object> target) {
  INIT(Sprite);
  SpriteTemplateInstanceTemplate->Set("SetScale", FunctionTemplate::New(Sprite_SetScale));
}
///////////////////////////////////////////////////////////////////////////////
//
// ContextSettings
//
///////////////////////////////////////////////////////////////////////////////

Persistent<FunctionTemplate> ContextSettingsTemplate;
JS_METHOD(ContextSettings) {
  HandleScope scope;
  if (args.IsConstructCall()) {
    sf::ContextSettings *settings;
    if (args.Length()) {
      unsigned int depth=0,stencil=0,antialiasing=0,major=2,minor=2;
      if(!args[0]->IsUndefined()) depth=args[0]->Int32Value();
      if(!args[1]->IsUndefined()) stencil=args[0]->Int32Value();
      if(!args[2]->IsUndefined()) antialiasing=args[0]->Int32Value();
      if(!args[3]->IsUndefined()) major=args[0]->Int32Value();
      if(!args[4]->IsUndefined()) minor=args[0]->Int32Value();
      settings = new sf::ContextSettings(depth,stencil,antialiasing,major,minor);
    } else {
      settings = new sf::ContextSettings();
    }
    cout<<"Creating ContextSettings "<<settings<<endl;
    args.Holder()->SetPointerInInternalField(0, settings);
    Persistent<Object>::New(args.This()).MakeWeak(settings, Dispose<sf::ContextSettings>);
    return scope.Close(args.This());
  }
  return scope.Close(ThrowError("Must use new operator"));
}

Handle<Value> ContextSettings__Getter_DepthBits(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::ContextSettings>(info,0)->DepthBits));
}
void ContextSettings__Setter_DepthBits(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::ContextSettings>(info,0)->DepthBits = value->IntegerValue();
  scope.Close(value);
}

Handle<Value> ContextSettings__Getter_StencilBits(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::ContextSettings>(info,0)->StencilBits));
}
void ContextSettings__Setter_StencilBits(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::ContextSettings>(info,0)->StencilBits = value->IntegerValue();
  scope.Close(value);
}

Handle<Value> ContextSettings__Getter_AntialiasingLevel(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::ContextSettings>(info,0)->AntialiasingLevel));
}
void ContextSettings__Setter_AntialiasingLevel(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::ContextSettings>(info,0)->AntialiasingLevel = value->IntegerValue();
  scope.Close(value);
}

Handle<Value> ContextSettings__Getter_MajorVersion(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::ContextSettings>(info,0)->MajorVersion));
}
void ContextSettings__Setter_MajorVersion(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::ContextSettings>(info,0)->MajorVersion = value->IntegerValue();
  scope.Close(value);
}

Handle<Value> ContextSettings__Getter_MinorVersion(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  return scope.Close(JS_NUM(UnwrapField<sf::ContextSettings>(info,0)->MinorVersion));
}
void ContextSettings__Setter_MinorVersion(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  UnwrapField<sf::ContextSettings>(info,0)->MinorVersion = value->IntegerValue();
  scope.Close(value);
}

void ContextSettings_init(Handle<Object> target) {
  INIT(ContextSettings);
  ContextSettingsTemplateInstanceTemplate->SetAccessor(JS_STR("DepthBits"), ContextSettings__Getter_DepthBits, ContextSettings__Setter_DepthBits);
  ContextSettingsTemplateInstanceTemplate->SetAccessor(JS_STR("StencilBits"), ContextSettings__Getter_StencilBits, ContextSettings__Setter_StencilBits);
  ContextSettingsTemplateInstanceTemplate->SetAccessor(JS_STR("AntialiasingLevel"), ContextSettings__Getter_AntialiasingLevel, ContextSettings__Setter_AntialiasingLevel);
  ContextSettingsTemplateInstanceTemplate->SetAccessor(JS_STR("MajorVersion"), ContextSettings__Getter_MajorVersion, ContextSettings__Setter_MajorVersion);
  ContextSettingsTemplateInstanceTemplate->SetAccessor(JS_STR("MinorVersion"), ContextSettings__Getter_MinorVersion, ContextSettings__Setter_MinorVersion);
}

} // namespace sfml

///////////////////////////////////////////////////////////////////////////////
//
// bindings
//
///////////////////////////////////////////////////////////////////////////////

extern "C" {
void init(Handle<Object> target) {
  HandleScope scope;

  sfml::Sprite_init(target);
  sfml::Texture_init(target);
  sfml::RenderWindow_init(target);
  sfml::VideoMode_init(target);
  sfml::ContextSettings_init(target);

  // enums
  Handle<Object> Style = Object::New();
  Style->Set(JS_STR("None"),JS_NUM(sf::Style::None));
  Style->Set(JS_STR("Titlebar"),JS_NUM(sf::Style::Titlebar));
  Style->Set(JS_STR("Resize"),JS_NUM(sf::Style::Resize));
  Style->Set(JS_STR("Close"),JS_NUM(sf::Style::Close));
  Style->Set(JS_STR("Fullscreen"),JS_NUM(sf::Style::Fullscreen));
  Style->Set(JS_STR("Default"),JS_NUM(sf::Style::Default));
  target->Set(JS_STR("Style"), Style);
}

NODE_MODULE(node_sfml, init)
}

