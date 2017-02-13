#include "../UtcTime.h"
#include <stdio.h>

using server::UtcTime;

void passByConstReference(const UtcTime &x)
{
	printf("%s\n", x.toString().c_str());
}

void passByvalue(UtcTime x)
{
	printf("%s\n", x.toString().c_str());
}

int main()
{
	UtcTime now(UtcTime::now());
	printf("%s\n", now.toString().c_str());
	passByvalue(now);
	passByConstReference(now);
}