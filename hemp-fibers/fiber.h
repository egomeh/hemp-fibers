#pragma once

#include <cstdint>
#include <vector>
#include <Windows.h>
#include <atomic>

typedef uint64_t HempFiberHandle;
struct InternalFiber;

enum class EHempFiberState : uint8_t
{
	INVALID = 0,
	NOT_STARTED,
	RUNNING,
	SUSPENDED,
	TERMINATED
};

_declspec(align(16)) struct RegisterContext
{
	// Voliatile registers
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t rbx;
	uint64_t rbp;
	uint64_t rsp;
	uint64_t rip;
	uint32_t mxcsr;

	// Volatile (caller-saved) registers
	// Added becuae the compiler apparent
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
};

class HempScheduler;
struct InternalFiber
{
	void* function;
	void* data;
	void* stack;
	RegisterContext register_context;
	HempFiberHandle FiberId;
	HempScheduler* scheduler;
	EHempFiberState state;
};

class HempScheduler
{
private:

public:

	HempScheduler();

	void EndFiber();
	static void StaticEndFiber();

	template<typename FunctionType>
	HempFiberHandle NewFiber(FunctionType& Function)
	{
		using FunctionTypeCallableType = void (FunctionType::*)() const;
		FunctionTypeCallableType lambda_function_address = &FunctionType::operator();
		return InternalNewFiber((void*)&lambda_function_address, (void*)&Function);
	}

	HempFiberHandle InternalNewFiber(void* function, void* data);

	void RunOneEpisode();
	bool HasActiveFibers();

private:
	std::vector<InternalFiber> m_LiveFibers;
	bool bHasTerminatedFibers;

	// For scheduling
private:
	std::vector<uint8_t> m_FiberReadyMask;
	std::atomic<uint64_t> m_NextToExecute;
	SRWLOCK m_SchedulingLock;
};

void FiberYield();

