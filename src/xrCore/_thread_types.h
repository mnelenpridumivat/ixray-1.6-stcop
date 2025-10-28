#pragma once

#ifdef IXR_WINDOWS
#	include <ppl.h>
#	include <concurrent_unordered_map.h>
#	include <concurrent_vector.h>
#else
#	include <tbb/task_group.h>
#	include <tbb/parallel_for.h>
#	include <tbb/blocked_range.h>
#	include <tbb/parallel_for_each.h>
#	include <tbb/concurrent_unordered_map.h>
#	include <tbb/concurrent_vector.h>
#endif
#include <atomic>

// Atomic types
using xr_atomic_u8  = std::atomic_uint8_t;
using xr_atomic_u32  = std::atomic_uint32_t;
using xr_atomic_s32  = std::atomic_int;
using xr_atomic_bool = std::atomic_bool;
using xr_atomic_float = std::atomic<float>;

// Tasks Redefinition
#ifdef IXR_WINDOWS
#if defined(IXRAY_PROFILER)
class xr_task_group
{
private:
	Concurrency::task_group m_taskGroup;

public:
	template<typename Func>
	void run(const Func& func, const char* taskName = "PPL Task")
	{
		m_taskGroup.run([func, taskName]() {
			PROF_THREAD(std::to_string(std::this_thread::get_id()._Get_underlying_id()).c_str())
			OPTICK_EVENT_DYNAMIC(taskName);
			func();
		});
	}

	void wait()
	{
		OPTICK_EVENT("TaskGroup Wait");
		m_taskGroup.wait();
	}
};
#else
using xr_task_group = concurrency::task_group;
#endif

template <typename T, typename U>
using xr_concurrent_unordered_map = concurrency::concurrent_unordered_map<T, U>;

template <typename T>
using xr_concurrent_vector = concurrency::concurrent_vector<T>;
#else
using xr_task_group = tbb::task_group;

template <typename T, typename U>
using xr_concurrent_unordered_map = tbb::concurrent_unordered_map<T, U>;
template <typename T>
using xr_concurrent_vector = tbb::concurrent_vector<T>;
#endif

template<typename BlockRangeType, typename Body>
inline void xr_parallel_for(BlockRangeType Begin, BlockRangeType End, Body Functor)
{
#ifdef IXR_WINDOWS
	concurrency::parallel_for(Begin, End, Functor);
#else
	using RangeType = tbb::blocked_range<BlockRangeType>;
	RangeType RangeBlock(Begin, End);

	tbb::parallel_for
	(
		RangeBlock,
		[&Functor](const RangeType& Range)
		{
			for (BlockRangeType Iter = Range.begin(); Iter != Range.end(); ++Iter)
			{
				Functor(Iter);
			}
		}
	);
#endif
}

inline const size_t xr_max_concurrency()
{
#ifdef IXR_WINDOWS
	return Concurrency::CurrentScheduler::Get()->GetNumberOfVirtualProcessors();
#elif defined(IXR_LINUX)
	return tbb::this_task_arena::max_concurrency();
#else
	return std::thread::hardware_concurrency();
#endif
}

template<typename BlockRangeType, typename Body>
inline void xr_parallel_for(BlockRangeType Begin, BlockRangeType End, BlockRangeType Grain, Body Functor)
{
#ifdef IXR_WINDOWS
	concurrency::parallel_for(Begin, End, Grain, Functor);
#else
	using RangeType = tbb::blocked_range<BlockRangeType>;
	tbb::parallel_for(RangeType(Begin, End, Grain), [&](const RangeType& Range)
	{
		for (BlockRangeType i = Range.begin(); i != Range.end(); ++i)
		{
			Functor(i);
		}
	});
#endif
}

template<typename Index, typename Body>
inline void xr_parallel_foreach(Index Begin, Index End, Body Functor)
{
#ifdef IXR_WINDOWS
	concurrency::parallel_for_each(Begin, End, Functor);
#else
	tbb::parallel_for_each(Begin, End, Functor);
#endif
}
