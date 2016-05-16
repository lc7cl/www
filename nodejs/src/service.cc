#include <node.h>

namespace dns {

using namespace v8;

void Method(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

void Initialize(Handle<Object> exports, Handle<Object> module) {
    NODE_SET_METHOD(module, "exports", Method);
}

NODE_MODULE(addon, Initialize)

}
