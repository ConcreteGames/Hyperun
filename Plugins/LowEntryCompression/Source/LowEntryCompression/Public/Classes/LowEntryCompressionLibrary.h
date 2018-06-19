#pragma once


#include "Engine.h"
#include "Core.h"
#include "CoreUObject.h"
#include "Compression.h"

#include "ELowEntryCompressionResult.h"

#include "LowEntryCompressionLibrary.generated.h"


UCLASS()
class LOWENTRYCOMPRESSION_API ULowEntryCompressionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	/**
	* Compresses a Byte Array using the ZLib algorithm.
	*/
	/*UFUNCTION(BlueprintPure, Category = "Low Entry|Compression", Meta = (DisplayName = "Compress (Zlib)"))*/
		static TArray<uint8> CompressZlib(const TArray<uint8>& Data);

	/**
	* Tries to decompress a Byte Array using the ZLib algorithm.
	*/
	/*UFUNCTION(BlueprintPure, Category = "Low Entry|Compression", Meta = (DisplayName = "Decompress (Zlib)", ExpandEnumAsExecs = "Branch"))*/
		static TArray<uint8> DecompressZlib(const TArray<uint8>& CompressedData, ELowEntryCompressionResult& Branch);


	/**
	* Compresses a Byte Array with LZF.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Compression", Meta = (DisplayName = "Compress (Lzf)"))
		static TArray<uint8> CompressLzf(const TArray<uint8>& Data);
		
	static TArray<uint8> CompressLzfThreadSafe(const TArray<uint8>& Data);

	/**
	* Tries to decompress a Byte Array using the LZF algorithm. Will return an empty Array on failure.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Compression", Meta = (DisplayName = "Decompress (Lzf)"))
		static TArray<uint8> DecompressLzf(const TArray<uint8>& CompressedData);
};
