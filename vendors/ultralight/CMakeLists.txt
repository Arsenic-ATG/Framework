add_library(Ultralight STATIC dummy.cpp)
if(WIN32)
    target_link_directories(Ultralight PUBLIC "lib/win64")
    file(GLOB SENTRY_LIBS "lib/win64/*.lib")
elseif(APPLE)
    target_link_directories(Ultralight PUBLIC "lib/macos")
    file(GLOB SENTRY_LIBS "lib/macos/*.dylib")
else()
    target_link_directories(Ultralight PUBLIC "lib/linux")
    file(GLOB SENTRY_LIBS "lib/linux/*.so")
endif()
target_link_libraries(Ultralight PUBLIC ${SENTRY_LIBS})
target_include_directories(Ultralight PUBLIC "include")
