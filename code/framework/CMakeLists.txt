include(UpdateGitHash)
update_git_version(FW "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

# List the source files
set(FRAMEWORK_SRC
    src/logging/logger.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    src/utils/delay_scope.cpp
    src/utils/config.cpp
    src/utils/interpolator.cpp
    src/utils/time.cpp
    src/utils/job_system.cpp
    src/utils/command_listener.cpp
    src/utils/command_processor.cpp
    src/utils/hashing.cpp
    src/utils/states/machine.cpp

    src/external/sentry/wrapper.cpp
    src/external/firebase/wrapper.cpp
    src/external/optick/wrapper.cpp

    src/world/engine.cpp
    src/world/modules/modules_impl.cpp

    src/networking/network_peer.cpp
    src/networking/errors.cpp
    )

set(FRAMEWORK_SERVER_SRC
    src/scripting/sdk.cpp
    src/scripting/engine.cpp
    src/scripting/events.cpp
    src/scripting/resource.cpp
    src/scripting/resource_manager.cpp

    src/scripting/builtins/factory.cpp

    src/scripting/v8_helpers/argstack.cpp
    src/scripting/v8_helpers/v8_class.cpp
    src/scripting/v8_helpers/v8_module.cpp
    src/scripting/v8_helpers/v8_source_location.cpp
    src/scripting/v8_helpers/v8_try_catch.cpp

    src/http/webserver.cpp

    src/services/masterlist.cpp
    src/services/user.cpp

    src/networking/network_server.cpp

    src/world/server.cpp

    src/integrations/server/instance.cpp
    src/integrations/server/masterlist.cpp
    src/integrations/server/networking/engine.cpp
    src/integrations/server/scripting/server.cpp
    )

set(FRAMEWORK_CLIENT_SRC
    src/graphics/renderer.cpp
    src/graphics/renderio.cpp

    src/networking/network_client.cpp

    src/external/steam/wrapper.cpp
    src/external/discord/wrapper.cpp
    src/external/imgui/wrapper.cpp
    src/external/sdl2/wrapper.cpp

    src/world/client.cpp

    src/external/imgui/widgets/console.cpp

    src/integrations/client/instance.cpp
    src/integrations/client/networking/engine.cpp
)

# Append the platform-dependent files to the related lists
if(WIN32)
    list(APPEND FRAMEWORK_CLIENT_SRC 
        src/graphics/backend/d3d9.cpp
        src/graphics/backend/d3d11.cpp
        src/utils/hooking/hook_function.cpp
        src/utils/hooking/hooking_patterns.cpp
        src/utils/hooking/hooking.cpp
        src/utils/memory.cpp
        src/utils/minidump.cpp
    )
endif()

# Additional libraries
find_package(OpenSSL REQUIRED)

# Build depending on the platform
add_library(Framework STATIC ${FRAMEWORK_SRC})
if(WIN32)
    # Build the loader and it's data
    add_library(FrameworkLoader
        STATIC
        src/utils/hashing.cpp
        src/launcher/project.cpp
        src/utils/minidump.cpp
        src/launcher/loaders/exe_ldr.cpp
        src/external/steam/wrapper.cpp
        src/utils/hooking/hook_function.cpp
        src/utils/hooking/hooking_patterns.cpp
        src/utils/hooking/hooking.cpp
        src/utils/memory.cpp)
    target_link_options(FrameworkLoader PUBLIC /IGNORE:4254 /SAFESEH:NO /DYNAMICBASE:NO /LARGEADDRESSAWARE /SUBSYSTEM:windows /SECTIONLAYOUT:@${CMAKE_CURRENT_SOURCE_DIR}/src/launcher/SectionLayout.txt)

    if (CMAKE_CL_64)
        target_link_options(FrameworkLoader PUBLIC /BASE:0x140000000)
    else()
        target_link_options(FrameworkLoader PUBLIC /BASE:0x400000)
    endif()

    add_library(FrameworkLoaderData
        SHARED 
        src/launcher/data/tls.cpp
        src/launcher/data/seh_win32.cpp)

    # Build the client
    add_library(FrameworkClient STATIC ${FRAMEWORK_CLIENT_SRC})
endif()
add_library(FrameworkServer STATIC ${FRAMEWORK_SERVER_SRC})

# Custom defines
target_compile_definitions(Framework PUBLIC MG_ENABLE_LOG=0)
IF(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
ENDIF()

# External libraries & other stuff
macro(link_shared_deps target_name)
    target_include_directories(${target_name} PUBLIC "src")

    # Required libraries for apple systems
    if(APPLE)
        target_link_libraries(${target_name} "-framework CoreFoundation" "-framework Foundation" "-framework GSS" "-framework Security" "-framework CoreText" "-framework CoreGraphics" bsm)
    endif()

    # Global libraries
    target_link_libraries(${target_name} slikenet glm spdlog cppfs nlohmann_json Sentry httplib ${OPENSSL_LIBRARIES} Firebase Curl OptickCore flecs_static semver)

    # Required libraries for windows
    if (WIN32)
        target_link_libraries(${target_name} StackWalker)
    endif()
endmacro()

# Link the solutions based on the platform
link_shared_deps(Framework)
if(WIN32)
    link_shared_deps(FrameworkClient)
endif()
link_shared_deps(FrameworkServer)
target_link_libraries(FrameworkServer mongoose NodeJS libsig v8pp)

# Platform-dependent post processing
if (WIN32)
    target_link_directories(Framework PUBLIC ${CMAKE_SOURCE_DIR}/vendors/openssl/lib)
    target_link_libraries(Framework ws2_32 dbghelp crypt32 winmm iphlpapi psapi userenv)

    target_link_libraries(FrameworkClient DiscordSDK DearImGUI SDL)

    set(CLIENT_SHARED_LIBS minhook GalaxySDK SteamSDK udis86)
    target_link_libraries(FrameworkClient ${CLIENT_SHARED_LIBS} RmlCore ${FREETYPE_LIBRARY})

    link_shared_deps(FrameworkLoader)
    target_link_libraries(FrameworkLoader ${CLIENT_SHARED_LIBS} FrameworkLoaderData ntdll sfd)
    target_link_libraries(FrameworkLoaderData udis86 minhook)
    target_link_options(FrameworkLoaderData PUBLIC /EXPORT:CoreRT_SetupSEHHandler /EXPORT:GetThreadLocalStorage /EXPORT:CoreSetExceptionOverride)

    if (NOT CMAKE_CL_64)
        target_link_options(FrameworkServer PUBLIC "-SAFESEH:NO")
    else ()
        target_link_libraries(FrameworkClient Ultralight)
        target_link_options(FrameworkClient PUBLIC "-SAFESEH:NO")
    endif ()
endif ()
