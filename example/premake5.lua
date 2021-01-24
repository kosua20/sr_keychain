
-- Projects

project("Example")

	language("C")

	kind("ConsoleApp")
	systemversion("latest")

	-- Build flags
	filter("toolset:not msc*")
		buildoptions({ "-Wall", "-Wextra" })
	filter("toolset:msc*")
		buildoptions({ "-W3"})
	filter({})

	-- Common include dirs
	sysincludedirs({ "../" })
	links({"sr_keychain"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({ "Security.framework" })

	filter("system:windows")
		links({"Advapi32"})

	filter("system:linux")
		-- On Linux We have to query the dependencies for libsecret
		listing, code = os.outputof("pkg-config --libs libsecret-1")
		libsecretLibs = string.explode(string.gsub(listing, "-l", ""), " ")
		links(libsecretLibs)
		
	filter({})

	files({ "main.c" })

