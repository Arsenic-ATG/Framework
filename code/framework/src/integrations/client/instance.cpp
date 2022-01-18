/*
 * MafiaHub OSS license
 * Copyright (c) 2022, MafiaHub. All rights reserved.
 *
 * This file comes from MafiaHub, hosted at https://github.com/MafiaHub/Framework.
 * See LICENSE file in the source repository for information regarding licensing.
 */

#include "instance.h"

#include "../shared/modules/mod.hpp"

#include <networking/messages/client_handshake.h>
#include <networking/messages/client_connection_finalized.h>
#include <networking/messages/game_sync/entity_messages.h>

#include <logging/logger.h>

namespace Framework::Integrations::Client {
    Instance::Instance() {
        _networkingEngine = std::make_unique<Networking::Engine>();
        _presence    = std::make_unique<External::Discord::Wrapper>();
        _imguiApp    = std::make_unique<External::ImGUI::Wrapper>();
        _renderer    = std::make_unique<Graphics::Renderer>();
        _worldEngine = std::make_unique<World::ClientEngine>();
        _renderIO    = std::make_unique<Graphics::RenderIO>();
    }

    ClientError Instance::Init(InstanceOptions &opts) {
        _opts = opts;

        if (opts.usePresence) {
            if (_presence && opts.discordAppId > 0) {
                _presence->Init(opts.discordAppId);
            }
        }

        if (opts.useRenderer) {
            if (_renderer) {
                _renderer->Init(opts.rendererOptions);
            }
        }

        if (_networkingEngine) {
            _networkingEngine->Init();
        }

        if (_worldEngine) {
            _worldEngine->Init();

            _worldEngine->GetWorld()->import<Shared::Modules::Mod>();
        }

        InitManagers();
        InitNetworkingMessages();
        PostInit();

        Framework::Logging::GetLogger(FRAMEWORK_INNER_CLIENT)->debug("Initialize success");
        _initialized = true;
        return ClientError::CLIENT_NONE;
    }

    ClientError Instance::Shutdown() {
        PreShutdown();

        if (_renderer && _renderer->IsInitialized()) {
            _renderer->Shutdown();
        }

        if (_presence && _presence->IsInitialized()) {
            _presence->Shutdown();
        }

        if (_networkingEngine) {
            _networkingEngine->Shutdown();
        }

        if (_imguiApp && _imguiApp->IsInitialized()) {
            _imguiApp->Shutdown();
        }

        if (_worldEngine) {
            _worldEngine->Shutdown();
        }

        return ClientError::CLIENT_NONE;
    }

    void Instance::Update() {
        if (_presence && _presence->IsInitialized()) {
            _presence->Update();
        }

        if (_networkingEngine) {
            _networkingEngine->Update();
        }

        if (_worldEngine) {
            _worldEngine->Update();
        }

        if (_imguiApp && _imguiApp->IsInitialized()) {
            _imguiApp->Update();
        }

        if (_renderIO) {
            _renderIO->UpdateMainThread();
        }

        PostUpdate();
    }

    void Instance::Render() {
        if (_renderer && _renderer->IsInitialized()) {
            _renderer->Update();
        }

        if (_renderIO) {
            _renderIO->UpdateRenderThread();
        }
    }

    void Instance::InitManagers() {
        _playerFactory.reset(new Integrations::Shared::Archetypes::PlayerFactory);
    }

    void Instance::InitNetworkingMessages() {
        using namespace Framework::Networking::Messages;
        const auto net = _networkingEngine->GetNetworkClient();
        net->SetOnPlayerConnectedCallback([this, net](SLNet::Packet *packet) {
            Logging::GetLogger(FRAMEWORK_INNER_CLIENT)->debug("Connection accepted by server, sending handshake");

            ClientHandshake msg;
            msg.FromParameters(_currentState._nickname, "MY_SUPER_ID_1", "MY_SUPER_ID_2");

            net->Send(msg, SLNet::UNASSIGNED_RAKNET_GUID);
        });
        net->RegisterMessage<ClientConnectionFinalized>(GameMessages::GAME_CONNECTION_FINALIZED, [this, net](SLNet::RakNetGUID guid, ClientConnectionFinalized *msg) {
            Logging::GetLogger(FRAMEWORK_INNER_CLIENT)->debug("Connection request finalized");
            _worldEngine->OnConnect(net, msg->GetServerTickRate());

            // Notify mod-level that network integration whole process succeeded
            if (_onConnectionFinalized) {
                _onConnectionFinalized(msg->GetEntityID());
            }
        });
        net->SetOnPlayerDisconnectedCallback([this](SLNet::Packet *packet, uint32_t reasonId) {
            _worldEngine->OnDisconnect();

            // Notify mod-level that network integration got closed
            if (_onConnectionClosed) {
                _onConnectionClosed();
            }
        });

        // default entity events
        net->RegisterMessage<GameSyncEntitySpawn>(GameMessages::GAME_SYNC_ENTITY_SPAWN, [this](SLNet::RakNetGUID guid, GameSyncEntitySpawn *msg) {
            if (!msg->Valid()) {
                return;
            }
            if (_worldEngine->GetEntityByServerID(msg->GetServerID()).is_alive()) {
                return;
            }
            const auto e = _worldEngine->CreateEntity(msg->GetServerID());

            auto tr = e.get_mut<World::Modules::Base::Transform>();
            *tr     = msg->GetTransform();
        });
        net->RegisterMessage<GameSyncEntityDespawn>(GameMessages::GAME_SYNC_ENTITY_DESPAWN, [this](SLNet::RakNetGUID guid, GameSyncEntityDespawn *msg) {
            if (!msg->Valid()) {
                return;
            }

            const auto e = _worldEngine->GetEntityByServerID(msg->GetServerID());

            if (!e.is_alive()) {
                return;
            }

            e.destruct();
        });
        net->RegisterMessage<GameSyncEntityUpdate>(GameMessages::GAME_SYNC_ENTITY_UPDATE, [this](SLNet::RakNetGUID guid, GameSyncEntityUpdate *msg) {
            if (!msg->Valid()) {
                return;
            }

            const auto e = _worldEngine->GetEntityByServerID(msg->GetServerID());
            
            if (!e.is_alive()) {
                return;
            }

            auto tr = e.get_mut<World::Modules::Base::Transform>();
            *tr     = msg->GetTransform();

            auto es = e.get_mut<World::Modules::Base::Streamable>();
            es->owner = msg->GetOwner();
        });
        net->RegisterMessage<GameSyncEntitySelfUpdate>(GameMessages::GAME_SYNC_ENTITY_SELF_UPDATE, [this](SLNet::RakNetGUID guid, GameSyncEntitySelfUpdate *msg) {
            if (!msg->Valid()) {
                return;
            }

            const auto e = _worldEngine->GetEntityByServerID(msg->GetServerID());

            if (!e.is_alive()) {
                return;
            }

            // Nothing to do for now.
        });
    }
} // namespace Framework::Integrations::Client
