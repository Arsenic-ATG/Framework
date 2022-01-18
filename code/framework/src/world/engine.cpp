/*
 * MafiaHub OSS license
 * Copyright (c) 2022, MafiaHub. All rights reserved.
 *
 * This file comes from MafiaHub, hosted at https://github.com/MafiaHub/Framework.
 * See LICENSE file in the source repository for information regarding licensing.
 */

#include "engine.h"

#include "modules/base.hpp"

#include <slikenet/types.h>

namespace Framework::World {
    EngineError Engine::Init(Networking::NetworkPeer *networkPeer) {
        _networkPeer = networkPeer;

        _world = std::make_unique<flecs::world>();

        // Register a base module
        _world->import<Modules::Base>();

        _findAllStreamerEntities = _world->query_builder<Modules::Base::Streamer>().build();

        return EngineError::ENGINE_NONE;
    }

    EngineError Engine::Shutdown() {
        return EngineError::ENGINE_NONE;
    }

    void Engine::Update() {
        _world->progress();
    }

    flecs::entity Engine::GetEntityByGUID(uint64_t guid) {
        flecs::entity ourEntity = {};
        _findAllStreamerEntities.iter([&ourEntity, guid](flecs::iter &it, Modules::Base::Streamer *s) {
            for (auto i : it) {
                if (s[i].guid == guid) {
                    ourEntity = it.entity(i);
                    return;
                }
            }
        });

        return ourEntity;
    }
} // namespace Framework::World
