// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;
using System;

public class Drift : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(ModuleDirectory); }
    }

    public Drift(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "EngineSettings",
				// ... add private dependencies that you statically link with here ...	
			}
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "NetworkReplayStreaming"
            }
        );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }

        /*
        string ExternalLibrariesPath = Path.GetFullPath(Path.Combine(ModulePath, "../Plugins/"));
        string FreeImagePath = Path.Combine(ExternalLibrariesPath, "FreeImage");

        Log.TraceError("FreeImagePath: ");
        Log.TraceError(FreeImagePath);

        if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            PublicIncludePaths.Add(Path.Combine(FreeImagePath, "x32"));
            PublicAdditionalLibraries.Add(Path.Combine(FreeImagePath, "x32", "FreeImage.lib"));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(FreeImagePath, "x32", "FreeImage.dll")));
            PublicDelayLoadDLLs.Add("FreeImage.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(FreeImagePath, "x64"));
            PublicAdditionalLibraries.Add(Path.Combine(FreeImagePath, "x64", "FreeImage.lib"));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(FreeImagePath, "x64", "FreeImage.dll")));
            PublicDelayLoadDLLs.Add("FreeImage.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            //PublicIncludePaths.Add(Path.Combine(FreeImagePath, "x64"));
            //PublicAdditionalLibraries.Add(Path.Combine(AssimpPath, "lib", "libassimp.3.1.1.dylib"));
        }

        
        string ImageMagickPath = Path.Combine(ExternalLibrariesPath, "ImageMagick");

        PublicIncludePaths.Add(Path.Combine(ImageMagickPath, "include"));

        PublicAdditionalLibraries.Add(Path.Combine(ImageMagickPath, "lib", "CORE_RL_Magick++_.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ImageMagickPath, "lib", "CORE_RL_MagickCore_.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ImageMagickPath, "lib", "CORE_RL_MagickWand_.lib"));

        if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
        {
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_bzlib_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_cairo_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_exr_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_flif_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_glib_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_jp2_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_jpeg_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_lcms_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_libraw_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_librsvg_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_libxml_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_lqr_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_Magick++_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_MagickCore_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_MagickWand_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_openjpeg_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_pango_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_png_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_tiff_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_ttf_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_webp_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "CORE_RL_zlib_.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "mfc120u.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "msvcp120.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "msvcr120.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ImageMagickPath, "vcomp120.dll")));

            PublicDelayLoadDLLs.Add("CORE_RL_bzlib_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_cairo_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_exr_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_flif_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_glib_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_jp2_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_jpeg_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_lcms_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_libraw_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_librsvg_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_libxml_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_lqr_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_Magick++_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_MagickCore_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_MagickWand_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_openjpeg_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_pango_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_png_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_tiff_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_ttf_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_webp_.dll");
            PublicDelayLoadDLLs.Add("CORE_RL_zlib_.dll");
            PublicDelayLoadDLLs.Add("mfc120u.dll");
            PublicDelayLoadDLLs.Add("msvcp120.dll");
            PublicDelayLoadDLLs.Add("msvcr120.dll");
            PublicDelayLoadDLLs.Add("vcomp120.dll");
        }*/
    }
}
