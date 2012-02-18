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
      video_mode = new sf::VideoMode(args[0]->IntegerValue(), args[1]->IntegerValue(), args[2]->IsUndefined() ? 32 : args[2]->IntegerValue());
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
vector<sf::RenderWindow*> windows;

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
    windows.push_back(RenderWindow);

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

uint32_t HTMLKey[]=
{
    65,            ///< The A key
    66,            ///< The B key
    67,            ///< The C key
    68,            ///< The D key
    69,            ///< The E key
    70,            ///< The F key
    71,            ///< The G key
    72,            ///< The H key
    73,            ///< The I key
    74,            ///< The J key
    75,            ///< The K key
    76,            ///< The L key
    77,            ///< The M key
    78,            ///< The N key
    79,            ///< The O key
    80,            ///< The P key
    81,            ///< The Q key
    82,            ///< The R key
    83,            ///< The S key
    84,            ///< The T key
    85,            ///< The U key
    86,            ///< The V key
    87,            ///< The W key
    88,            ///< The X key
    89,            ///< The Y key
    90,            ///< The Z key
    48,         ///< The 0 key
    49,         ///< The 1 key
    50,         ///< The 2 key
    51,         ///< The 3 key
    52,         ///< The 4 key
    53,         ///< The 5 key
    54,         ///< The 6 key
    55,         ///< The 7 key
    56,         ///< The 8 key
    57,         ///< The 9 key
    27,       ///< The Escape key
    17,     ///< The left Control key
    16,       ///< The left Shift key
    18,         ///< The left Alt key
    91,      ///< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
    17,     ///< The right Control key
    16,       ///< The right Shift key
    18,         ///< The right Alt key
    93,      ///< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
    0xff,         ///< The Menu key
    219,     ///< The [ key
    221,     ///< The ] key
    186,    ///< The ; key
    188,        ///< The , key
    190,       ///< The . key
    222,        ///< The ' key
    191,        ///< The / key
    220,    ///< The \ key
    192,        ///< The ~ key
    187,        ///< The = key
    189,         ///< The - key
    32,        ///< The Space key
    13,       ///< The Return key
    8,         ///< The Backspace key
    9,          ///< The Tabulation key
    33,       ///< The Page up key
    34,     ///< The Page down key
    35,          ///< The End key
    36,         ///< The Home key
    0xff,       ///< The Insert key
    46,       ///< The Delete key
    187,          ///< +
    189,     ///< -
    0xff,     ///< *
    191,       ///< /
    37,         ///< Left arrow
    39,        ///< Right arrow
    38,           ///< Up arrow
    40,         ///< Down arrow
    96,      ///< The numpad 0 key
    97,      ///< The numpad 1 key
    98,      ///< The numpad 2 key
    99,      ///< The numpad 3 key
    100,      ///< The numpad 4 key
    101,      ///< The numpad 5 key
    102,      ///< The numpad 6 key
    103,      ///< The numpad 7 key
    104,      ///< The numpad 8 key
    105,      ///< The numpad 9 key
    112,           ///< The F1 key
    113,           ///< The F2 key
    114,           ///< The F3 key
    115,           ///< The F4 key
    116,           ///< The F5 key
    117,           ///< The F6 key
    118,           ///< The F7 key
    119,           ///< The F8 key
    120,           ///< The F8 key
    121,          ///< The F10 key
    122,          ///< The F11 key
    123,          ///< The F12 key
    124,          ///< The F13 key
    125,          ///< The F14 key
    126,          ///< The F15 key
    0xff,        ///< The Pause key
};

JS_METHOD(RenderWindow_PollEvent) {
  HandleScope scope;

  sf::RenderWindow *window = UnwrapField<sf::RenderWindow>(args,0);
  sf::Event event;
  if (!window->PollEvent(event)) {
    return scope.Close(Undefined());
  }

  Local<Object> evt = Object::New();
  evt->Set(JS_STR("code"), JS_INT(event.Type));

  switch (event.Type) {
  case sf::Event::KeyReleased:
  case sf::Event::KeyPressed: {
    sf::Event::KeyEvent kevt=event.Key;
    evt->Set(JS_STR("type"),JS_STR(event.Type==sf::Event::KeyReleased ? "keyup" : "keydown"));
    evt->Set(JS_STR("ctrlKey"),JS_BOOL(kevt.Control));
    evt->Set(JS_STR("shiftKey"),JS_BOOL(kevt.Shift));
    evt->Set(JS_STR("altKey"),JS_BOOL(kevt.Alt));
    evt->Set(JS_STR("metaKey"),JS_BOOL(kevt.System));

    evt->Set(JS_STR("rawCode"), JS_INT(kevt.Code));
    if(kevt.Code<sf::Keyboard::KeyCount) {
      if(kevt.Code<26) { // A-Z
        char c=(char) (kevt.Code + 65);
        evt->Set(JS_STR("charCode"), JS_STR(&c,1));
      }
      else if(kevt.Code>25 && kevt.Code<36) { // 0-1
        evt->Set(JS_STR("charCode"), JS_INT(kevt.Code-26));
      }
      evt->Set(JS_STR("which"), JS_INT(HTMLKey[kevt.Code]));
      evt->Set(JS_STR("keyCode"), JS_INT(HTMLKey[kevt.Code]));
    }
    break;
  }
  case sf::Event::MouseButtonReleased:
  case sf::Event::MouseButtonPressed: {
    sf::Event::MouseButtonEvent mevt=event.MouseButton;
    evt->Set(JS_STR("type"),JS_STR(event.Type==sf::Event::MouseButtonReleased ? "mouseup" : "mousedown"));
    evt->Set(JS_STR("button"),JS_INT(mevt.Button));
    evt->Set(JS_STR("which"),JS_INT(mevt.Button));
    evt->Set(JS_STR("x"),JS_INT(mevt.X));
    evt->Set(JS_STR("y"),JS_INT(mevt.Y));
    evt->Set(JS_STR("pageX"),JS_INT(mevt.X));
    evt->Set(JS_STR("pageY"),JS_INT(mevt.Y));
    break;
  }
  case sf::Event::MouseMoved: {
    sf::Event::MouseMoveEvent mevt=event.MouseMove;
    evt->Set(JS_STR("type"),JS_STR("mousemove"));
    evt->Set(JS_STR("pageX"),JS_INT(mevt.X));
    evt->Set(JS_STR("pageY"),JS_INT(mevt.Y));
    evt->Set(JS_STR("x"),JS_INT(mevt.X));
    evt->Set(JS_STR("y"),JS_INT(mevt.Y));
    break;
  }
  case sf::Event::MouseWheelMoved: {
    sf::Event::MouseWheelEvent mevt=event.MouseWheel;
    evt->Set(JS_STR("type"),JS_STR("mousewheel"));
    evt->Set(JS_STR("wheelDelta"),JS_INT(mevt.Delta));
    evt->Set(JS_STR("X"), JS_INT(mevt.X));
    evt->Set(JS_STR("Y"), JS_INT(mevt.Y));
    break;
  }
  case sf::Event::Resized: {
    sf::Event::SizeEvent mevt=event.Size;
    evt->Set(JS_STR("type"),JS_STR("resize"));
    evt->Set(JS_STR("width"),JS_INT(mevt.Width));
    evt->Set(JS_STR("height"),JS_INT(mevt.Height));
    break;
  }
  case sf::Event::MouseEntered:
    evt->Set(JS_STR("Type"), JS_STR("mouseentered"));
    break;
  case sf::Event::MouseLeft:
    evt->Set(JS_STR("Type"), JS_STR("mouseleft"));
    break;
  case sf::Event::Closed:
    evt->Set(JS_STR("type"), JS_STR("quit"));
    break;
  default:
    evt->Set(JS_STR("type"), JS_STR("unknown"));
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
      if(!args[1]->IsUndefined()) stencil=args[1]->Int32Value();
      if(!args[2]->IsUndefined()) antialiasing=args[2]->Int32Value();
      if(!args[3]->IsUndefined()) major=args[3]->Int32Value();
      if(!args[4]->IsUndefined()) minor=args[4]->Int32Value();
      cout<<"ContextSettings:"<<endl;
      cout<<"  depth:"<<depth<<endl;
      cout<<"  stencil:"<<stencil<<endl;
      cout<<"  antialiasing:"<<antialiasing<<endl;
      cout<<"  major:"<<major<<endl;
      cout<<"  minor:"<<minor<<endl;
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

// make sure we close everything when we exit
void AtExit() {
  for(vector<sf::RenderWindow*>::const_iterator it=windows.begin();it!=windows.end();it++)
    (*it)->Close();
  windows.clear();
}

} // namespace sfml

///////////////////////////////////////////////////////////////////////////////
//
// bindings
//
///////////////////////////////////////////////////////////////////////////////

extern "C" {
void init(Handle<Object> target) {
  atexit(sfml::AtExit);

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

