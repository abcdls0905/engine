//--------------------------------------------------------------------
// 文件名:		terrain_cull_sort.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年3月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

// 优化排序函数

#define CUTOFF 8 // testing shows that this is good value
#define STKSIZ (8 * sizeof(void*) - 2)

static inline int compare_sm_visual(render_collect_t::visual_t* elem1, 
	render_collect_t::visual_t* elem2)
{
	if (elem1->fViewZ < elem2->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static inline void swap_sm_visual(render_collect_t::visual_t* a, 
	render_collect_t::visual_t* b)
{
	if (a != b)
	{
		render_collect_t::visual_t tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static void qsort_sm_visual(render_collect_t::visual_t* base, size_t num)
{
	Assert((base != NULL) || (num == 0));

	// Note: the number of stack entries required is no more than
	// 1 + log2(num), so 30 is sufficient for any array
	render_collect_t::visual_t* lo; // ends of sub-array currently sorting
	render_collect_t::visual_t* hi; // ends of sub-array currently sorting
	render_collect_t::visual_t* mid; // points to middle of subarray
	render_collect_t::visual_t* loguy; // traveling pointers for partition step
	render_collect_t::visual_t* higuy; // traveling pointers for partition step
	size_t size; // size of the sub-array
	render_collect_t::visual_t* lostk[STKSIZ];
	render_collect_t::visual_t* histk[STKSIZ];
	int stkptr; // stack for saving sub-array to be processed
	render_collect_t::visual_t* p;
	render_collect_t::visual_t* max;

	if (num < 2)
	{
		return; // nothing to do
	}

	stkptr = 0; // initialize stack
	lo = base;
	hi = base + (num - 1); // initialize limits

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// preserved, locals aren't, so we preserve stuff on the stack
recurse:
	size = (hi - lo) + 1; // number of el's to sort

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= CUTOFF) 
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		while (hi > lo) 
		{
			// A[i] <= A[j] for i <= j, j > hi
			max = lo;

			for (p = lo + 1; p <= hi; p += 1) 
			{
				// A[i] <= A[max] for lo <= i < p
				if (compare_sm_visual(p, max) > 0) 
				{
					max = p;
				}
				/// A[i] <= A[max] for lo <= i <= p
			}

			// A[i] <= A[max] for lo <= i <= hi
			swap_sm_visual(max, hi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			hi -= 1;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted
	}
	else 
	{
		// First we pick a partitioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the median
		// of the values, but also that we select one fast.  We choose the
		// median of the first, middle, and last elements, to avoid bad
		// performance in the face of already sorted data, or data that is made
		// up of multiple sorted runs appended together.  Testing shows that a
		// median-of-three algorithm provides better performance than simply
		// picking the middle element for the latter case.
		mid = lo + (size / 2); // find middle element

		// Sort the first, middle, last elements into order
		if (compare_sm_visual(lo, mid) > 0) 
		{
			swap_sm_visual(lo, mid);
		}

		if (compare_sm_visual(lo, hi) > 0) 
		{
			swap_sm_visual(lo, hi);
		}

		if (compare_sm_visual(mid, hi) > 0) 
		{
			swap_sm_visual(mid, hi);
		}

		// We now wish to partition the array into three pieces, one consisting
		// of elements <= partition element, one of elements equal to the
		// partition element, and one of elements > than it.  This is done
		// below; comments indicate conditions established at every step.
		loguy = lo;
		higuy = hi;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;) 
		{
			// lo <= loguy < hi, lo < higuy <= hi,
			// A[i] <= A[mid] for lo <= i <= loguy,
			// A[i] > A[mid] for higuy <= i < hi,
			// A[hi] >= A[mid]

			// The doubled loop is to avoid calling comp(mid,mid), since some
			// existing comparison funcs don't work when passed the same
			// value for both pointers.
			if (mid > loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy < mid && compare_sm_visual(loguy, mid) <= 0);
			}

			if (mid <= loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy <= hi && compare_sm_visual(loguy, mid) <= 0);
			}

			// lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
			// either loguy > hi or A[loguy] > A[mid]
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_sm_visual(higuy, mid) > 0);

			// lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
			// either higuy == lo or A[higuy] <= A[mid]
			if (higuy < loguy)
			{
				break;
			}

			// if loguy > hi or higuy == lo, then we would have exited, so
			// A[loguy] > A[mid], A[higuy] <= A[mid],
			// loguy <= hi, higuy > lo
			swap_sm_visual(loguy, higuy);

			// If the partition element was moved, follow it.  Only need
			// to check for mid == higuy, since before the swap,
			// A[loguy] > A[mid] implies loguy != mid.
			if (mid == higuy)
			{
				mid = loguy;
			}

			// A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			// of loop is re-established
		}

		// A[i] <= A[mid] for lo <= i < loguy,
		// A[i] > A[mid] for higuy < i < hi,
		// A[hi] >= A[mid]
		// higuy < loguy
		// implying:
		// higuy == loguy-1
		// or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid]

		// Find adjacent elements equal to the partition element.  The
		// doubled loop is to avoid calling comp(mid,mid), since some
		// existing comparison funcs don't work when passed the same value
		// for both pointers.
		higuy += 1;

		if (mid < higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_sm_visual(higuy, mid) == 0);
		}

		if (mid >= higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > lo && compare_sm_visual(higuy, mid) == 0);
		}

		// OK, now we have the following:
		// higuy < loguy
		// lo <= higuy <= hi
		// A[i]  <= A[mid] for lo <= i <= higuy
		// A[i]  == A[mid] for higuy < i < loguy
		// A[i]  >  A[mid] for loguy <= i < hi
		// A[hi] >= A[mid]

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy] and [loguy, hi].
		// We do the smaller one first to minimize stack usage.
		// We only sort arrays of length 2 or more.
		if (higuy - lo >= hi - loguy) 
		{
			if (lo < higuy) 
			{
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           // save big recursion for later

			if (loguy < hi) 
			{
				lo = loguy;
				goto recurse;           // do small recursion
			}
		}
		else 
		{
			if (loguy < hi) 
			{
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               // save big recursion for later
			}

			if (lo < higuy) 
			{
				hi = higuy;
				goto recurse;           // do small recursion
			}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;

	if (stkptr >= 0) 
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           // pop subarray from stack
	}
	else
	{
		return;                 // all subarrays done
	}
}

//

static inline int compare_sm_chunk(render_collect_t::chunk_t* elem1, 
	render_collect_t::chunk_t* elem2)
{
	if (elem1->fViewZ < elem2->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static inline void swap_sm_chunk(render_collect_t::chunk_t* a, 
	render_collect_t::chunk_t* b)
{
	if (a != b)
	{
		render_collect_t::chunk_t tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static void qsort_sm_chunk(render_collect_t::chunk_t* base, size_t num)
{
	Assert((base != NULL) || (num == 0));

	// Note: the number of stack entries required is no more than
	// 1 + log2(num), so 30 is sufficient for any array
	render_collect_t::chunk_t* lo; // ends of sub-array currently sorting
	render_collect_t::chunk_t* hi; // ends of sub-array currently sorting
	render_collect_t::chunk_t* mid; // points to middle of subarray
	render_collect_t::chunk_t* loguy; // traveling pointers for partition step
	render_collect_t::chunk_t* higuy; // traveling pointers for partition step
	size_t size; // size of the sub-array
	render_collect_t::chunk_t* lostk[STKSIZ];
	render_collect_t::chunk_t* histk[STKSIZ];
	int stkptr; // stack for saving sub-array to be processed
	render_collect_t::chunk_t* p;
	render_collect_t::chunk_t* max;

	if (num < 2)
	{
		return; // nothing to do
	}

	stkptr = 0; // initialize stack
	lo = base;
	hi = base + (num - 1); // initialize limits

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// preserved, locals aren't, so we preserve stuff on the stack
recurse:
	size = (hi - lo) + 1; // number of el's to sort

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= CUTOFF) 
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		while (hi > lo) 
		{
			// A[i] <= A[j] for i <= j, j > hi
			max = lo;

			for (p = lo + 1; p <= hi; p += 1) 
			{
				// A[i] <= A[max] for lo <= i < p
				if (compare_sm_chunk(p, max) > 0) 
				{
					max = p;
				}
				/// A[i] <= A[max] for lo <= i <= p
			}

			// A[i] <= A[max] for lo <= i <= hi
			swap_sm_chunk(max, hi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			hi -= 1;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted
	}
	else 
	{
		// First we pick a partitioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the median
		// of the values, but also that we select one fast.  We choose the
		// median of the first, middle, and last elements, to avoid bad
		// performance in the face of already sorted data, or data that is made
		// up of multiple sorted runs appended together.  Testing shows that a
		// median-of-three algorithm provides better performance than simply
		// picking the middle element for the latter case.
		mid = lo + (size / 2); // find middle element

		// Sort the first, middle, last elements into order
		if (compare_sm_chunk(lo, mid) > 0) 
		{
			swap_sm_chunk(lo, mid);
		}

		if (compare_sm_chunk(lo, hi) > 0) 
		{
			swap_sm_chunk(lo, hi);
		}

		if (compare_sm_chunk(mid, hi) > 0) 
		{
			swap_sm_chunk(mid, hi);
		}

		// We now wish to partition the array into three pieces, one consisting
		// of elements <= partition element, one of elements equal to the
		// partition element, and one of elements > than it.  This is done
		// below; comments indicate conditions established at every step.
		loguy = lo;
		higuy = hi;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;) 
		{
			// lo <= loguy < hi, lo < higuy <= hi,
			// A[i] <= A[mid] for lo <= i <= loguy,
			// A[i] > A[mid] for higuy <= i < hi,
			// A[hi] >= A[mid]

			// The doubled loop is to avoid calling comp(mid,mid), since some
			// existing comparison funcs don't work when passed the same
			// value for both pointers.
			if (mid > loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy < mid && compare_sm_chunk(loguy, mid) <= 0);
			}

			if (mid <= loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy <= hi && compare_sm_chunk(loguy, mid) <= 0);
			}

			// lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
			// either loguy > hi or A[loguy] > A[mid]
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_sm_chunk(higuy, mid) > 0);

			// lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
			// either higuy == lo or A[higuy] <= A[mid]
			if (higuy < loguy)
			{
				break;
			}

			// if loguy > hi or higuy == lo, then we would have exited, so
			// A[loguy] > A[mid], A[higuy] <= A[mid],
			// loguy <= hi, higuy > lo
			swap_sm_chunk(loguy, higuy);

			// If the partition element was moved, follow it.  Only need
			// to check for mid == higuy, since before the swap,
			// A[loguy] > A[mid] implies loguy != mid.
			if (mid == higuy)
			{
				mid = loguy;
			}

			// A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			// of loop is re-established
		}

		// A[i] <= A[mid] for lo <= i < loguy,
		// A[i] > A[mid] for higuy < i < hi,
		// A[hi] >= A[mid]
		// higuy < loguy
		// implying:
		// higuy == loguy-1
		// or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid]

		// Find adjacent elements equal to the partition element.  The
		// doubled loop is to avoid calling comp(mid,mid), since some
		// existing comparison funcs don't work when passed the same value
		// for both pointers.
		higuy += 1;

		if (mid < higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_sm_chunk(higuy, mid) == 0);
		}

		if (mid >= higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > lo && compare_sm_chunk(higuy, mid) == 0);
		}

		// OK, now we have the following:
		// higuy < loguy
		// lo <= higuy <= hi
		// A[i]  <= A[mid] for lo <= i <= higuy
		// A[i]  == A[mid] for higuy < i < loguy
		// A[i]  >  A[mid] for loguy <= i < hi
		// A[hi] >= A[mid]

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy] and [loguy, hi].
		// We do the smaller one first to minimize stack usage.
		// We only sort arrays of length 2 or more.
		if (higuy - lo >= hi - loguy) 
		{
			if (lo < higuy) 
			{
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           // save big recursion for later

			if (loguy < hi) 
			{
				lo = loguy;
				goto recurse;           // do small recursion
			}
		}
		else 
		{
			if (loguy < hi) 
			{
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               // save big recursion for later
			}

			if (lo < higuy) 
			{
				hi = higuy;
				goto recurse;           // do small recursion
			}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;

	if (stkptr >= 0) 
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           // pop subarray from stack
	}
	else
	{
		return;                 // all subarrays done
	}
}

//

typedef CTerrainChunk* render_chunk_t;

static inline int compare_chunk(render_chunk_t* elem1, render_chunk_t* elem2)
{
	if ((*elem1)->GetViewZ() < (*elem2)->GetViewZ())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static inline void swap_chunk(render_chunk_t* a, render_chunk_t* b)
{
	if (a != b)
	{
		render_chunk_t tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static void qsort_chunk(render_chunk_t* base, size_t num)
{
	Assert((base != NULL) || (num == 0));

	// Note: the number of stack entries required is no more than
	// 1 + log2(num), so 30 is sufficient for any array
	render_chunk_t* lo; // ends of sub-array currently sorting
	render_chunk_t* hi; // ends of sub-array currently sorting
	render_chunk_t* mid; // points to middle of subarray
	render_chunk_t* loguy; // traveling pointers for partition step
	render_chunk_t* higuy; // traveling pointers for partition step
	size_t size; // size of the sub-array
	render_chunk_t* lostk[STKSIZ];
	render_chunk_t* histk[STKSIZ];
	int stkptr; // stack for saving sub-array to be processed
	render_chunk_t* p;
	render_chunk_t* max;

	if (num < 2)
	{
		return; // nothing to do
	}

	stkptr = 0; // initialize stack
	lo = base;
	hi = base + (num - 1); // initialize limits

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// preserved, locals aren't, so we preserve stuff on the stack
recurse:
	size = (hi - lo) + 1; // number of el's to sort

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= CUTOFF) 
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		while (hi > lo) 
		{
			// A[i] <= A[j] for i <= j, j > hi
			max = lo;

			for (p = lo + 1; p <= hi; p += 1) 
			{
				// A[i] <= A[max] for lo <= i < p
				if (compare_chunk(p, max) > 0) 
				{
					max = p;
				}
				/// A[i] <= A[max] for lo <= i <= p
			}

			// A[i] <= A[max] for lo <= i <= hi
			swap_chunk(max, hi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			hi -= 1;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted
	}
	else 
	{
		// First we pick a partitioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the median
		// of the values, but also that we select one fast.  We choose the
		// median of the first, middle, and last elements, to avoid bad
		// performance in the face of already sorted data, or data that is made
		// up of multiple sorted runs appended together.  Testing shows that a
		// median-of-three algorithm provides better performance than simply
		// picking the middle element for the latter case.
		mid = lo + (size / 2); // find middle element

		// Sort the first, middle, last elements into order
		if (compare_chunk(lo, mid) > 0) 
		{
			swap_chunk(lo, mid);
		}

		if (compare_chunk(lo, hi) > 0) 
		{
			swap_chunk(lo, hi);
		}

		if (compare_chunk(mid, hi) > 0) 
		{
			swap_chunk(mid, hi);
		}

		// We now wish to partition the array into three pieces, one consisting
		// of elements <= partition element, one of elements equal to the
		// partition element, and one of elements > than it.  This is done
		// below; comments indicate conditions established at every step.
		loguy = lo;
		higuy = hi;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;) 
		{
			// lo <= loguy < hi, lo < higuy <= hi,
			// A[i] <= A[mid] for lo <= i <= loguy,
			// A[i] > A[mid] for higuy <= i < hi,
			// A[hi] >= A[mid]

			// The doubled loop is to avoid calling comp(mid,mid), since some
			// existing comparison funcs don't work when passed the same
			// value for both pointers.
			if (mid > loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy < mid && compare_chunk(loguy, mid) <= 0);
			}

			if (mid <= loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy <= hi && compare_chunk(loguy, mid) <= 0);
			}

			// lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
			// either loguy > hi or A[loguy] > A[mid]
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_chunk(higuy, mid) > 0);

			// lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
			// either higuy == lo or A[higuy] <= A[mid]
			if (higuy < loguy)
			{
				break;
			}

			// if loguy > hi or higuy == lo, then we would have exited, so
			// A[loguy] > A[mid], A[higuy] <= A[mid],
			// loguy <= hi, higuy > lo
			swap_chunk(loguy, higuy);

			// If the partition element was moved, follow it.  Only need
			// to check for mid == higuy, since before the swap,
			// A[loguy] > A[mid] implies loguy != mid.
			if (mid == higuy)
			{
				mid = loguy;
			}

			// A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			// of loop is re-established
		}

		// A[i] <= A[mid] for lo <= i < loguy,
		// A[i] > A[mid] for higuy < i < hi,
		// A[hi] >= A[mid]
		// higuy < loguy
		// implying:
		// higuy == loguy-1
		// or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid]

		// Find adjacent elements equal to the partition element.  The
		// doubled loop is to avoid calling comp(mid,mid), since some
		// existing comparison funcs don't work when passed the same value
		// for both pointers.
		higuy += 1;

		if (mid < higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_chunk(higuy, mid) == 0);
		}

		if (mid >= higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > lo && compare_chunk(higuy, mid) == 0);
		}

		// OK, now we have the following:
		// higuy < loguy
		// lo <= higuy <= hi
		// A[i]  <= A[mid] for lo <= i <= higuy
		// A[i]  == A[mid] for higuy < i < loguy
		// A[i]  >  A[mid] for loguy <= i < hi
		// A[hi] >= A[mid]

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy] and [loguy, hi].
		// We do the smaller one first to minimize stack usage.
		// We only sort arrays of length 2 or more.
		if (higuy - lo >= hi - loguy) 
		{
			if (lo < higuy) 
			{
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           // save big recursion for later

			if (loguy < hi) 
			{
				lo = loguy;
				goto recurse;           // do small recursion
			}
		}
		else 
		{
			if (loguy < hi) 
			{
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               // save big recursion for later
			}

			if (lo < higuy) 
			{
				hi = higuy;
				goto recurse;           // do small recursion
			}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;

	if (stkptr >= 0) 
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           // pop subarray from stack
	}
	else
	{
		return;                 // all subarrays done
	}
}

//

static inline int compare_ground(render_chunk_t* elem1, render_chunk_t* elem2)
{
	if ((*elem1)->GetCameraDistance() < (*elem2)->GetCameraDistance())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static inline void swap_ground(render_chunk_t* a, render_chunk_t* b)
{
	if (a != b)
	{
		render_chunk_t tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static void qsort_ground(render_chunk_t* base, size_t num)
{
	Assert((base != NULL) || (num == 0));

	// Note: the number of stack entries required is no more than
	// 1 + log2(num), so 30 is sufficient for any array
	render_chunk_t* lo; // ends of sub-array currently sorting
	render_chunk_t* hi; // ends of sub-array currently sorting
	render_chunk_t* mid; // points to middle of subarray
	render_chunk_t* loguy; // traveling pointers for partition step
	render_chunk_t* higuy; // traveling pointers for partition step
	size_t size; // size of the sub-array
	render_chunk_t* lostk[STKSIZ];
	render_chunk_t* histk[STKSIZ];
	int stkptr; // stack for saving sub-array to be processed
	render_chunk_t* p;
	render_chunk_t* max;

	if (num < 2)
	{
		return; // nothing to do
	}

	stkptr = 0; // initialize stack
	lo = base;
	hi = base + (num - 1); // initialize limits

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// preserved, locals aren't, so we preserve stuff on the stack
recurse:
	size = (hi - lo) + 1; // number of el's to sort

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= CUTOFF) 
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		while (hi > lo) 
		{
			// A[i] <= A[j] for i <= j, j > hi
			max = lo;

			for (p = lo + 1; p <= hi; p += 1) 
			{
				// A[i] <= A[max] for lo <= i < p
				if (compare_ground(p, max) > 0) 
				{
					max = p;
				}
				/// A[i] <= A[max] for lo <= i <= p
			}

			// A[i] <= A[max] for lo <= i <= hi
			swap_ground(max, hi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			hi -= 1;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted
	}
	else 
	{
		// First we pick a partitioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the median
		// of the values, but also that we select one fast.  We choose the
		// median of the first, middle, and last elements, to avoid bad
		// performance in the face of already sorted data, or data that is made
		// up of multiple sorted runs appended together.  Testing shows that a
		// median-of-three algorithm provides better performance than simply
		// picking the middle element for the latter case.
		mid = lo + (size / 2); // find middle element

		// Sort the first, middle, last elements into order
		if (compare_ground(lo, mid) > 0) 
		{
			swap_ground(lo, mid);
		}

		if (compare_ground(lo, hi) > 0) 
		{
			swap_ground(lo, hi);
		}

		if (compare_ground(mid, hi) > 0) 
		{
			swap_ground(mid, hi);
		}

		// We now wish to partition the array into three pieces, one consisting
		// of elements <= partition element, one of elements equal to the
		// partition element, and one of elements > than it.  This is done
		// below; comments indicate conditions established at every step.
		loguy = lo;
		higuy = hi;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;) 
		{
			// lo <= loguy < hi, lo < higuy <= hi,
			// A[i] <= A[mid] for lo <= i <= loguy,
			// A[i] > A[mid] for higuy <= i < hi,
			// A[hi] >= A[mid]

			// The doubled loop is to avoid calling comp(mid,mid), since some
			// existing comparison funcs don't work when passed the same
			// value for both pointers.
			if (mid > loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy < mid && compare_ground(loguy, mid) <= 0);
			}

			if (mid <= loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy <= hi && compare_ground(loguy, mid) <= 0);
			}

			// lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
			// either loguy > hi or A[loguy] > A[mid]
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_ground(higuy, mid) > 0);

			// lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
			// either higuy == lo or A[higuy] <= A[mid]
			if (higuy < loguy)
			{
				break;
			}

			// if loguy > hi or higuy == lo, then we would have exited, so
			// A[loguy] > A[mid], A[higuy] <= A[mid],
			// loguy <= hi, higuy > lo
			swap_ground(loguy, higuy);

			// If the partition element was moved, follow it.  Only need
			// to check for mid == higuy, since before the swap,
			// A[loguy] > A[mid] implies loguy != mid.
			if (mid == higuy)
			{
				mid = loguy;
			}

			// A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			// of loop is re-established
		}

		// A[i] <= A[mid] for lo <= i < loguy,
		// A[i] > A[mid] for higuy < i < hi,
		// A[hi] >= A[mid]
		// higuy < loguy
		// implying:
		// higuy == loguy-1
		// or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid]

		// Find adjacent elements equal to the partition element.  The
		// doubled loop is to avoid calling comp(mid,mid), since some
		// existing comparison funcs don't work when passed the same value
		// for both pointers.
		higuy += 1;

		if (mid < higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_ground(higuy, mid) == 0);
		}

		if (mid >= higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > lo && compare_ground(higuy, mid) == 0);
		}

		// OK, now we have the following:
		// higuy < loguy
		// lo <= higuy <= hi
		// A[i]  <= A[mid] for lo <= i <= higuy
		// A[i]  == A[mid] for higuy < i < loguy
		// A[i]  >  A[mid] for loguy <= i < hi
		// A[hi] >= A[mid]

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy] and [loguy, hi].
		// We do the smaller one first to minimize stack usage.
		// We only sort arrays of length 2 or more.
		if (higuy - lo >= hi - loguy) 
		{
			if (lo < higuy) 
			{
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           // save big recursion for later

			if (loguy < hi) 
			{
				lo = loguy;
				goto recurse;           // do small recursion
			}
		}
		else 
		{
			if (loguy < hi) 
			{
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               // save big recursion for later
			}

			if (lo < higuy) 
			{
				hi = higuy;
				goto recurse;           // do small recursion
			}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;

	if (stkptr >= 0) 
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           // pop subarray from stack
	}
	else
	{
		return;                 // all subarrays done
	}
}

//

static inline int compare_visual(render_visual_t* elem1, render_visual_t* elem2)
{
	if (elem1->fViewZ < elem2->fViewZ)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

static inline void swap_visual(render_visual_t* a, render_visual_t* b)
{
	if (a != b)
	{
		render_visual_t tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static void qsort_visual(render_visual_t* base, size_t num)
{
	Assert((base != NULL) || (num == 0));

	// Note: the number of stack entries required is no more than
	// 1 + log2(num), so 30 is sufficient for any array
	render_visual_t* lo; // ends of sub-array currently sorting
	render_visual_t* hi; // ends of sub-array currently sorting
	render_visual_t* mid; // points to middle of subarray
	render_visual_t* loguy; // traveling pointers for partition step
	render_visual_t* higuy; // traveling pointers for partition step
	size_t size; // size of the sub-array
	render_visual_t* lostk[STKSIZ];
	render_visual_t* histk[STKSIZ];
	int stkptr; // stack for saving sub-array to be processed
	render_visual_t* p;
	render_visual_t* max;

	if (num < 2)
	{
		return; // nothing to do
	}

	stkptr = 0; // initialize stack
	lo = base;
	hi = base + (num - 1); // initialize limits

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// preserved, locals aren't, so we preserve stuff on the stack
recurse:
	size = (hi - lo) + 1; // number of el's to sort

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= CUTOFF) 
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		while (hi > lo) 
		{
			// A[i] <= A[j] for i <= j, j > hi
			max = lo;

			for (p = lo + 1; p <= hi; p += 1) 
			{
				// A[i] <= A[max] for lo <= i < p
				if (compare_visual(p, max) > 0) 
				{
					max = p;
				}
				/// A[i] <= A[max] for lo <= i <= p
			}

			// A[i] <= A[max] for lo <= i <= hi
			swap_visual(max, hi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			hi -= 1;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted
	}
	else 
	{
		// First we pick a partitioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the median
		// of the values, but also that we select one fast.  We choose the
		// median of the first, middle, and last elements, to avoid bad
		// performance in the face of already sorted data, or data that is made
		// up of multiple sorted runs appended together.  Testing shows that a
		// median-of-three algorithm provides better performance than simply
		// picking the middle element for the latter case.
		mid = lo + (size / 2); // find middle element

		// Sort the first, middle, last elements into order
		if (compare_visual(lo, mid) > 0) 
		{
			swap_visual(lo, mid);
		}

		if (compare_visual(lo, hi) > 0) 
		{
			swap_visual(lo, hi);
		}

		if (compare_visual(mid, hi) > 0) 
		{
			swap_visual(mid, hi);
		}

		// We now wish to partition the array into three pieces, one consisting
		// of elements <= partition element, one of elements equal to the
		// partition element, and one of elements > than it.  This is done
		// below; comments indicate conditions established at every step.
		loguy = lo;
		higuy = hi;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;) 
		{
			// lo <= loguy < hi, lo < higuy <= hi,
			// A[i] <= A[mid] for lo <= i <= loguy,
			// A[i] > A[mid] for higuy <= i < hi,
			// A[hi] >= A[mid]

			// The doubled loop is to avoid calling comp(mid,mid), since some
			// existing comparison funcs don't work when passed the same
			// value for both pointers.
			if (mid > loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy < mid && compare_visual(loguy, mid) <= 0);
			}

			if (mid <= loguy) 
			{
				do  
				{
					loguy += 1;
				} while (loguy <= hi && compare_visual(loguy, mid) <= 0);
			}

			// lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
			// either loguy > hi or A[loguy] > A[mid]
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_visual(higuy, mid) > 0);

			// lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
			// either higuy == lo or A[higuy] <= A[mid]
			if (higuy < loguy)
			{
				break;
			}

			// if loguy > hi or higuy == lo, then we would have exited, so
			// A[loguy] > A[mid], A[higuy] <= A[mid],
			// loguy <= hi, higuy > lo
			swap_visual(loguy, higuy);

			// If the partition element was moved, follow it.  Only need
			// to check for mid == higuy, since before the swap,
			// A[loguy] > A[mid] implies loguy != mid.
			if (mid == higuy)
			{
				mid = loguy;
			}

			// A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
			// of loop is re-established
		}

		// A[i] <= A[mid] for lo <= i < loguy,
		// A[i] > A[mid] for higuy < i < hi,
		// A[hi] >= A[mid]
		// higuy < loguy
		// implying:
		// higuy == loguy-1
		// or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid]

		// Find adjacent elements equal to the partition element.  The
		// doubled loop is to avoid calling comp(mid,mid), since some
		// existing comparison funcs don't work when passed the same value
		// for both pointers.
		higuy += 1;

		if (mid < higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > mid && compare_visual(higuy, mid) == 0);
		}

		if (mid >= higuy) 
		{
			do  
			{
				higuy -= 1;
			} while (higuy > lo && compare_visual(higuy, mid) == 0);
		}

		// OK, now we have the following:
		// higuy < loguy
		// lo <= higuy <= hi
		// A[i]  <= A[mid] for lo <= i <= higuy
		// A[i]  == A[mid] for higuy < i < loguy
		// A[i]  >  A[mid] for loguy <= i < hi
		// A[hi] >= A[mid]

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy] and [loguy, hi].
		// We do the smaller one first to minimize stack usage.
		// We only sort arrays of length 2 or more.
		if (higuy - lo >= hi - loguy) 
		{
			if (lo < higuy) 
			{
				lostk[stkptr] = lo;
				histk[stkptr] = higuy;
				++stkptr;
			}                           // save big recursion for later

			if (loguy < hi) 
			{
				lo = loguy;
				goto recurse;           // do small recursion
			}
		}
		else 
		{
			if (loguy < hi) 
			{
				lostk[stkptr] = loguy;
				histk[stkptr] = hi;
				++stkptr;               // save big recursion for later
			}

			if (lo < higuy) 
			{
				hi = higuy;
				goto recurse;           // do small recursion
			}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;

	if (stkptr >= 0) 
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;           // pop subarray from stack
	}
	else
	{
		return;                 // all subarrays done
	}
}

