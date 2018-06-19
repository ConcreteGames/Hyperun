#pragma once


#include "Engine.h"


class ULowEntryCompressionByteArrayBuffer
{
public:
	ULowEntryCompressionByteArrayBuffer();
	ULowEntryCompressionByteArrayBuffer(int32 initialSize);
	ULowEntryCompressionByteArrayBuffer(int32 initialSize, int32 maxInitialSize);


private:
	TArray<uint8>	buffer;
	int32			length = 0;


private:
	FORCEINLINE void access(int32 index)
	{
		if(index >= buffer.Num())
		{
			int32 newSize = buffer.Num();
			while(index >= newSize)
			{
				newSize = (newSize >= 1073741823) ? 2147483647 : newSize * 2;
			}
			buffer.SetNumUninitialized(newSize);
		}
		if((index + 1) > length)
		{
			length = index + 1;
		}
	}


public:
	FORCEINLINE void set(int32 index, uint8 value)
	{
		access(index);
		buffer[index] = value;
	}

	FORCEINLINE void set(int32 index, const TArray<uint8>& values, int32 valuesOffset, int32 length0)
	{
		access(index + (length0 - 1));
		FMemory::Memcpy(buffer.GetData() + index, values.GetData() + valuesOffset, length0);
	}

	FORCEINLINE uint8 get(int32 index)
	{
		if(index >= buffer.Num())
		{
			return 0;
		}
		return buffer[index];
	}

	FORCEINLINE TArray<uint8> getData()
	{
		buffer.SetNum(length, true);
		return buffer;
	}

	FORCEINLINE TArray<uint8> getData(int32 length0)
	{
		access(length0 - 1);
		if(length0 < length)
		{
			TArray<uint8> copy;
			copy.Append(buffer.GetData(), length0);
			return copy;
		}
		buffer.SetNum(length0, true);
		return buffer;
	}
};
