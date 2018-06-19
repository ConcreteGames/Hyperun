#include "LowEntryCompressionPrivatePCH.h"
#include "LowEntryCompressionByteArrayBuffer.h"


ULowEntryCompressionByteArrayBuffer::ULowEntryCompressionByteArrayBuffer()
{
	buffer.SetNum(32);
}

ULowEntryCompressionByteArrayBuffer::ULowEntryCompressionByteArrayBuffer(int32 initialSize)
{
	buffer.SetNum((initialSize > 0) ? initialSize : 32);
}

ULowEntryCompressionByteArrayBuffer::ULowEntryCompressionByteArrayBuffer(int32 initialSize, int32 maxInitialSize)
{
	initialSize = (initialSize <= maxInitialSize) ? initialSize : maxInitialSize;
	buffer.SetNum((initialSize > 0) ? initialSize : 32);
}
