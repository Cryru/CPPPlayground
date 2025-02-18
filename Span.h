#pragma once

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "Common.h"

struct SpanType
{
    void *Data;
    i32 Length;
};
typedef struct SpanType Span;

#define NEW_STACK_SPAN(name, size) \
    char __spanInner[size]; \
    Span name = CreateSpan(__spanInner, size)

Span EmptySpan();
Span CreateSpan(void *data, i32 length);
Span SliceSpanAtNullTerminated(char *data, i32 allocatedSize);
void PrintSpan(Span *sp);

i32 CopyToSpan(Span* span, void *data, i32 amount);
Span SliceSpan(Span* span, i32 offset, i32 length);