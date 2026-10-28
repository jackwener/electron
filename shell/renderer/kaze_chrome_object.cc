// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/renderer/kaze_chrome_object.h"

#include <initializer_list>
#include <tuple>
#include <utility>

#include "chrome/renderer/loadtimes_bindings.h"  // nogncheck
#include "content/public/renderer/chrome_object_extensions_utils.h"
#include "gin/converter.h"
#include "v8/include/v8-context.h"
#include "v8/include/v8-function.h"
#include "v8/include/v8-isolate.h"
#include "v8/include/v8-microtask-queue.h"
#include "v8/include/v8-object.h"
#include "v8/include/v8-primitive.h"

namespace electron {

namespace {

// chrome.app for a page that isn't an installed app: what Chrome's
// AppHooksDelegate (chrome/renderer/extensions/api/app_hooks_delegate.cc)
// answers there.
void GetIsInstalled(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(false);
}

void GetDetails(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().SetNull();
}

void RunningState(const v8::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(gin::StringToV8(info.GetIsolate(), "cannot_run"));
}

// The callback hears "not_installed" later, as Chrome's answers from the
// browser do — never during the call.
void AnswerInstallState(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Function> callback = info.Data().As<v8::Function>();
  v8::Local<v8::Value> state = gin::StringToV8(isolate, "not_installed");
  std::ignore = callback->Call(isolate->GetCurrentContext(),
                               v8::Undefined(isolate), 1, &state);
}

void InstallState(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  if (info.Length() < 1 || !info[0]->IsFunction()) {
    isolate->ThrowException(v8::Exception::TypeError(gin::StringToV8(
        isolate,
        "Error in invocation of app.installState(function callback): No "
        "matching signature.")));
    return;
  }
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Function> answer;
  if (!v8::Function::New(context, AnswerInstallState, info[0], 0,
                         v8::ConstructorBehavior::kThrow)
           .ToLocal(&answer)) {
    return;
  }
  context->GetMicrotaskQueue()->EnqueueMicrotask(isolate, answer);
}

bool SetMethod(v8::Local<v8::Context> context,
               v8::Local<v8::Object> target,
               const char* name,
               v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Function> function;
  if (!v8::Function::New(context, callback, v8::Local<v8::Value>(), 0,
                         v8::ConstructorBehavior::kThrow)
           .ToLocal(&function)) {
    return false;
  }
  v8::Local<v8::String> key = gin::StringToSymbol(isolate, name);
  function->SetName(key);
  return target->Set(context, key, function).FromMaybe(false);
}

v8::Local<v8::Object> Enum(v8::Isolate* isolate,
                           v8::Local<v8::Context> context,
                           std::initializer_list<std::pair<const char*,
                                                           const char*>>
                               values) {
  v8::Local<v8::Object> object = v8::Object::New(isolate);
  for (const auto& [key, value] : values) {
    std::ignore = object->Set(context, gin::StringToSymbol(isolate, key),
                              gin::StringToV8(isolate, value));
  }
  return object;
}

void InstallApp(v8::Isolate* isolate, v8::Local<v8::Context> context) {
  v8::Local<v8::Object> chrome =
      content::GetOrCreateChromeObject(isolate, context);
  v8::Local<v8::String> app_key = gin::StringToSymbol(isolate, "app");
  if (chrome->HasOwnProperty(context, app_key).FromMaybe(true))
    return;
  v8::Local<v8::Object> app = v8::Object::New(isolate);
  std::ignore = app->Set(context, gin::StringToSymbol(isolate, "isInstalled"),
                         v8::False(isolate));
  if (!SetMethod(context, app, "getDetails", GetDetails) ||
      !SetMethod(context, app, "getIsInstalled", GetIsInstalled) ||
      !SetMethod(context, app, "installState", InstallState) ||
      !SetMethod(context, app, "runningState", RunningState)) {
    return;
  }
  std::ignore = app->Set(context, gin::StringToSymbol(isolate, "InstallState"),
                         Enum(isolate, context,
                              {{"DISABLED", "disabled"},
                               {"INSTALLED", "installed"},
                               {"NOT_INSTALLED", "not_installed"}}));
  std::ignore = app->Set(context, gin::StringToSymbol(isolate, "RunningState"),
                         Enum(isolate, context,
                              {{"CANNOT_RUN", "cannot_run"},
                               {"READY_TO_RUN", "ready_to_run"},
                               {"RUNNING", "running"}}));
  std::ignore = chrome->Set(context, app_key, app);
}

}  // namespace

void InstallKazeChromeObject(v8::Local<v8::Context> context) {
  if (context.IsEmpty())
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::MicrotasksScope microtasks_scope(
      isolate, context->GetMicrotaskQueue(),
      v8::MicrotasksScope::kDoNotRunMicrotasks);
  v8::Context::Scope context_scope(context);
  // Chrome's own chrome.loadTimes() and chrome.csi().
  LoadTimesBindings::Install(context);
  InstallApp(isolate, context);
}

}  // namespace electron
