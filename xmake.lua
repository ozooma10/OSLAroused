-- include subprojects
includes("lib/commonlibsse")

-- set project constants
set_project("OSLAroused")
set_version("2.8.2")
set_license("Unlicense")
set_languages("c++23")
set_warnings("allextra")
set_encodings("utf-8")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- silence the MSVC warnings that the previous CMake build also suppressed
if is_plat("windows") then
    add_defines("_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX", "UNICODE", "_UNICODE")
    add_cxxflags("cl::/permissive-", "cl::/Zc:preprocessor", "cl::/EHsc")
    add_cxxflags("cl::/wd4251", "cl::/wd4275", "cl::/wd4267", "cl::/wd4244", "cl::/wd4996")
end

-- build option for the Catch2 test suite
option("build_tests", function()
    set_default(false)
    set_description("Build the Catch2 unit/integration/e2e tests.")
end)

-- deploy the built plugin to contrib/Distribution and any Mod Organizer 2 targets
-- (preserves the workflow the old CMakeLists.txt provided)
rule("oslaroused.deploy", function()
    after_build(function(target)
        local buildname = is_mode("debug") and "Debug" or "Release"
        local distdir = path.join(os.projectdir(), "contrib", "Distribution", "Plugin" .. buildname)
        os.mkdir(distdir)
        os.cp(target:targetfile(), distdir)
        if os.isfile(target:symbolfile()) then
            os.cp(target:symbolfile(), distdir)
        end

        -- semicolon-separated list of MO2 mod folders, same env var as the old build
        local targets = os.getenv("SkyrimPluginTargets")
        if targets then
            for _, dir in ipairs(path.splitenv(targets)) do
                local plugindir = path.join(dir, "SKSE", "Plugins")
                os.mkdir(plugindir)
                os.cp(target:targetfile(), plugindir)
                if os.isfile(target:symbolfile()) then
                    os.cp(target:symbolfile(), plugindir)
                end
            end
        end
    end)
end)

-- the SKSE plugin
target("OSLAroused", function()
    add_rules("commonlibsse.plugin", {
        name = "OSLAroused",
        author = "ozooma10",
        description = "High-Performance Arousal Framework. Support for both Sexlab and OStim."
    })
    add_rules("oslaroused.deploy")

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")
end)

-- the test suite (configure with `xmake f --build_tests=y`)
if has_config("build_tests") then
    add_requires("catch2")

    target("OSLArousedTests", function()
        set_kind("binary")
        set_default(false)

        -- link against the same CommonLibSSE static library as the plugin
        add_deps("commonlibsse")
        add_packages("catch2")

        -- all plugin sources plus the test sources, matching the old CMake test target
        add_files("src/**.cpp")
        add_files("test/**.cpp")
        add_includedirs("src")
        set_pcxxheader("src/PCH.h")
    end)
end
