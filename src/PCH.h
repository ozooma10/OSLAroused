#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>


// spdlog.h provides the free-function API (set_level/set_pattern/default_logger/
// set_default_logger) used by Config.cpp and Main.cpp. The sink headers are
// needed by InitializeLogging() in Main.cpp; commonlib uses spdlog internally
// but does not expose any of these in its public headers.
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// Compatible declarations with other sample projects.
#define DLLEXPORT __declspec(dllexport)

using namespace std::literals;

