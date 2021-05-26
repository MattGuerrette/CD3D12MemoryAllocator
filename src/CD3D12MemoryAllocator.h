
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
#include <stdbool.h>
#endif

BEGIN_DECLS

#include <d3d12.h>
#include <dxgi.h>

typedef void *(*CD3D12MA_ALLOCATE_FUNC_PTR) (size_t Size, size_t Alignment, void *pUserData);
typedef void (*CD3D12MA_FREE_FUNC_PTR) (void *pMemory, void *pUserData);

typedef struct CD3D12MAAllocator CD3D12MAAllocator;
typedef struct CD3D12MAAllocation CD3D12MAAllocation;
typedef struct CD3D12MAPool CD3D12MAPool;

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
  void (*Release) (CD3D12MAPool *This);
  void (*GetDesc) (CD3D12MAPool *This, CD3D12MA_POOL_DESC *pOut);
  void (*CalculateStats) (CD3D12MAPool *This, CD3D12MA_STAT_INFO *pOut);
  void (*SetName) (CD3D12MAPool *This, LPCWSTR Name);
  LPCWSTR (*GetName) (CD3D12MAPool *This);
} CD3D12MAPoolVtbl;

typedef struct CD3D12MAPool
{
  /*<private>*/
  const CD3D12MAPoolVtbl *pVtbl;
  void *pInstance;
} CD3D12MAPool;

#define CD3D12MAPool_Release(this) (this->pVtbl)->Release (this)
#define CD3D12MAPool_GetDesc(this, pOut) (this->pVtbl)->GetDesc (this, pOut)
#define CD3D12MAPool_CalculateStats(this, pOut) (this->pVtbl)->CalculateStats (this, pOut)
#define CD3D12MAPool_SetName(this, name) (this->pVtbl)->SetName (this, name)
#define CD3D12MAPool_GetName(this) (this->pVtbl)->GetName (this)

typedef enum
{
  CD3D12MA_ALLOCATION_FLAG_NONE = 0,
  CD3D12MA_ALLOCATION_FLAG_COMMITTED = 0x1,
  CD3D12MA_ALLOCATION_FLAG_NEVER_ALLOCATE = 0x2,
  CD3D12MA_ALLOCATION_FLAG_WITHIN_BUDGET = 0x4
} CD3D12MA_ALLOCATION_FLAGS;

typedef struct
{
  CD3D12MA_ALLOCATION_FLAGS Flags;
  D3D12_HEAP_TYPE HeapType;
  D3D12_HEAP_FLAGS ExtraHeapFlags;
  CD3D12MAPool *CustomPool;
} CD3D12MA_ALLOCATION_DESC;

typedef struct CD3D12MAAllocationVtbl
{
  void (*Release) (CD3D12MAAllocation *This);
  UINT64 (*GetOffset) (CD3D12MAAllocation *This);
  UINT64 (*GetSize) (CD3D12MAAllocation *This);
  ID3D12Resource *(*GetResource) (CD3D12MAAllocation *This);
  ID3D12Heap *(*GetHeap) (CD3D12MAAllocation *This);
  void (*SetName) (CD3D12MAAllocation *This, LPCWSTR Name);
  LPCWSTR (*GetName) (CD3D12MAAllocation *This);
  bool (*WasZeroInitialized) (CD3D12MAAllocation *This);
} CD3D12MAAllocationVtbl;

struct CD3D12MAAllocation
{
  /*<private>*/
  const CD3D12MAAllocationVtbl *pVtbl;
  void *pInstance;
};

#define CD3D12MAAllocation_Release(this) (this->pVtbl)->Release (this)
#define CD3D12MAAllocation_GetOffset(this) (this->pVtbl)->GetOffset (this)
#define CD3D12MAAllocation_GetSize(this) (this->pVtbl)->GetSize (this)
#define CD3D12MAAllocation_GetResource(this) (this->pVtbl)->GetResource (this)
#define CD3D12MAAllocation_GetHeap(this) (this->pVtbl)->GetHeap (this)
#define CD3D12MAAllocation_SetName(this, name) (this->pVtbl)->SetName (this, name)
#define CD3D12MAAllocation_GetName(this) (this->pVtbl)->GetName (this)
#define CD3D12MAAllocation_WasZeroInitialized(this) (this->pVtbl)->WasZeroInitialized (this)

typedef struct CD3D12MAAllocatorVtbl
{
  void (*Release) (CD3D12MAAllocator *This);

  void (*GetD3D12Options) (CD3D12MAAllocator *This,
                           const D3D12_FEATURE_DATA_D3D12_OPTIONS **ppOptions);
  bool (*IsUMA) (CD3D12MAAllocator *This);

  bool (*IsCacheCoherentUMA) (CD3D12MAAllocator *This);

  HRESULT (*CreateResource)
  (CD3D12MAAllocator *This,
   const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
   const D3D12_RESOURCE_DESC *pResourceDesc,
   D3D12_RESOURCE_STATES InitialResourceState,
   const D3D12_CLEAR_VALUE *pOptimizedClearValue,
   CD3D12MAAllocation **ppAllocation,
   REFIID iidResource,
   void **ppvResource);

#ifdef __ID3D12Device4_INTERFACE_DEFINED__
  HRESULT (*CreateResource1)
  (CD3D12MAAllocator *This,
   const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
   const D3D12_RESOURCE_DESC *pResourceDesc,
   D3D12_RESOURCE_STATES InitialResourceState,
   const D3D12_CLEAR_VALUE *pOptimizedClearValue,
   ID3D12ProtectedResourceSession *pProtectedSession,
   CD3D12MAAllocation **ppAllocation,
   REFIID iidResource,
   void **ppvResource);
#endif

#ifdef __ID3D12Device8_INTERFACE_DEFINED__
  HRESULT (*CreateResource2)
  (CD3D12MAAllocator *This,
   const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
   const D3D12_RESOURCE_DESC1 *pResourceDesc,
   D3D12_RESOURCE_STATES InitialResourceState,
   const D3D12_CLEAR_VALUE *pOptimizedClearValue,
   ID3D12ProtectedResourceSession *pProtectedSession,
   CD3D12MAAllocation **ppAllocation,
   REFIID iidResource,
   void **ppvResource);
#endif

  HRESULT (*AllocateMemory)
  (CD3D12MAAllocator *This,
   const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
   const D3D12_RESOURCE_ALLOCATION_INFO *pAllocInfo,
   CD3D12MAAllocation **ppAllocation);

#ifdef __ID3D12Device4_INTERFACE_DEFINED__
  HRESULT (*AllocateMemory1)
  (CD3D12MAAllocator *This,
   const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
   const D3D12_RESOURCE_ALLOCATION_INFO *pAllocInfo,
   ID3D12ProtectedResourceSession *pProtectedSession,
   CD3D12MAAllocation **ppAllocation);
#endif

  HRESULT (*CreateAliasingResource)
  (CD3D12MAAllocator *This,
   CD3D12MAAllocation *pAllocation,
   UINT64 AllocationLocalOffset,
   const D3D12_RESOURCE_DESC *pResourceDesc,
   D3D12_RESOURCE_STATES InitialResourceState,
   const D3D12_CLEAR_VALUE *pOptimizedClearValue,
   REFIID iidResource,
   void **ppvResource);

  HRESULT (*CreatePool)
  (CD3D12MAAllocator *This, const CD3D12MA_POOL_DESC *pPoolDesc, CD3D12MAPool **ppPool);

  void (*SetCurrentFrameIndex) (CD3D12MAAllocator *This, UINT frameIndex);

  void (*CalculateStats) (CD3D12MAAllocator *This, CD3D12MA_STATS *pStats);

  void (*GetBudget) (CD3D12MAAllocator *This,
                     CD3D12MA_BUDGET *pGpuBudget,
                     CD3D12MA_BUDGET *pCpuBudget);
} CD3D12MAAllocatorVtbl;

struct CD3D12MAAllocator
{
  /*<private>*/
  const CD3D12MAAllocatorVtbl *pVtbl;
  void *pInstance;
};

#define CD3D12MAAllocator_Release(this) (this->pVtbl)->Release (this)
#define CD3D12MAAllocator_GetD3D12Options(this, ppOptions)                                        \
  (this->pVtbl)->GetD3D12Options (this, &(ppOptions))
#define CD3D12MAAllocator_IsUMA(this) (this->pVtbl)->IsUMA (this)
#define CD3D12MAAllocator_IsCacheCoherentUMA(this) (this->pVtbl)->IsCacheCoherentUMA (this)
#define CD3D12MAAllocator_CreateResource(this, pAllocDesc, pResourceDesc, InitialResourceState,   \
                                         pOptimizedClearValue, ppAllocation, iidResource,         \
                                         ppvResource)                                             \
  (this->pVtbl)                                                                                   \
      ->CreateResource (this, pAllocDesc, pResourceDesc, InitialResourceState,                    \
                        pOptimizedClearValue, ppAllocation, iidResource, ppvResource)

#ifdef __ID3D12Device4_INTERFACE_DEFINED__
#define CD3D12MAAllocator_CreateResource1(this, pAllocDesc, pResourceDesc, InitialResourceState,  \
                                          pOptimizedClearValue, pProtectedSession, ppAllocation,  \
                                          iidResource, ppvResource)                               \
  (this->pVtbl)                                                                                   \
      ->CreateResource (this, pAllocDesc, pResourceDesc, InitialResourceState,                    \
                        pOptimizedClearValue, pProtectedSession, ppAllocation, iidResource,       \
                        ppvResource)
#endif

#ifdef __ID3D12Device8_INTERFACE_DEFINED__
#define CD3D12MAAllocator_CreateResource2(this, pAllocDesc, pResourceDesc, InitialResourceState,  \
                                          pOptimizedClearValue, pProtectedSession, ppAllocation,  \
                                          iidResource, ppvResource)                               \
  (this->pVtbl)                                                                                   \
      ->CreateResource (this, pAllocDesc, pResourceDesc, InitialResourceState,                    \
                        pOptimizedClearValue, pProtectedSession, ppAllocation, iidResource,       \
                        ppvResource)
#endif
#define CD3D12MAAllocator_AllocateMemory(this, pAllocDesc, pAllocInfo, ppAllocation)              \
  (this->pVtbl)->AllocateMemory (this, pAllocDesc, pAllocInfo, ppAllocation)
#ifdef __ID3D12Device4_INTERFACE_DEFINED__
#define CD3D12MAAllocator_AllocateMemory1(this, pAllocDesc, pAllocInfo, pProtectedSession,        \
                                          ppAllocation)                                           \
  (this->pVtbl)->AllocateMemory1 (this, pAllocDesc, pAllocInfo, pProtectedSession, ppAllocation)
#endif
#define CD3D12MAAllocator_CreateAliasingResource(this, pAllocation, AllocationLocalOffset,        \
                                                 pResourceDesc, InitialResourceState,             \
                                                 pOptimizedClearValue, iidResource, ppvResource)  \
  (this->pVtbl)                                                                                   \
      ->CreateAliasingResource (this, pAllocation, AllocationLocalOffset, pResourceDesc,          \
                                InitialResourceState, pOptimizedClearValue, iidResource,          \
                                ppvResource)
#define CD3D12MAAllocator_CreatePool(this, pPoolDesc, ppPool)                                     \
  (this->pVtbl)->CreatePool (this, pPoolDesc, ppPool)
#define CD3D12MAAllocator_SetCurrentFrameIndex(this, index)                                       \
  (this->pVtbl)->SetCurrentFrameIndex (this, index)
#define CD3D12MAAllocator_CalculateStats(this, pStats) (this->pVtbl)->CalculateStats (this, pStats)
#define CD3D12MAAllocator_GetBudget(this, pGpuBudget, pCpuBudget)                                 \
  (this->pVtbl)->GetBudget (this, pGpuBudget, pCpuBudget)


HRESULT
CreateAllocator (const CD3D12MA_ALLOCATOR_DESC *pDesc, CD3D12MAAllocator **ppAllocator);

END_DECLS

#endif