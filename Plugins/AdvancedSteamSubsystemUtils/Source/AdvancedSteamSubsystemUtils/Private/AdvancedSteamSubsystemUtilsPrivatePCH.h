// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "Sockets.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "AdvancedSteamSubsystemUtilsModule.h"

/** FName declaration of Steam subsystem before 4.11 */
#ifndef STEAM_SUBSYSTEM
#define STEAM_SUBSYSTEM FName(TEXT("STEAM"))
#endif

// disable warning in steam matchmakingtypes.h
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

#include "Runtime/Launch/Resources/Version.h"

#if !defined(UE_MAKE_VERSION)
#define UE_MAKE_VERSION(major, minor) (major*100 + minor)
#define UE_VERSION UE_MAKE_VERSION(ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION)
#endif

// STEAMSDK_FOUND was added from 4.13
#if UE_VERSION > UE_MAKE_VERSION(4, 12)

// Steamworks SDK headers
#if STEAMSDK_FOUND == 0
#error Steam SDK not located.  Expected to be found in Engine/Source/ThirdParty/Steamworks/{SteamVersion}
#endif
#endif // GS_UE_VERSION 

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

#pragma pop_macro("ARRAY_COUNT")

// @todo Steam: See above
#ifdef _MSC_VER
#pragma warning(pop)
#endif