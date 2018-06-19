#include "LowEntryCompressionPrivatePCH.h"
#include "LowEntryCompressionLzfLibrary.h"
#include "LowEntryCompressionLibrary.h"


// init >>
	ULowEntryCompressionLibrary::ULowEntryCompressionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}
// init <<


TArray<uint8> ULowEntryCompressionLibrary::CompressZlib(const TArray<uint8>& Data)
{
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);
	TArray<uint8> LocalData = Data;
	Compressor << LocalData;
	Compressor.Flush();
	Compressor.Close();
	return CompressedData;
}

TArray<uint8> ULowEntryCompressionLibrary::DecompressZlib(const TArray<uint8>& CompressedData, ELowEntryCompressionResult& Branch)
{
	FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);
	if(Decompressor.IsError())
	{
		Branch = ELowEntryCompressionResult::Failed;
		return TArray<uint8>();
	}
	Branch = ELowEntryCompressionResult::Success;
	FBufferArchive Data;
	Decompressor << Data;
	Decompressor.Flush();
	Decompressor.Close();
	return Data;
}


TArray<uint8> ULowEntryCompressionLibrary::CompressLzf(const TArray<uint8>& Data)
{
	return ULowEntryCompressionLzfLibrary::Compress(Data);
}

TArray<uint8> ULowEntryCompressionLibrary::CompressLzfThreadSafe(const TArray<uint8>& Data)
{
	return ULowEntryCompressionLzfLibrary::Compress(Data, true);
}

TArray<uint8> ULowEntryCompressionLibrary::DecompressLzf(const TArray<uint8>& CompressedData)
{
	return ULowEntryCompressionLzfLibrary::Decompress(CompressedData);
}
