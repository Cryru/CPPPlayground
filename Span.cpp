#include "Span.h"

Span EmptySpan()
{
	Span sp;
	sp.Data = NULL;
	sp.Length = 0;
	return sp;
}

Span CreateSpan(void *data, i32 length)
{
	Span sp;
	sp.Data = data;
	sp.Length = length;
	return sp;
}

Span SliceSpanAtNullTerminated(char *data, i32 allocatedSize)
{
	for (i32 i = 0; i < allocatedSize; i++)
	{
		i8 c = data[i];
		if (!c)
			return CreateSpan(data, i);
	}
	return CreateSpan(data, allocatedSize);
}

void PrintSpan(Span *sp)
{
	for (i32 i = 0; i < sp->Length; i++)
	{
		printf("%c", ((i8 *)sp->Data)[i]);
	}
}

i32 CopyToSpan(Span* span, void *data, i32 amount)
{
	amount = std::min(amount, span->Length);
	memcpy(span->Data, data, amount);
	return amount;
}

Span SliceSpan(Span *span, i32 offset, i32 length)
{
	Span sp;
	sp.Data = (void*) ((i8*) span->Data + offset);
	sp.Length = length;
	return sp;
}