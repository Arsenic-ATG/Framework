#include "v8_module.h"

namespace Framework::Scripting::Helpers {
    V8Module::V8Module(std::string name, ModuleInitCallback cb): _name(name), _initCb(cb) {
        auto isolate = v8::Isolate::GetCurrent();
        if (!isolate) {
            // gServer->GetLoggingModule()->Get(LOG_SCRIPTING)->critical("Null isolate");
            return;
        }

        _objTemplate = v8::ObjectTemplate::New(isolate);
        if (_objTemplate.IsEmpty()) {
            // gServer->GetLoggingModule()->Get(LOG_SCRIPTING)->critical("Empty object template");
            return;
        }
    }

    V8HelperError V8Module::AddClass(V8Class *cls) {
        if(_classes.count(cls->GetName()) > 0){
            return HELPER_ALREADY_LOADED;
        }
        
        cls->Load();
        _classes[cls->GetName()] = cls;
        return HELPER_NONE;
    }

    V8Class* V8Module::GetClass(const std::string &name){
        return _classes[name];
    }

    V8HelperError V8Module::Register(v8::Local<v8::Context> context) {
        auto isolate = v8::Isolate::GetCurrent();
        if (!isolate) {
            return HELPER_ISOLATE_NULL;
        }

        if (context.IsEmpty()) {
            return HELPER_CONTEXT_EMPTY;
        }

        // Load all classes
        for (auto &cls : _classes) { 
            cls.second->Load();
        }

        // Create the object template
        v8::Local<v8::Object> objTpl = _objTemplate->NewInstance(context).ToLocalChecked();

        // Create the inner content of the module
        if(_initCb){
            _initCb(context, objTpl);
        }

        // Bind the classes to the object template
        for (auto &cls : _classes) { 
            cls.second->Register(context, objTpl); 
        };

        // Register the obj template to the global obj template
        v8::Maybe<bool> res = context->Global()->Set(
            context, v8::String::NewFromUtf8(isolate, _name.c_str(), v8::NewStringType::kNormal).ToLocalChecked(),
            objTpl);

        // If something went weird, just cancel and evacuate
        if (!res.ToChecked()) {
            return HELPER_REGISTER_FAILED;
        }
        return HELPER_NONE;
    }
} // namespace Framework::Scripting::Helpers
