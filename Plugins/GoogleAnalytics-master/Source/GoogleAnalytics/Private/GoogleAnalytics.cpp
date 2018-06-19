// Google Analytics Provider
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#include "GoogleAnalyticsPrivatePCH.h"
#include "Runtime/Core/Public/Misc/SecureHash.h"
#include <string>

#if PLATFORM_IOS
#import <GoogleAnalytics/GAI.h>
#import <GoogleAnalytics/GAIFields.h>
#import <GoogleAnalytics/GAIDictionaryBuilder.h>
#elif PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include <android_native_app_glue.h>
#endif


DEFINE_LOG_CATEGORY_STATIC(LogAnalytics, Display, All);

#define LOCTEXT_NAMESPACE "GoogleAnalytics"

IMPLEMENT_MODULE(FAnalyticsGoogleAnalytics, GoogleAnalytics)

TSharedPtr<IAnalyticsProvider> FAnalyticsProviderGoogleAnalytics::Provider;


#if PLATFORM_ANDROID
void AndroidThunkCpp_GoogleAnalyticsStartSession(FString& TrackingId, int32& DispatchInterval)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring TrackingIdFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*TrackingId));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsStartSession", "(Ljava/lang/String;I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, TrackingIdFinal, DispatchInterval);
		Env->DeleteLocalRef(TrackingIdFinal);
	}
}

void AndroidThunkCpp_GoogleAnalyticsRecordScreen(const FString& ScreenName)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring ScreenNameFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*ScreenName));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsRecordScreen", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, ScreenNameFinal);
		Env->DeleteLocalRef(ScreenNameFinal);
	}
}

void AndroidThunkCpp_GoogleAnalyticsRecordEvent(const FString& Category, const FString& Action, const FString& Label, const int32& Value)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring CategoryFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*Category));
		jstring ActionFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*Action));
		jstring LabelFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*Label));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsRecordEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, CategoryFinal, ActionFinal, LabelFinal, Value);
		Env->DeleteLocalRef(CategoryFinal);
		Env->DeleteLocalRef(ActionFinal);
		Env->DeleteLocalRef(LabelFinal);
	}
}

void AndroidThunkCpp_GoogleAnalyticsFlushEvents()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsFlushEvents", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

void AndroidThunkCpp_GoogleAnalyticsSetUserId(const FString& UserId) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring UserIdFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*UserId));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsSetUserId", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, UserIdFinal);
		Env->DeleteLocalRef(UserIdFinal);
	}
}

FString AndroidThunkCpp_GoogleAnalyticsGetUserId() {
	FString ResultUserId = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsGetUserId", "()Ljava/lang/String;", false);
		jstring UserIdString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
		const char *nativeUserIdString = Env->GetStringUTFChars(UserIdString, 0);
		ResultUserId = FString(nativeUserIdString);
		Env->ReleaseStringUTFChars(UserIdString, nativeUserIdString);
		Env->DeleteLocalRef(UserIdString);
	}
	return ResultUserId;
}

void AndroidThunkCpp_GoogleAnalyticsSetLocation(const FString& Location) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring LocationFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*Location));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsSetLocation", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, LocationFinal);
		Env->DeleteLocalRef(LocationFinal);
	}
}

void AndroidThunkCpp_GoogleAnalyticsRecordError(const FString& Description) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring DescriptionFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*Description));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsRecordError", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, DescriptionFinal);
		Env->DeleteLocalRef(DescriptionFinal);
	}
}

void AndroidThunkCpp_GoogleAnalyticsRecordCurrencyPurchase(const FString& TransactionId, const FString& GameCurrencyType, const int32& GameCurrencyAmount, const FString& RealCurrencyType, const float& RealMoneyCost, const FString& PaymentProvider) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring TransactionIdFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*TransactionId));
		jstring GameCurrencyTypeFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*GameCurrencyType));
		jstring RealCurrencyTypeFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*RealCurrencyType));
		jstring PaymentProviderFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*PaymentProvider));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleAnalyticsRecordCurrencyPurchase", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;FLjava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, TransactionIdFinal, GameCurrencyTypeFinal, GameCurrencyAmount, RealCurrencyTypeFinal, RealMoneyCost, PaymentProviderFinal);
		Env->DeleteLocalRef(TransactionIdFinal);
		Env->DeleteLocalRef(GameCurrencyTypeFinal);
		Env->DeleteLocalRef(RealCurrencyTypeFinal);
		Env->DeleteLocalRef(PaymentProviderFinal);
	}
}

#endif


void FAnalyticsGoogleAnalytics::StartupModule()
{
}

void FAnalyticsGoogleAnalytics::ShutdownModule()
{
	FAnalyticsProviderGoogleAnalytics::Destroy();
}

TSharedPtr<IAnalyticsProvider> FAnalyticsGoogleAnalytics::CreateAnalyticsProvider(const FAnalyticsProviderConfigurationDelegate& GetConfigValue) const
{
	if (GetConfigValue.IsBound())
	{
		FString TrackingId = FString("");
#if PLATFORM_IOS
		TrackingId = GetConfigValue.Execute(TEXT("TrackingIdIOS"), true);
#elif PLATFORM_ANDROID
		TrackingId = GetConfigValue.Execute(TEXT("TrackingIdAndroid"), true);
#else
		TrackingId = GetConfigValue.Execute(TEXT("TrackingIdUniversal"), true);
#endif
		const FString SendInterval = GetConfigValue.Execute(TEXT("SendInterval"), false);
		return FAnalyticsProviderGoogleAnalytics::Create(TrackingId, FCString::Atoi(*SendInterval));
	}
	else
	{
		UE_LOG(LogAnalytics, Warning, TEXT("GoogleAnalytics::CreateAnalyticsProvider called with an unbound config delegate"));
	}
	return nullptr;
}

// Provider
FAnalyticsProviderGoogleAnalytics::FAnalyticsProviderGoogleAnalytics(const FString TrackingId, const int32 SendInterval) :
	ApiTrackingId(TrackingId),
	bHasSessionStarted(false),
	bSessionStartedSent(false),
	Interval(SendInterval)
{
}

FAnalyticsProviderGoogleAnalytics::~FAnalyticsProviderGoogleAnalytics()
{
	if (bHasSessionStarted)
	{
		EndSession();
	}
}

bool FAnalyticsProviderGoogleAnalytics::StartSession(const TArray<FAnalyticsEventAttribute>& Attributes)
{
	if (!bHasSessionStarted && ApiTrackingId.Len() > 0)
	{
#if PLATFORM_IOS
		if (Interval > 0)
		{
			[[GAI sharedInstance] setDispatchInterval:Interval];
		}
		[[GAI sharedInstance] setTrackUncaughtExceptions:true];
		id<GAITracker> tracker = [[GAI sharedInstance] trackerWithName:@"DefaultTracker" trackingId : ApiTrackingId.GetNSString()];
		if (tracker != nil)
		{
			tracker.allowIDFACollection = YES;
		}
#elif PLATFORM_ANDROID
		AndroidThunkCpp_GoogleAnalyticsStartSession(ApiTrackingId, Interval);
#else
		FString UniversalCidBufor("");
		GConfig->GetString(TEXT("GoogleAnalytics"), TEXT("UniversalCid"), UniversalCidBufor, GEngineIni);

		if (UniversalCidBufor.Len() > 0)
		{
			UniversalCid = UniversalCidBufor;
		}
		else
		{
			UniversalCid = FMD5::HashAnsiString(*("UniversalCid" + FDateTime::Now().ToString()));
		}

		GConfig->SetString(TEXT("GoogleAnalytics"), TEXT("UniversalCid"), *UniversalCid, GEngineIni);
#endif
		if (Attributes.Num() > 0)
		{
			RecordEvent(TEXT("SessionAttributes"), Attributes);
		}
		bHasSessionStarted = true;
	}

	return bHasSessionStarted;
}

void FAnalyticsProviderGoogleAnalytics::EndSession()
{
	if (bHasSessionStarted)
	{
#if PLATFORM_IOS
		[[GAI sharedInstance] removeTrackerByName:@"DefaultTracker"];
#endif
		bHasSessionStarted = false;
		bSessionStartedSent = false;
	}
}

void FAnalyticsProviderGoogleAnalytics::SetTrackingId(const FString& TrackingId)
{
	const TArray<FAnalyticsEventAttribute> Attributes;
	EndSession();
	ApiTrackingId = TrackingId;
	StartSession(Attributes);
}

FString FAnalyticsProviderGoogleAnalytics::GetTrackingId()
{
	return ApiTrackingId;
}

FString FAnalyticsProviderGoogleAnalytics::GetSystemInfo()
{
	FString SystemInfo = FString("");

	if (GEngine->GameViewport)
	{
		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		SystemInfo += FString("&ul=" + FInternationalization::Get().GetCurrentCulture()->GetName() + "&ua=Windows&sr=" + FString::FromInt(ViewportSize.X) + "x" + FString::FromInt(ViewportSize.Y) + "&vp=" + FString::FromInt(ViewportSize.X) + "x" + FString::FromInt(ViewportSize.Y));
	}

	if (!bSessionStartedSent)
	{
		bSessionStartedSent = true;
		SystemInfo += "&sc=start";
	}

	return SystemInfo;
}

void FAnalyticsProviderGoogleAnalytics::FlushEvents()
{
	if (bHasSessionStarted)
	{
#if PLATFORM_IOS
		[[GAI sharedInstance] dispatch];
#elif PLATFORM_ANDROID
		AndroidThunkCpp_GoogleAnalyticsFlushEvents();
#endif
	}
}

void FAnalyticsProviderGoogleAnalytics::SetUserID(const FString& InUserId)
{
	if (bHasSessionStarted)
	{
#if PLATFORM_IOS
		id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

		if (tracker != nil)
		{
			[tracker set : kGAIUserId value : InUserId.GetNSString()];
		}
#elif PLATFORM_ANDROID
		AndroidThunkCpp_GoogleAnalyticsSetUserId(InUserId);
#else
		UserId = InUserId;
#endif
	}
}

FString FAnalyticsProviderGoogleAnalytics::GetUserID() const
{
	if (bHasSessionStarted)
	{
#if PLATFORM_IOS
		id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

		if (tracker)
		{
			return FString([tracker get : kGAIUserId]);
		}
		else
		{
			return FString();
		}
#elif PLATFORM_ANDROID
		return AndroidThunkCpp_GoogleAnalyticsGetUserId();
#else
		return UserId;
#endif
	}
	return FString("");
}

void FAnalyticsProviderGoogleAnalytics::SetGender(const FString& InGender)
{
	if (bHasSessionStarted)
	{
		TArray<FAnalyticsEventAttribute> Params;
		Params.Add(FAnalyticsEventAttribute(TEXT("Category"), TEXT("Gender")));
		RecordEvent(InGender, Params);
	}
}

void FAnalyticsProviderGoogleAnalytics::SetAge(const int32 InAge)
{
	if (bHasSessionStarted)
	{
		TArray<FAnalyticsEventAttribute> Params;
		Params.Add(FAnalyticsEventAttribute(TEXT("Category"), TEXT("Age")));
		Params.Add(FAnalyticsEventAttribute(TEXT("Value"), InAge));
		RecordEvent(FString::FromInt(InAge), Params);
	}
}

void FAnalyticsProviderGoogleAnalytics::SetLocation(const FString& InLocation)
{
	if (bHasSessionStarted)
	{
#if PLATFORM_IOS
		id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

		if (tracker != nil)
		{
			[tracker set : kGAILocation value : InLocation.GetNSString()];
		}
#elif PLATFORM_ANDROID
		AndroidThunkCpp_GoogleAnalyticsSetLocation(InLocation);
#else 
		Location = InLocation;
#endif
	}
}

FString FAnalyticsProviderGoogleAnalytics::GetSessionID() const
{
	// Ignored
	UE_LOG(LogAnalytics, Display, TEXT("FAnalyticsProviderGoogleAnalytics::GetSessionID - ignoring call"));

	return FString();
}

bool FAnalyticsProviderGoogleAnalytics::SetSessionID(const FString& InSessionID)
{
	// Ignored
	UE_LOG(LogAnalytics, Display, TEXT("FAnalyticsProviderGoogleAnalytics::SetSessionID - ignoring call"));

	return true;
}

FString EncodeStringsForHTML(FString ToBeEncoded)
{
	ToBeEncoded = ToBeEncoded.Replace(TEXT(" "), TEXT("%20"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("&"), TEXT("%26"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("#"), TEXT("%23"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("\""), TEXT("%22"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("?"), TEXT("%3F"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("@"), TEXT("%40"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT(":"), TEXT("%3A"), ESearchCase::IgnoreCase);
	ToBeEncoded = ToBeEncoded.Replace(TEXT("_"), TEXT("%5F"), ESearchCase::IgnoreCase);

	return ToBeEncoded;
}

void FAnalyticsProviderGoogleAnalytics::RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes)
{
	if (bHasSessionStarted)
	{
		if (EventName.Len() > 0)
		{
			FString Action = FString(EventName);
			FString Category = FString("Default Category");
			FString Label = FString("");
			float Value = 0;

			for (int i = 0; i < Attributes.Num(); i++)
			{
				if (Attributes[i].AttrName.Equals("Category") && Attributes[i].AttrValue.Len() > 0)
				{
					Category = Attributes[i].AttrValue;
				}
				else if (Attributes[i].AttrName.Equals("Label") && Attributes[i].AttrValue.Len() > 0)
				{
					Label = Attributes[i].AttrValue;
				}
				else if (Attributes[i].AttrName.Equals("Value"))
				{
					Value = FCString::Atof(*Attributes[i].AttrValue);
				}
			}

#if PLATFORM_IOS
			NSString* EventLabel = Label.Len() > 0 ? Label.GetNSString() : nil;
			id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

			if (tracker != nil)
			{
				[tracker send : [[GAIDictionaryBuilder createEventWithCategory : Category.GetNSString()
					action : EventName.GetNSString()
					label : EventLabel
					value : @(Value)] build]];
			}
#elif PLATFORM_ANDROID
			AndroidThunkCpp_GoogleAnalyticsRecordEvent(Category, EventName, Label, Value);
#else 
			TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
			HttpRequest->SetURL("https://www.google-analytics.com/collect?v=1&t=event&tid=" + ApiTrackingId + "&cid=" + UniversalCid + "&ec=" + EncodeStringsForHTML(Category) + "&ea=" + EncodeStringsForHTML(Action) + "&el=" + EncodeStringsForHTML(Label) + "&ev=" + FString::FromInt(Value) + "&geoid=" + Location + "&uid=" + UserId + GetSystemInfo());
			HttpRequest->SetVerb("GET");
			HttpRequest->ProcessRequest();
#endif
		}
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordScreen(const FString& ScreenName)
{
	if (bHasSessionStarted)
	{
		if (ScreenName.Len() > 0)
		{
#if PLATFORM_IOS
			id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

			if (tracker != nil)
			{
				[tracker set : kGAIScreenName value : ScreenName.GetNSString()];
				[tracker send : [[GAIDictionaryBuilder createScreenView] build]];
			}
#elif PLATFORM_ANDROID
			AndroidThunkCpp_GoogleAnalyticsRecordScreen(ScreenName);
#else
			TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
			HttpRequest->SetURL("https://www.google-analytics.com/collect?v=1&t=pageview&tid=" + ApiTrackingId + "&cid=" + UniversalCid + "&dp=" + EncodeStringsForHTML(ScreenName) + "&geoid=" + Location + "&uid=" + UserId + GetSystemInfo());
			HttpRequest->SetVerb("GET");
			HttpRequest->ProcessRequest();
#endif
		}
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordItemPurchase(const FString& ItemId, int ItemQuantity, const TArray<FAnalyticsEventAttribute>& EventAttrs)
{
	if (bHasSessionStarted)
	{
		FString Currency = FString("");
		int32 PerItemCost = 0;

		for (int i = 0; i < EventAttrs.Num(); i++)
		{
			if (EventAttrs[i].AttrName.Equals("Currency") && EventAttrs[i].AttrValue.Len() > 0)
			{
				Currency = EventAttrs[i].AttrValue;
			}
			if (EventAttrs[i].AttrName.Equals("PerItemCost"))
			{
				PerItemCost = FCString::Atoi(*EventAttrs[i].AttrValue);
			}
		}

		TArray<FAnalyticsEventAttribute> Params;
		Params.Add(FAnalyticsEventAttribute(TEXT("Category"), TEXT("Item Purchase")));
		Params.Add(FAnalyticsEventAttribute(TEXT("Label"), FString::Printf(TEXT("Cost: %d %s"), PerItemCost, *Currency)));
		Params.Add(FAnalyticsEventAttribute(TEXT("Value"), ItemQuantity));
		RecordEvent(ItemId, Params);
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordCurrencyPurchase(const FString& GameCurrencyType, int GameCurrencyAmount, const TArray<FAnalyticsEventAttribute>& EventAttrs)
{
	if (bHasSessionStarted)
	{
		if (GameCurrencyType.Len() > 0)
		{
			FString RealCurrencyType = FString("USD");
			float RealMoneyCost = 0;
			FString PaymentProvider = FString("Default Provider");

			for (int i = 0; i < EventAttrs.Num(); i++)
			{
				if (EventAttrs[i].AttrName.Equals("RealCurrencyType") && EventAttrs[i].AttrValue.Len() > 0)
				{
					RealCurrencyType = EventAttrs[i].AttrValue;
				}
				if (EventAttrs[i].AttrName.Equals("RealMoneyCost") && EventAttrs[i].AttrValue.Len() > 0)
				{
					RealMoneyCost = FCString::Atof(*EventAttrs[i].AttrValue);
				}
				if (EventAttrs[i].AttrName.Equals("PaymentProvider") && EventAttrs[i].AttrValue.Len() > 0)
				{
					PaymentProvider = EventAttrs[i].AttrValue;
				}
			}

			FString TransactionId = FMD5::HashAnsiString(*(GameCurrencyType + RealCurrencyType + PaymentProvider + FDateTime::Now().ToString()));

#if PLATFORM_IOS
			id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

			if (tracker != nil)
			{
				[tracker send : [[GAIDictionaryBuilder createTransactionWithId : TransactionId.GetNSString()
					affiliation : PaymentProvider.GetNSString()
					revenue : @(RealMoneyCost)
							  tax : @0.0F
					shipping : @0
					currencyCode : RealCurrencyType.GetNSString()] build]];

				[tracker send : [[GAIDictionaryBuilder createItemWithTransactionId : TransactionId.GetNSString()
					name : GameCurrencyType.GetNSString()
					sku : GameCurrencyType.GetNSString()
					category : PaymentProvider.GetNSString()
					price : @((RealMoneyCost / GameCurrencyAmount))
							quantity:@(GameCurrencyAmount)
									 currencyCode : RealCurrencyType.GetNSString()] build]];
			}
#elif PLATFORM_ANDROID
			AndroidThunkCpp_GoogleAnalyticsRecordCurrencyPurchase(TransactionId, GameCurrencyType, GameCurrencyAmount, RealCurrencyType, RealMoneyCost, PaymentProvider);
#else
			TSharedRef<IHttpRequest> HttpRequestTransaction = FHttpModule::Get().CreateRequest();
			HttpRequestTransaction->SetURL("https://www.google-analytics.com/collect?v=1&t=transaction&tid=" + ApiTrackingId + "&cid=" + UniversalCid + "&geoid=" + Location + "&uid=" + UserId + "&ti=" + TransactionId + "&ta=" + PaymentProvider + "&tr=" + FString::SanitizeFloat(RealMoneyCost) + "&ts=0&tt=0&cu=" + RealCurrencyType + GetSystemInfo());
			HttpRequestTransaction->SetVerb("GET");
			HttpRequestTransaction->ProcessRequest();

			TSharedRef<IHttpRequest> HttpRequestItem = FHttpModule::Get().CreateRequest();
			HttpRequestItem->SetURL("https://www.google-analytics.com/collect?v=1&t=item&tid=" + ApiTrackingId + "&cid=" + UniversalCid + "&geoid=" + Location + "&uid=" + UserId + "&ti=" + TransactionId + "&in=" + GameCurrencyType + "&ip=" + FString::SanitizeFloat(RealMoneyCost / GameCurrencyAmount) + "&iq=" + FString::FromInt(GameCurrencyAmount) + "&iv=" + PaymentProvider + "&ic=" + GameCurrencyType + "&cu=" + RealCurrencyType + GetSystemInfo());
			HttpRequestItem->SetVerb("GET");
			HttpRequestItem->ProcessRequest();
#endif
		}
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordCurrencyGiven(const FString& GameCurrencyType, int GameCurrencyAmount, const TArray<FAnalyticsEventAttribute>& EventAttrs)
{
	if (bHasSessionStarted)
	{
		TArray<FAnalyticsEventAttribute> Params;
		Params.Add(FAnalyticsEventAttribute(TEXT("Category"), TEXT("Currency Given")));
		Params.Add(FAnalyticsEventAttribute(TEXT("Value"), GameCurrencyAmount));
		RecordEvent(GameCurrencyType, Params);
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordError(const FString& Error, const TArray<FAnalyticsEventAttribute>& EventAttrs)
{
	if (bHasSessionStarted)
	{
		if (Error.Len() > 0)
		{
#if PLATFORM_IOS
			id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];

			if (tracker != nil)
			{
				[tracker send : [[GAIDictionaryBuilder
					createExceptionWithDescription : Error.GetNSString()
					withFatal : @NO] build]];
			}
#elif PLATFORM_ANDROID
			AndroidThunkCpp_GoogleAnalyticsRecordError(Error);
#else
			TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
			HttpRequest->SetURL("https://www.google-analytics.com/collect?v=1&t=exception&tid=" + ApiTrackingId + "&cid=" + UniversalCid + "&geoid=" + Location + "&uid=" + UserId + "&exd=" + Error + "&exf=0" + GetSystemInfo());
			HttpRequest->SetVerb("GET");
			HttpRequest->ProcessRequest();
#endif
		}
	}
}

void FAnalyticsProviderGoogleAnalytics::RecordProgress(const FString& ProgressType, const TArray<FString>& ProgressHierarchy, const TArray<FAnalyticsEventAttribute>& EventAttrs)
{
	if (bHasSessionStarted)
	{
		FString Hierarchy;

		for (int32 Index = 0; Index < ProgressHierarchy.Num(); Index++)
		{
			Hierarchy += ProgressHierarchy[Index];
			if (Index + 1 < ProgressHierarchy.Num())
			{
				Hierarchy += TEXT(".");
			}
		}

		TArray<FAnalyticsEventAttribute> Params;
		Params.Add(FAnalyticsEventAttribute(TEXT("Category"), TEXT("Progression")));
		Params.Add(FAnalyticsEventAttribute(TEXT("Label"), Hierarchy));
		RecordEvent(ProgressType, Params);
	}
}
