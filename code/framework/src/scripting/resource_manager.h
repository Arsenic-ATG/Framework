/*
 * MafiaHub OSS license
 * Copyright (c) 2022, MafiaHub. All rights reserved.
 *
 * This file comes from MafiaHub, hosted at https://github.com/MafiaHub/Framework.
 * See LICENSE file in the source repository for information regarding licensing.
 */

#pragma once

#include "errors.h"
#include "init.h"

#include <functional>
#include <map>
#include <string>
#include <v8.h>

namespace Framework::Scripting {
    class Engine;
    class Resource;

    class ResourceManager {
      private:
        Engine *_engine = nullptr;
        std::map<std::string, Resource *> _resources;

      public:
        ResourceManager(Engine *);

        ~ResourceManager();

        ResourceManagerError Load(std::string, SDKRegisterCallback = nullptr);

        void LoadAll(SDKRegisterCallback = nullptr);

        ResourceManagerError Unload(std::string);

        void UnloadAll();

        size_t ResourcesLength() const {
            return _resources.size();
        }

        std::map<std::string, Resource *> GetAllResources() const {
            return _resources;
        }

        void InvokeErrorEvent(const std::string &, const std::string &, const std::string &, int32_t);

        template <typename... Args>
        void InvokeEvent(const std::string &eventName, Args &&... args) {
            auto isolate = _engine->GetIsolate();
            v8::Locker locker(isolate);
            v8::Isolate::Scope isolateScope(isolate);
            v8::HandleScope handleScope(isolate);

            for (auto resPair : _resources) {
                const auto res = resPair.second;

                auto ctx = res->GetContext();
                v8::Context::Scope contextScope(ctx);

                auto args = cb(isolate, ctx);
                res->InvokeEvent(eventName, false, std::forward(args...));
            }
        }
    };
} // namespace Framework::Scripting
