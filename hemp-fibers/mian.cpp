#include "fiber.h"

#include <iostream>

int main()
{
	HempScheduler scheduler;

	int x = 7;
	auto f = [&]()
	{
		for (int i = 0; i < 3; ++i)
		{
			x += 2;
			printf("Hello from f, x is now %d\n", x);
			FiberYield();
		}
	};

	scheduler.NewFiber(f);

	while (scheduler.HasActiveFibers())
	{
		scheduler.RunOneEpisode();
		printf("Main thread has control now and x is %d\n", x);
	}

	printf("x is now %d", x);
}


