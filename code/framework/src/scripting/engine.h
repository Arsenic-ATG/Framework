#pragma once

#include "errors.h"
#include "init.h"
#include "resource_manager.h"

#include <node.h>
#include <string>
#include <uv.h>
#include <v8.h>
#include <vector>

namespace Framework::Scripting {
    class Engine {
      private:
        v8::Isolate *_isolate           = nullptr;
        node::IsolateData *_isolateData = nullptr;
        uv_loop_t _uvLoop;
        v8::Persistent<v8::ObjectTemplate> _globalObjectTemplate;
        std::unique_ptr<node::MultiIsolatePlatform> _platform;
        std::shared_ptr<node::ArrayBufferAllocator> _allocator;

        ResourceManager *_resourceManager = nullptr;

      public:
        Engine() = default;

        ~Engine();

        EngineError Init(SDKRegisterCallback = nullptr);

        EngineError Shutdown();

        void Update();

        v8::Isolate *GetIsolate() const {
            return _isolate;
        }

        node::IsolateData *GetIsolateData() const {
            return _isolateData;
        }

        node::MultiIsolatePlatform *GetPlatform() const {
            return _platform.get();
        }

        v8::Persistent<v8::ObjectTemplate> &GetObjectTemplate() {
            return _globalObjectTemplate;
        }

        ResourceManager *GetResourceManager() const {
            return _resourceManager;
        }
    };
} // namespace Framework::Scripting
