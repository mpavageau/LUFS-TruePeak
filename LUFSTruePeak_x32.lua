-- A solution contains projects, and defines the available configurations
solution "LUFSTruePeak_x32"
    configurations { "Debug", "Release" }
	
if ( _ACTION == "vs2012" ) then
    location "build/vs2012"
elseif ( _ACTION == "xcode4" ) then
    location "build/xcode4"
else
    location "build/todo_set_platform"
end
		
    includedirs { 
        "source", 
        "extern/juce", 
        "extern/asiosdk/common", 
        "extern/vstsdk2.4", 
        "extern/vstsdk3.6", 
        "extern/aaxsdk/Interfaces", 
        "extern/aaxsdk/Interfaces/ACF", 
        }
    objdir "build/temp"
    platforms "x32"
    targetdir "build/bin"

if ( _ACTION == "xcode4" ) then
        files { 
            "source/**.h", 
            "source/**.cpp",
            "source/Info.plist",
			"source/LUFSTruePeak.icns",
            "source/**.mm",
            }
else
        files { 
            "source/**.h", 
            "source/**.cpp",
			"source/WindowsResources.rc",
			"source/LUFSTruePeak.ico",
            "extern/juce/modules/juce_audio_basics/juce_audio_basics.cpp",
            "extern/juce/modules/juce_audio_devices/juce_audio_devices.cpp",
            "extern/juce/modules/juce_audio_formats/juce_audio_formats.cpp",
            "extern/juce/modules/juce_audio_processors/juce_audio_processors.cpp",
            "extern/juce/modules/juce_audio_utils/juce_audio_utils.cpp",
            "extern/juce/modules/juce_core/juce_core.cpp",
            "extern/juce/modules/juce_cryptography/juce_cryptography.cpp",
            "extern/juce/modules/juce_data_structures/juce_data_structures.cpp",
            "extern/juce/modules/juce_events/juce_events.cpp",
            "extern/juce/modules/juce_graphics/juce_graphics.cpp",
            "extern/juce/modules/juce_gui_basics/juce_gui_basics.cpp",
            "extern/juce/modules/juce_gui_extra/juce_gui_extra.cpp",
            }
end
			
if ( _ACTION == "xcode4" ) then
    links { 
		"AppKit.framework", 
		"AudioToolBox.framework", 
		"Carbon.framework", 
		"CoreAudio.framework", 
		"CoreMIDI.framework", 
		"Foundation.framework", 
		"IOKit.framework", 
		"Quartz.framework", 
		"vecLib.framework", 
		"WebKit.framework", 
		} 
end
    
    project "LUFSTruePeak_App_x32"
        kind "WindowedApp"
        language "C++"
		
if ( _ACTION == "vs2012" ) then
		location "build/vs2012"
		flags { "WinMain", "StaticRuntime", "Unicode", "NoRuntimeChecks" }
		defines { 
			"LUFS_TRUEPEAK_WINDOWS",
			"LUFS_TRUEPEAK_APPLICATION",
			"LUFS_TRUEPEAK_USING_ASIO", -- comment this out if you don't have ASIO 
		}
elseif ( _ACTION == "xcode4" ) then
		location "build/xcode4"
		flags { "StaticRuntime", "Unicode" }
		defines { 
			"LUFS_TRUEPEAK_MACOS",
			"LUFS_TRUEPEAK_APPLICATION",
		}
else
		location "build/todo_set_platform"
		flags "flags to be set"
		defines { 
			"LUFS_TRUEPEAK_APPLICATION",
			"__DEFINES_TO_BE_ADDED_IN_LUA_CONFIG_FILE__",
		}
end
		
        configuration "Debug"
            defines {
				"DEBUG",
				"LUFS_TRUEPEAK_USING_AAX",  -- comment this out if you don't have AAX 
				"LUFS_TRUEPEAK_USING_VST3", -- comment this out if you don't have VST3
			}
            flags { "Symbols", "ExtraWarnings", }

        configuration "Release"
            defines "NDEBUG"
            flags "Optimize" 
            
    project "LUFSTruePeak_Plug_x32"
        kind "SharedLib"
        language "C++"
		
if ( _ACTION == "vs2012" ) then
		location "build/vs2012"
		flags { "Unicode", "StaticRuntime", "NoRuntimeChecks" }
		defines { 
			"LUFS_TRUEPEAK_WINDOWS",
            "LUFS_TRUEPEAK_PLUGIN",
		}
elseif ( _ACTION == "xcode4" ) then
		location "build/xcode4"
		flags { "Unicode", "StaticRuntime" }
		defines { 
			"LUFS_TRUEPEAK_MACOS",
            "LUFS_TRUEPEAK_PLUGIN",
		}
else
		location "build/todo_set_platform"
		flags "flags to be set"
		defines { 
            "LUFS_TRUEPEAK_PLUGIN",
			"__DEFINES_TO_BE_ADDED_IN_LUA_CONFIG_FILE__",
		}
end
		
if ( _ACTION == "xcode4" ) then
        files { 
            "extern/juce/modules/juce_audio_plugin_client/VST/juce_VST_Wrapper.cpp",
            "extern/juce/modules/juce_audio_plugin_client/VST/juce_VST_Wrapper.mm",
            "extern/juce/modules/juce_audio_plugin_client/VST3/juce_VST3_Wrapper.cpp",
            "extern/juce/modules/juce_audio_plugin_client/VST3/juce_VST3_Wrapper.mm",
            "extern/juce/modules/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp",
            "extern/juce/modules/juce_audio_plugin_client/AU/juce_AU_Resources.r",
            "extern/juce/modules/juce_audio_plugin_client/AU/juce_AU_Wrapper.mm",
            "extern/juce/modules/juce_audio_plugin_client/AAX/juce_AAX_Wrapper.cpp",
            "extern/juce/modules/juce_audio_plugin_client/AAX/juce_AAX_Wrapper.mm",
		}        
else
        files { 
            "extern/juce/modules/juce_audio_plugin_client/VST/juce_VST_Wrapper.cpp",
            "extern/juce/modules/juce_audio_plugin_client/VST3/juce_VST3_Wrapper.cpp",
            "extern/juce/modules/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp",
            "extern/juce/modules/juce_audio_plugin_client/AAX/juce_AAX_Wrapper.cpp",
		}        
end
        configuration "Debug"
            defines "DEBUG"
            flags "Symbols"

        configuration "Release"
            defines "NDEBUG"
            flags "Optimize" 

          
		 