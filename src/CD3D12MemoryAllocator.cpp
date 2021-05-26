
#include "CD3D12MemoryAllocator.h"
#include "D3D12MemAlloc.h"

static_assert (sizeof (CD3D12MA_ALLOCATION_CALLBACKS) == sizeof (D3D12MA::ALLOCATION_CALLBACKS),
               "Size mismatch: sizeof(CD3D12MA_ALLOCATION_CALLBACKS) != "
               "sizeof(D3D12MA::ALLOCATION_CALLBACKS)");
static_assert (sizeof (CD3D12MA_STAT_INFO) == sizeof (D3D12MA::StatInfo),
               "Size mismatch: sizeof(CD3D12MA_STAT_INFO) != sizeof(D3D12MA::StatInfo)");
static_assert (sizeof (CD3D12MA_STATS) == sizeof (D3D12MA::Stats),
               "Size mismatch: sizeof(CD3D12MA_STATS) != sizeof(D3D12MA::Stats)");
static_assert (sizeof (CD3D12MA_BUDGET) == sizeof (D3D12MA::Budget),
               "Size mismatch: sizeof(CD3D12MA_BUDGET) != sizeof(D3D12MA::Budget)");
static_assert (
    sizeof (CD3D12MA_ALLOCATOR_DESC) == sizeof (D3D12MA::ALLOCATOR_DESC),
    "Size mismatch: sizeof(CD3D12MA_ALLOCATOR_DESC) != sizeof(D3D12MA::ALLOCATOR_DESC)");
static_assert (sizeof (CD3D12MA_POOL_DESC) == sizeof (D3D12MA::POOL_DESC),
               "Size mismatch: sizeof(CD3D12MA_POOL_DESC) != sizeof(D3D12MA::POOL_DESC)");
static_assert (
    sizeof (CD3D12MA_ALLOCATION_DESC) == sizeof (D3D12MA::ALLOCATION_DESC),
    "Size mismatch: sizeof(CD3D12MA_ALLOCATION_DESC) != sizeof(D3D12MA::ALLOCATION_DESC)");

static void
CD3D12MAAllocationRelease (CD3D12MAAllocation *pAllocation)
{
  reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->Release ();
  free (pAllocation);
}

static UINT64
CD3D12MAAllocationGetOffset (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->GetOffset ();
}

static UINT64
CD3D12MAAllocationGetSize (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->GetSize ();
}

static ID3D12Resource *
CD3D12MAAllocationGetResource (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->GetResource ();
}

static ID3D12Heap *
CD3D12MAAllocationGetHeap (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->GetHeap ();
}

static void
CD3D12MAAllocationSetName (CD3D12MAAllocation *pAllocation, LPCWSTR Name)
{
  reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->SetName (Name);
}

static LPCWSTR
CD3D12MAAllocationGetName (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->GetName ();
}

static bool
CD3D12MAAllocationWasZeroInitialized (CD3D12MAAllocation *pAllocation)
{
  return reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance)->WasZeroInitialized ();
}

static const CD3D12MAAllocationVtbl g_CD3D12MAAllocationVtbl = {
  CD3D12MAAllocationRelease,     CD3D12MAAllocationGetOffset,         CD3D12MAAllocationGetSize,
  CD3D12MAAllocationGetResource, CD3D12MAAllocationGetHeap,           CD3D12MAAllocationSetName,
  CD3D12MAAllocationGetName,     CD3D12MAAllocationWasZeroInitialized
};

//////////////////////////////////////////////////////////////////////////////////////////////////

static void
CD3D12MAPoolRelease (CD3D12MAPool *pPool)
{
  reinterpret_cast<D3D12MA::Pool *> (pPool->pInstance)->Release ();
  free (pPool);
}

static void
CD3D12MAPoolGetDesc (CD3D12MAPool *pPool, CD3D12MA_POOL_DESC *pOut)
{
  D3D12MA::POOL_DESC pool_desc = reinterpret_cast<D3D12MA::Pool *> (pPool->pInstance)->GetDesc ();
  *pOut = *reinterpret_cast<CD3D12MA_POOL_DESC *> (&pool_desc);
}

static void
CD3D12MAPoolCalculateStats (CD3D12MAPool *pPool, CD3D12MA_STAT_INFO *pOut)
{
  D3D12MA::StatInfo stats_info = {};
  reinterpret_cast<D3D12MA::Pool *> (pPool->pInstance)->CalculateStats (&stats_info);
  *pOut = *reinterpret_cast<CD3D12MA_STAT_INFO *> (&stats_info);
}

static void
CD3D12MAPoolSetName (CD3D12MAPool *pPool, LPCWSTR Name)
{
  reinterpret_cast<D3D12MA::Pool *> (pPool->pInstance)->SetName (Name);
}

static LPCWSTR
CD3D12MAPoolGetName (CD3D12MAPool *pPool)
{
  return reinterpret_cast<D3D12MA::Pool *> (pPool->pInstance)->GetName ();
}

static const CD3D12MAPoolVtbl g_CD3D12MAPoolVtbl = { CD3D12MAPoolRelease, CD3D12MAPoolGetDesc,
                                                     CD3D12MAPoolCalculateStats,
                                                     CD3D12MAPoolSetName, CD3D12MAPoolGetName };

//////////////////////////////////////////////////////////////////////////////////////////////////

static void
CD3D12MAAllocatorRelease (CD3D12MAAllocator *pAllocator)
{
  reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->Release ();
  free (pAllocator);
}

static void
CD3D12MAAllocatorGetD3D12Options (CD3D12MAAllocator *pAllocator,
                                  const D3D12_FEATURE_DATA_D3D12_OPTIONS **ppOptions)
{
  *ppOptions = &reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->GetD3D12Options ();
}

static bool
CD3D12MAAllocatorIsUMA (CD3D12MAAllocator *pAllocator)
{
  return reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->IsUMA ();
}

static bool
CD3D12MAAllocatorIsCacheCoherentUMA (CD3D12MAAllocator *pAllocator)
{
  return reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->IsCacheCoherentUMA ();
}

static HRESULT
CD3D12MAAllocatorCreateResource (CD3D12MAAllocator *pAllocator,
                                 const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
                                 const D3D12_RESOURCE_DESC *pResourceDesc,
                                 D3D12_RESOURCE_STATES InitialResourceState,
                                 const D3D12_CLEAR_VALUE *pOptimizedClearValue,
                                 CD3D12MAAllocation **ppAllocation,
                                 REFIID iidResource,
                                 void **ppvResource)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::ALLOCATION_DESC allocation_desc = *const_cast<D3D12MA::ALLOCATION_DESC *> (
      reinterpret_cast<const D3D12MA::ALLOCATION_DESC *> (pAllocDesc));
  if (pAllocDesc->CustomPool != nullptr)
    {
      allocation_desc.CustomPool =
          reinterpret_cast<D3D12MA::Pool *> (pAllocDesc->CustomPool->pInstance);
    }

  D3D12MA::Allocation *allocation = nullptr;
  HRESULT hResult =
      reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
          ->CreateResource (&allocation_desc, pResourceDesc, InitialResourceState,
                            pOptimizedClearValue, &allocation, iidResource, ppvResource);
  *ppAllocation = static_cast<CD3D12MAAllocation *> (malloc (sizeof (CD3D12MAAllocation)));
  (*ppAllocation)->pVtbl = &g_CD3D12MAAllocationVtbl;
  (*ppAllocation)->pInstance = allocation;

  return hResult;
}

#ifdef __ID3D12Device4_INTERFACE_DEFINED__
static HRESULT
CD3D12MAAllocatorCreateResource1 (CD3D12MAAllocator *pAllocator,
                                  const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
                                  const D3D12_RESOURCE_DESC *pResourceDesc,
                                  D3D12_RESOURCE_STATES InitialResourceState,
                                  const D3D12_CLEAR_VALUE *pOptimizedClearValue,
                                  ID3D12ProtectedResourceSession *pProtectedSession,
                                  CD3D12MAAllocation **ppAllocation,
                                  REFIID iidResource,
                                  void **ppvResource)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::ALLOCATION_DESC allocation_desc = *const_cast<D3D12MA::ALLOCATION_DESC *> (
      reinterpret_cast<const D3D12MA::ALLOCATION_DESC *> (pAllocDesc));
  if (pAllocDesc->CustomPool != nullptr)
    {
      allocation_desc.CustomPool =
          reinterpret_cast<D3D12MA::Pool *> (pAllocDesc->CustomPool->pInstance);
    }

  D3D12MA::Allocation *allocation = nullptr;
  HRESULT hResult = reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
                        ->CreateResource1 (&allocation_desc, pResourceDesc, InitialResourceState,
                                           pOptimizedClearValue, pProtectedSession, &allocation,
                                           iidResource, ppvResource);
  *ppAllocation = static_cast<CD3D12MAAllocation *> (malloc (sizeof (CD3D12MAAllocation)));
  (*ppAllocation)->pVtbl = &g_CD3D12MAAllocationVtbl;
  (*ppAllocation)->pInstance = allocation;

  return hResult;
}
#endif

#ifdef __ID3D12Device8_INTERFACE_DEFINED__
static HRESULT
CD3D12MAAllocatorCreateResource2 (CD3D12MAAllocator *pAllocator,
                                  const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
                                  const D3D12_RESOURCE_DESC1 *pResourceDesc,
                                  D3D12_RESOURCE_STATES InitialResourceState,
                                  const D3D12_CLEAR_VALUE *pOptimizedClearValue,
                                  ID3D12ProtectedResourceSession *pProtectedSession,
                                  CD3D12MAAllocation **ppAllocation,
                                  REFIID iidResource,
                                  void **ppvResource)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::ALLOCATION_DESC allocation_desc = *const_cast<D3D12MA::ALLOCATION_DESC *> (
      reinterpret_cast<const D3D12MA::ALLOCATION_DESC *> (pAllocDesc));
  if (pAllocDesc->CustomPool != nullptr)
    {
      allocation_desc.CustomPool =
          reinterpret_cast<D3D12MA::Pool *> (pAllocDesc->CustomPool->pInstance);
    }

  D3D12MA::Allocation *allocation = nullptr;
  HRESULT hResult = reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
                        ->CreateResource2 (&allocation_desc, pResourceDesc, InitialResourceState,
                                           pOptimizedClearValue, pProtectedSession, &allocation,
                                           iidResource, ppvResource);
  *ppAllocation = static_cast<CD3D12MAAllocation *> (malloc (sizeof (CD3D12MAAllocation)));
  (*ppAllocation)->pVtbl = &g_CD3D12MAAllocationVtbl;
  (*ppAllocation)->pInstance = allocation;

  return hResult;
}
#endif

static HRESULT
CD3D12MAAllocatorAllocateMemory (CD3D12MAAllocator *pAllocator,
                                 const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
                                 const D3D12_RESOURCE_ALLOCATION_INFO *pAllocInfo,
                                 CD3D12MAAllocation **ppAllocation)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::ALLOCATION_DESC allocation_desc = *const_cast<D3D12MA::ALLOCATION_DESC *> (
      reinterpret_cast<const D3D12MA::ALLOCATION_DESC *> (pAllocDesc));
  if (pAllocDesc->CustomPool != nullptr)
    {
      allocation_desc.CustomPool =
          reinterpret_cast<D3D12MA::Pool *> (pAllocDesc->CustomPool->pInstance);
    }

  D3D12MA::Allocation *allocation = nullptr;
  HRESULT hResult = reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
                        ->AllocateMemory (&allocation_desc, pAllocInfo, &allocation);
  *ppAllocation = static_cast<CD3D12MAAllocation *> (malloc (sizeof (CD3D12MAAllocation)));
  (*ppAllocation)->pVtbl = &g_CD3D12MAAllocationVtbl;
  (*ppAllocation)->pInstance = allocation;

  return hResult;
}

#ifdef __ID3D12Device4_INTERFACE_DEFINED__
static HRESULT
CD3D12MAAllocatorAllocateMemory1 (CD3D12MAAllocator *pAllocator,
                                  const CD3D12MA_ALLOCATION_DESC *pAllocDesc,
                                  const D3D12_RESOURCE_ALLOCATION_INFO *pAllocInfo,
                                  ID3D12ProtectedResourceSession *pProtectedSession,
                                  CD3D12MAAllocation **ppAllocation)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::ALLOCATION_DESC allocation_desc = *const_cast<D3D12MA::ALLOCATION_DESC *> (
      reinterpret_cast<const D3D12MA::ALLOCATION_DESC *> (pAllocDesc));
  if (pAllocDesc->CustomPool != nullptr)
    {
      allocation_desc.CustomPool =
          reinterpret_cast<D3D12MA::Pool *> (pAllocDesc->CustomPool->pInstance);
    }

  D3D12MA::Allocation *allocation = nullptr;
  HRESULT hResult =
      reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
          ->AllocateMemory1 (&allocation_desc, pAllocInfo, pProtectedSession, &allocation);
  *ppAllocation = static_cast<CD3D12MAAllocation *> (malloc (sizeof (CD3D12MAAllocation)));
  (*ppAllocation)->pVtbl = &g_CD3D12MAAllocationVtbl;
  (*ppAllocation)->pInstance = allocation;

  return hResult;
}
#endif

static HRESULT
CD3D12MAAllocatorCreateAliasingResource (CD3D12MAAllocator *pAllocator,
                                         CD3D12MAAllocation *pAllocation,
                                         UINT64 AllocationLocalOffset,
                                         const D3D12_RESOURCE_DESC *pResourceDesc,
                                         D3D12_RESOURCE_STATES InitialResourceState,
                                         const D3D12_CLEAR_VALUE *pOptimizedClearValue,
                                         REFIID iidResource,
                                         void **ppvResource)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  HRESULT hResult = reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
                        ->CreateAliasingResource (
                            reinterpret_cast<D3D12MA::Allocation *> (pAllocation->pInstance),
                            AllocationLocalOffset, pResourceDesc, InitialResourceState,
                            pOptimizedClearValue, iidResource, ppvResource);

  return hResult;
}

static HRESULT
CD3D12MAAllocatorCreatePool (CD3D12MAAllocator *pAllocator,
                             const CD3D12MA_POOL_DESC *pPoolDesc,
                             CD3D12MAPool **ppPool)
{
  if (!pAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::Pool *pool = nullptr;
  HRESULT hResult =
      reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
          ->CreatePool (reinterpret_cast<const D3D12MA::POOL_DESC *> (pPoolDesc), &pool);
  *ppPool = static_cast<CD3D12MAPool *> (malloc (sizeof (CD3D12MAPool)));
  (*ppPool)->pVtbl = &g_CD3D12MAPoolVtbl;
  (*ppPool)->pInstance = pool;

  return hResult;
}

static void
CD3D12MAAllocatorSetCurrentFrameIndex (CD3D12MAAllocator *pAllocator, UINT frameIndex)
{
  reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)
      ->SetCurrentFrameIndex (frameIndex);
}

static void
CD3D12MAAllocatorCalculateStats (CD3D12MAAllocator *pAllocator, CD3D12MA_STATS *pStats)
{
  D3D12MA::Stats stats = {};
  reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->CalculateStats (&stats);
  *pStats = *reinterpret_cast<CD3D12MA_STATS *> (&stats);
}

static void
CD3D12MAAllocatorGetBudget (CD3D12MAAllocator *pAllocator,
                            CD3D12MA_BUDGET *pGpuBudget,
                            CD3D12MA_BUDGET *pCpuBudget)
{
  D3D12MA::Budget gpu = {};
  D3D12MA::Budget cpu = {};
  reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->GetBudget (&gpu, &cpu);
  *pGpuBudget = *reinterpret_cast<CD3D12MA_BUDGET *> (&gpu);
  *pCpuBudget = *reinterpret_cast<CD3D12MA_BUDGET *> (&cpu);
}

static const CD3D12MAAllocatorVtbl g_CD3D12MAAllocatorVtbl = {
  CD3D12MAAllocatorRelease,
  CD3D12MAAllocatorGetD3D12Options,
  CD3D12MAAllocatorIsUMA,
  CD3D12MAAllocatorIsCacheCoherentUMA,
  CD3D12MAAllocatorCreateResource
#ifdef __ID3D12Device4_INTERFACE_DEFINED__
  ,
  CD3D12MAAllocatorCreateResource1
#endif

#ifdef __ID3D12Device8_INTERFACE_DEFINED__
  ,
  CD3D12MAAllocatorCreateResource2
#endif
  ,
  CD3D12MAAllocatorAllocateMemory
#ifdef __ID3D12Device4_INTERFACE_DEFINED__
  ,
  CD3D12MAAllocatorAllocateMemory1
#endif
  ,
  CD3D12MAAllocatorCreateAliasingResource,
  CD3D12MAAllocatorCreatePool,
  CD3D12MAAllocatorSetCurrentFrameIndex,
  CD3D12MAAllocatorCalculateStats,
  CD3D12MAAllocatorGetBudget
};

HRESULT
CreateAllocator (const CD3D12MA_ALLOCATOR_DESC *pDesc, CD3D12MAAllocator **ppAllocator)
{
  if (!ppAllocator)
    {
      return E_INVALIDARG;
    }

  D3D12MA::Allocator *allocator;
  HRESULT hResult = D3D12MA::CreateAllocator (
      reinterpret_cast<const D3D12MA::ALLOCATOR_DESC *> (pDesc), &allocator);

  *ppAllocator = static_cast<CD3D12MAAllocator *> (malloc (sizeof (CD3D12MAAllocator)));
  (*ppAllocator)->pInstance = allocator;
  (*ppAllocator)->pVtbl = &g_CD3D12MAAllocatorVtbl;

  return hResult;
}