-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("SeamlessSaving")
set_license("MIT")

local version = "0.0.1"
local ver = version:split("%.")
set_version(version)

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.release", "mode.releasedbg")
set_defaultmode("releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- add packages
add_requires("minhook")
add_requires("boost", { configs = { filesystem = false } })

-- targets
target("SeamlessSaving")
-- add dependencies to target
add_deps("commonlibsse-ng")
add_packages("minhook", "boost")

-- unicode (matches cmake preset flags -DUNICODE -D_UNICODE)
add_defines("UNICODE", "_UNICODE")

-- set DLL output name
set_basename("seamless-saving")

-- generate PDB (releasedbg handles /Zi; /DEBUG tells linker to emit PDB for the DLL)
add_shflags("/DEBUG", { force = true })

-- version config vars
set_configvar("VERSION_MAJOR", tonumber(ver[1]))
set_configvar("VERSION_MINOR", tonumber(ver[2]))
set_configvar("VERSION_PATCH", tonumber(ver[3]))
set_configvar("VERSION_STRING", version)

-- add commonlibsse-ng plugin
add_rules("commonlibsse-ng.plugin", {
    name = "SeamlessSaving",
    author = "JerryYOJ",
    description = "Seamless Saving SKSE Plugin",
})

-- add src files
add_files("src/**.cpp")
add_headerfiles("src/**.h")
add_includedirs("src")
set_pcxxheader("src/pch.h")
add_configfiles("src/Version.h.in")

-- auto deploy
-- SkyrimPluginTargets: semicolon-separated list of mod/data dirs (xmake-style)
-- SKYRIM_MODS_FOLDER:  MO2/Vortex mods folder; deploys to <SKYRIM_MODS_FOLDER>/SeamlessSaving (cmake-style)
-- SKYRIM_FOLDER:       Skyrim install dir; deploys to <SKYRIM_FOLDER>/Data (cmake-style)
after_build(function(target)
    local dirs = {}

    local plugin_targets = os.getenv("SkyrimPluginTargets")
    if plugin_targets then
        for _, dir in ipairs(plugin_targets:split(";")) do
            dir = dir:trim()
            if dir ~= "" then
                table.insert(dirs, dir)
            end
        end
    end

    local mods_folder = os.getenv("SKYRIM_MODS_FOLDER")
    if mods_folder and os.isdir(mods_folder) then
        table.insert(dirs, path.join(mods_folder, "Seamless Saving"))
    end

    local skyrim_folder = os.getenv("SKYRIM_FOLDER")
    if skyrim_folder and os.isdir(path.join(skyrim_folder, "Data")) then
        table.insert(dirs, path.join(skyrim_folder, "Data"))
    end

    if #dirs == 0 then
        return
    end

    local dll = target:targetfile()
    local pdb = target:symbolfile()
    for _, dir in ipairs(dirs) do
        local dest = path.join(dir, "SKSE", "Plugins")
        os.mkdir(dest)
        os.cp(dll, dest)
        if os.isfile(pdb) then
            os.cp(pdb, dest)
        end
        print("Deployed to " .. dest)
    end
end)
