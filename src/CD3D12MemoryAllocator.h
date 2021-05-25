
#ifndef CD3D12_MEMORY_ALLOCATOR_H_
#define CD3D12_MEMORY_ALLOCATOR_H_

#ifdef __cplusplus
#define BEGIN_DECLS                                                                               \
  extern "C"                                                                                      \
  {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif

BEGIN_DECLS

#include <d3d12.h>
#include <dxgi.h>

typedef void *(*CD3D12MA_ALLOCATE_FUNC_PTR) (size_t Size, size_t Alignment, void *pUserData);
typedef void (*CD3D12MA_FREE_FUNC_PTR) (void *pMemory, void *pUserData);

typedef struct CD3D12MAAllocator CD3D12MAAllocator;

typedef struct
{
  CD3D12MA_ALLOCATE_FUNC_PTR pAllocate;
  CD3D12MA_FREE_FUNC_PTR pFree;
  void *pUserData;
} CD3D12MA_ALLOCATION_CALLBACKS;

typedef struct
{
  /// Number of memory blocks (heaps) allocated.
  UINT BlockCount;
  /// Number of D3D12MA::Allocation objects allocated.
  UINT AllocationCount;
  /// Number of free ranges of memory between allocations.
  UINT UnusedRangeCount;
  /// Total number of bytes occupied by all allocations.
  UINT64 UsedBytes;
  /// Total number of bytes occupied by unused ranges.
  UINT64 UnusedBytes;
  UINT64 AllocationSizeMin;
  UINT64 AllocationSizeAvg;
  UINT64 AllocationSizeMax;
  UINT64 UnusedRangeSizeMin;
  UINT64 UnusedRangeSizeAvg;
  UINT64 UnusedRangeSizeMax;
} CD3D12MA_STAT_INFO;

#define CD3D12MA_HEAP_TYPE_COUNT 4
typedef struct
{
  CD3D12MA_STAT_INFO Total;
  CD3D12MA_STAT_INFO HeapType[CD3D12MA_HEAP_TYPE_COUNT];
} CD3D12MA_STATS;

typedef struct
{
  UINT64 BlockBytes;
  UINT64 AllocationBytes;
  UINT64 UsageBytes;
  UINT64 BudgetBytes;
} CD3D12MA_BUDGET;

typedef enum
{
  CD3D12MA_ALLOCATOR_FLAG_NONE = 0,
  CD3D12MA_ALLOCATOR_FLAG_SINGLETHREADED = 0x1,
  CD3D12MA_ALLOCATOR_FLAG_ALWAYS_COMMITTED = 0x2
} CD3D12MA_ALLOCATOR_FLAGS;

typedef struct
{
  CD3D12MA_ALLOCATOR_FLAGS Flags;
  ID3D12Device *pDevice;
  UINT64 PreferredBlockSize;
  const CD3D12MA_ALLOCATION_CALLBACKS *pAllocationCallbacks;
  IDXGIAdapter *pAdapter;
} CD3D12MA_ALLOCATOR_DESC;

typedef struct
{
  D3D12_HEAP_PROPERTIES HeapProperties;
  D3D12_HEAP_FLAGS HeapFlags;
  UINT64 BlockSize;
  UINT MinBlockCount;
  UINT MaxBlockCount;
} CD3D12MA_POOL_DESC;

typedef struct CD3D12MAPoolVtbl
{
  void (*Release) ();
} CD3D12MAPoolVtbl;

typedef struct CD3D12MAPool
{
  /*<private>*/
  const CD3D12MAPoolVtbl *pVtbl;
  void *pInstance;
} CD3D12MAPool;

typedef struct CD3D12MAAllocatorVtbl
{
  void (*Release) (CD3D12MAAllocator *This);
  void (*GetD3D12Options) (CD3D12MAAllocator *This,
                           const D3D12_FEATURE_DATA_D3D12_OPTIONS **ppOptions);
} CD3D12MAAllocatorVtbl;

struct CD3D12MAAllocator
{
  /*<private>*/
  const CD3D12MAAllocatorVtbl *pVtbl;
  void *pInstance;
};

#define CD3D12MAAllocator_Release(this) (this->pVtbl)->Release (this)
#define CD3D12MAAllocator_GetD3D12Options(this, ppOptions)                                        \
  (this->pVtbl)->GetD3D12Options (this, &ppOptions)

HRESULT
CreateAllocator (const CD3D12MA_ALLOCATOR_DESC *pDesc, CD3D12MAAllocator **ppAllocator);

END_DECLS

#endif