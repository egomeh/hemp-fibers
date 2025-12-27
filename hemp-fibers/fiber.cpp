#include "fiber.h"

extern "C" void swap_registers(RegisterContext* register_context);
extern "C" void start_fiber(RegisterContext* register_context, void* data);

static thread_local InternalFiber* tls_fiber;

HempScheduler::HempScheduler()
{
	m_NextToExecute = 0;
	m_FiberReadyMask.clear();
	m_SchedulingLock = SRWLOCK_INIT;
}

__declspec(noinline) void HempScheduler::EndFiber()
{
	bHasTerminatedFibers = true;
	tls_fiber->state = EHempFiberState::TERMINATED;
	swap_registers(&tls_fiber->register_context);
}

void HempScheduler::StaticEndFiber()
{
	tls_fiber->scheduler->EndFiber();
}

HempFiberHandle HempScheduler::InternalNewFiber(void* function, void* data)
{
	//AcquireSRWLockShared(&m_SchedulingLock);

	//for (uint64_t i = 0; i < m_LiveFibers.size(); ++i)
	//{

	//}

	// ReleaseSRWLockShared(&m_SchedulingLock);

	InternalFiber NewFiber;
	NewFiber.scheduler = this;
	NewFiber.state = EHempFiberState::NOT_STARTED;

	// Store function address and the lambda struct
	memcpy(&NewFiber.function, function, sizeof(void*));
	NewFiber.data = data;

	constexpr uint32_t stack_size = 0x8000; 

	// Allocate 32 KiB for the stack
	NewFiber.stack = VirtualAlloc(nullptr, stack_size, MEM_COMMIT, PAGE_READWRITE);

	memset((void*)&NewFiber.register_context, 0, sizeof(RegisterContext));

	// Setup the regisers for stack and function
	NewFiber.register_context.rip = (uint64_t)NewFiber.function;
	NewFiber.register_context.rsp = (uint64_t)NewFiber.stack + stack_size - 0x28;
	uint64_t& fiber_rsp_value = *(uint64_t*)(NewFiber.register_context.rsp);

	// The return value on the stack goes to end the fiber and return control to main thread.
	fiber_rsp_value = (uint64_t)&(HempScheduler::StaticEndFiber);

	m_LiveFibers.emplace_back(NewFiber);

	return m_LiveFibers.back().FiberId;
}

void HempScheduler::RunOneEpisode()
{
	for (uint64_t i = 0; i < m_LiveFibers.size(); ++i)
	{
		auto& fiber = m_LiveFibers[i];

		switch (fiber.state)
		{
		case EHempFiberState::NOT_STARTED:
		{
			tls_fiber = &fiber;
			tls_fiber->state = EHempFiberState::RUNNING;
			start_fiber(&fiber.register_context, fiber.data);
			break;
		}
		case EHempFiberState::RUNNING:
		{
			continue;
		}
		case EHempFiberState::SUSPENDED:
		{
			tls_fiber = &fiber;
			fiber.state = EHempFiberState::RUNNING;
			swap_registers(&fiber.register_context);
			break;
		}
		case EHempFiberState::TERMINATED:
		{
			// Zero out the fiber to make space for a new one
			// Maybe this should just happen the moment it terminates...
			memset(&fiber, 0, sizeof(InternalFiber));
			break;
		}
		}
	}

	tls_fiber = nullptr;

	// Remove all terminated fibers if there are any
	if (bHasTerminatedFibers)
	while (true)
	{
		auto terminated_fiber = std::find_if(m_LiveFibers.begin(), m_LiveFibers.end(), [](const InternalFiber& fiber)
		{
			return fiber.state == EHempFiberState::TERMINATED;
		});

		if (terminated_fiber == m_LiveFibers.end())
		{
			break;
		}
		else
		{
			m_LiveFibers.erase(terminated_fiber);
		}
		bHasTerminatedFibers = false;
	}
}

bool HempScheduler::HasActiveFibers()
{
	return m_LiveFibers.size() > 0;
}

void FiberYield()
{
	// If you're not currently being a fiber, do nothing.
	// Maybe this should be an error?
	if (!tls_fiber)
		return;

	// Mark this fiber as suspended, and return control to thread before us
	tls_fiber->state = EHempFiberState::SUSPENDED;

	swap_registers(&tls_fiber->register_context);
}

