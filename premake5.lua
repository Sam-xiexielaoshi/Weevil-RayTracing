-- premake5.lua
workspace "Weevil"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Weevil"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Weevil"