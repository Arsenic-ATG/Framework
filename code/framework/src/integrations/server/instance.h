/*
 * MafiaHub OSS license
 * Copyright (c) 2022, MafiaHub. All rights reserved.
 *
 * This file comes from MafiaHub, hosted at https://github.com/MafiaHub/Framework.
 * See LICENSE file in the source repository for information regarding licensing.
 */

#pragma once

#include "../shared/types/environment.hpp"
#include "../shared/types/streaming.hpp"
#include "../shared/types/player.hpp"
#include "errors.h"
#include "external/firebase/wrapper.h"
#include "masterlist.h"
#include "networking/engine.h"
#include "utils/config.h"
#include "world/server.h"

#include <chrono>
#include <http/webserver.h>
#include <logging/logger.h>
#include <memory>
#include <scripting/engine.h>
#include <sig.h>
#include <string>

namespace Framework::Integrations::Server {
    struct InstanceOptions {
        std::string modSlug;
        std::string modHelpText;
        std::string modName;
        std::string modVersion;
        std::string modConfigFile = "server.json";

        std::string bindName;
        std::string bindHost;
        std::string bindSecretKey;
        std::string bindMapName;
        int32_t bindPort;
        std::string bindPassword;
        bool bindPublicServer = false;

        int32_t maxPlayers;
        std::string httpServeDir;

        bool enableSignals;

        // update intervals
        float tickInterval         = 0.016667f;
        float streamerTickInterval = 0.033334f;

        // args
        int argc;
        char **argv;

        // firebase
        bool firebaseEnabled = false;
        std::string firebaseProjectId;
        std::string firebaseAppId;
        std::string firebaseApiKey;

        // scripting
        Scripting::SDKRegisterCallback sdkRegisterCallback;
    };

    using OnPlayerConnectionCallback = std::function<void(flecs::entity, uint64_t)>;

    class Instance {
      private:
        bool _alive;
        std::chrono::time_point<std::chrono::high_resolution_clock> _nextTick;

        InstanceOptions _opts;

        std::unique_ptr<Scripting::Engine> _scriptingEngine;
        std::unique_ptr<Networking::Engine> _networkingEngine;
        std::unique_ptr<HTTP::Webserver> _webServer;
        std::unique_ptr<External::Firebase::Wrapper> _firebaseWrapper;
        std::unique_ptr<Masterlist> _masterlistSync;
        std::unique_ptr<Utils::Config> _fileConfig;
        std::shared_ptr<World::ServerEngine> _worldEngine;

        void InitEndpoints();
        void InitModules();
        void InitManagers();
        void InitNetworkingMessages();
        bool LoadConfigFromJSON();

        // managers
        flecs::entity _weatherManager;

        // entity factories
        std::unique_ptr<Shared::Archetypes::EnvironmentFactory> _envFactory;
        std::unique_ptr<Shared::Archetypes::PlayerFactory> _playerFactory;
        std::unique_ptr<Shared::Archetypes::StreamingFactory> _streamingFactory;

        // callbacks
        OnPlayerConnectionCallback _onPlayerConnectedCallback;
        OnPlayerConnectionCallback _onPlayerDisconnectedCallback;

        void RegisterScriptingBuiltins(Scripting::SDK *);

      public:
        Instance();
        ~Instance();

        ServerError Init(InstanceOptions &);
        ServerError Shutdown();

        virtual void PostInit() {}

        virtual void PostUpdate() {}

        virtual void PreShutdown() {}

        void Update();

        void Run();

        void OnSignal(const sig_signal_t);

        bool IsAlive() const {
            return _alive;
        }

        void SetOnPlayerConnectedCallback(OnPlayerConnectionCallback onPlayerConnectedCallback) {
            _onPlayerConnectedCallback = onPlayerConnectedCallback;
        }

        void SetOnPlayerDisconnectedCallback(OnPlayerConnectionCallback onPlayerDisconnectedCallback) {
            _onPlayerDisconnectedCallback = onPlayerDisconnectedCallback;
        }

        InstanceOptions &GetOpts() {
            return _opts;
        }

        Scripting::Engine *GetScriptingEngine() const {
            return _scriptingEngine.get();
        }

        std::shared_ptr<World::ServerEngine> GetWorldEngine() const {
            return _worldEngine;
        }

        Networking::Engine *GetNetworkingEngine() const {
            return _networkingEngine.get();
        }

        HTTP::Webserver *GetWebserver() const {
            return _webServer.get();
        }

        Shared::Archetypes::PlayerFactory* GetPlayerFactory() const {
            return _playerFactory.get();
        }

        Shared::Archetypes::StreamingFactory* GetStreamingFactory() const {
            return _streamingFactory.get();
        }
    };
} // namespace Framework::Integrations::Server
