
workspace("sr_keychain")

-- Configurations
	configurations({ "Release", "Debug"})
	location("build")
	targetdir ("build/%{prj.name}/%{cfg.longname}")
	debugdir ("build/%{prj.name}/%{cfg.longname}")
	architecture("x64")

	-- Configuration specific settings.
	filter("configurations:Release")
		defines({ "NDEBUG" })
		optimize("On")

	filter("configurations:Debug")
		defines({ "DEBUG" })
		symbols("On")

	filter({})
	startproject("sr_keychain")

project("sr_keychain")
	kind("StaticLib")
	systemversion("latest")
	

	filter("toolset:not msc*")
		buildoptions({ "-Wall", "-Wextra" })
	filter("toolset:msc*")
		buildoptions({ "-W3"})
	filter({})

	language("C")
		
	-- common files
	files({"sr_keychain.h", "sr_keychain.c"})
	
	-- system build filters
	filter("system:windows")

	filter("system:macosx")

	filter({"system:linux"})
		-- On Linux We have to query the dependencies for libsecret
		buildoptions({"`pkg-config --cflags libsecret-1`"})
	
	-- visual studio filters
	--filter("action:vs*")
	--	defines({ "_CRT_SECURE_NO_WARNINGS" })    

	filter({})

include("example/premake5.lua")

-- Actions

newaction {
   trigger     = "clean",
   description = "Clean the build directory",
   execute     = function ()
      print("Cleaning...")
      os.rmdir("./build")
      print("Done.")
   end
}