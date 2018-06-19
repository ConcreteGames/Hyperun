// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to retrieve all leadeboards for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamGetLeaderboard.h"

#include "AdvancedSteamLeaderboard.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "Private/OnlineLeaderboardInterfaceSteam.h"


/** Helper function to convert enums */
inline ELeaderboardSortMethod ToSteamLeaderboardSortMethod(ELeaderboardSort::Type InSortMethod)
{
	switch (InSortMethod)
	{
	case ELeaderboardSort::Ascending:
		return k_ELeaderboardSortMethodAscending;
	case ELeaderboardSort::Descending:
		return k_ELeaderboardSortMethodDescending;
	case ELeaderboardSort::None:
	default:
		return k_ELeaderboardSortMethodNone;
	}
}

/** Helper function to convert enums */
inline ELeaderboardDisplayType ToSteamLeaderboardDisplayType(ELeaderboardFormat::Type InDisplayFormat)
{
	switch (InDisplayFormat)
	{
	case ELeaderboardFormat::Seconds:
		return k_ELeaderboardDisplayTypeTimeSeconds;
	case ELeaderboardFormat::Milliseconds:
		return k_ELeaderboardDisplayTypeTimeMilliSeconds;
	case ELeaderboardFormat::Number:
	default:
		return k_ELeaderboardDisplayTypeNumeric;
	}
}

/** Helper function to convert enums */
inline ELeaderboardFormat::Type FromSteamLeaderboardDisplayType(ELeaderboardDisplayType InDisplayFormat)
{
	switch (InDisplayFormat)
	{
	case k_ELeaderboardDisplayTypeTimeSeconds:
		return ELeaderboardFormat::Seconds;
	case k_ELeaderboardDisplayTypeTimeMilliSeconds:
		return ELeaderboardFormat::Milliseconds;
	case k_ELeaderboardDisplayTypeNumeric:
	default:
		return ELeaderboardFormat::Number;
	}
}

/** Helper function to convert enums */
inline ELeaderboardSort::Type FromSteamLeaderboardSortMethod(ELeaderboardSortMethod InSortMethod)
{
	switch (InSortMethod)
	{
	case k_ELeaderboardSortMethodAscending:
		return ELeaderboardSort::Ascending;
	case  k_ELeaderboardSortMethodDescending:
		return ELeaderboardSort::Descending;
	case k_ELeaderboardSortMethodNone:
	default:
		return ELeaderboardSort::None;
	}
}

FOnlineAsyncTaskSteamGetLeaderboard::FOnlineAsyncTaskSteamGetLeaderboard(
	FOnlineSubsystemSteam* InSteamSubsystem, 
	const FName& InLeaderboardName, 
	ELeaderboardSort::Type InSortMethod, 
	ELeaderboardFormat::Type InDisplayFormat, 
	FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr)
	: 
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	LeaderboardName(InLeaderboardName),
	SortMethod(InSortMethod),
	DisplayFormat(InDisplayFormat),
	bFindOnly(false),
	AdvancedSteamLeaderboardPtr(InAdvancedSteamLeaderboardPtr)
{
}

/** Find a leaderboard implementation */
FOnlineAsyncTaskSteamGetLeaderboard::FOnlineAsyncTaskSteamGetLeaderboard(
	FOnlineSubsystemSteam* InSteamSubsystem, 
	const FName& InLeaderboardName, 
	FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	LeaderboardName(InLeaderboardName),
	SortMethod(ELeaderboardSort::Ascending),
	DisplayFormat(ELeaderboardFormat::Number),
	bFindOnly(true),
	AdvancedSteamLeaderboardPtr(InAdvancedSteamLeaderboardPtr)
{
}

void FOnlineAsyncTaskSteamGetLeaderboard::CreateOrFindLeaderboard(const FName& InLeaderboardName, ELeaderboardSort::Type InSortMethod, ELeaderboardFormat::Type InDisplayFormat)
{
	if (bFindOnly)
	{
		CallbackHandle = SteamUserStats()->FindLeaderboard(TCHAR_TO_UTF8(*InLeaderboardName.ToString()));
	}
	else
	{
		ELeaderboardSortMethod SortMethodSteam = ToSteamLeaderboardSortMethod(InSortMethod);
		ELeaderboardDisplayType DisplayTypeSteam = ToSteamLeaderboardDisplayType(InDisplayFormat);
		CallbackHandle = SteamUserStats()->FindOrCreateLeaderboard(TCHAR_TO_UTF8(*InLeaderboardName.ToString()), SortMethodSteam, DisplayTypeSteam);
	}
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamGetLeaderboard::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamGetLeaderboard bWasSuccessful: %d"), bWasSuccessful);
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamGetLeaderboard::Tick()
{
	ISteamUtils* SteamUtilsPtr = SteamUtils();
	check(SteamUtilsPtr);

	if (!bInit)
	{
		CreateOrFindLeaderboard(LeaderboardName, SortMethod, DisplayFormat);
		bInit = true;
	}

	if (CallbackHandle != k_uAPICallInvalid)
	{
		bool bFailedCall = false;

		// Poll for completion status
		bIsComplete = SteamUtilsPtr->IsAPICallCompleted(CallbackHandle, &bFailedCall) ? true : false;
		if (bIsComplete)
		{
			bool bFailedResult;
			// Retrieve the callback data from the request
			bool bSuccessCallResult = SteamUtilsPtr->GetAPICallResult(CallbackHandle, &CallbackResults, sizeof(CallbackResults), CallbackResults.k_iCallback, &bFailedResult);
			bWasSuccessful = (bSuccessCallResult ? true : false) &&
				(!bFailedCall ? true : false) &&
				(!bFailedResult ? true : false) &&
				((CallbackResults.m_bLeaderboardFound != 0) ? true : false);
		}
	}
	else
	{
		// Invalid API call
		bIsComplete = true;
		bWasSuccessful = false;
	}
}

/**
* Give the async task a chance to marshal its data back to the game thread
* Can only be called on the game thread by the async task manager
*/
void FOnlineAsyncTaskSteamGetLeaderboard::Finalize()
{
	FOnlineAsyncTaskSteam::Finalize();

#if !WITH_EDITOR
	// Copy the leaderboard handle into the array of data
	FLeaderboardMetadata* Leaderboard = AdvancedSteamLeaderboardPtr->GetLeaderboardMetadata(LeaderboardName);

	if (bWasSuccessful)
	{
		ISteamUserStats* SteamUserPtr = SteamUserStats();
		check(LeaderboardName.ToString() == FString(SteamUserPtr->GetLeaderboardName(CallbackResults.m_hSteamLeaderboard)));

		Leaderboard->LeaderboardHandle = CallbackResults.m_hSteamLeaderboard;
		Leaderboard->TotalLeaderboardRows = SteamUserPtr->GetLeaderboardEntryCount(CallbackResults.m_hSteamLeaderboard);
		Leaderboard->DisplayFormat = FromSteamLeaderboardDisplayType(SteamUserPtr->GetLeaderboardDisplayType(CallbackResults.m_hSteamLeaderboard));
		Leaderboard->SortMethod = FromSteamLeaderboardSortMethod(SteamUserPtr->GetLeaderboardSortMethod(CallbackResults.m_hSteamLeaderboard));
		Leaderboard->AsyncState = EOnlineAsyncTaskState::Done;
	}
	else
	{
		Leaderboard->LeaderboardHandle = -1;
		Leaderboard->TotalLeaderboardRows = 0;
		Leaderboard->AsyncState = EOnlineAsyncTaskState::Failed;
	}
#endif
}

