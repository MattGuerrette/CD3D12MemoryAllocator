
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

#define CD3D12MA_NEW(allocs, type) new (D3D12MA::Allocate<type> (allocs)) (type)
#define CD3D12MA_NEW_ARRAY(allocs, type, count)                                                   \
  new (D3D12MA::AllocateArray<type> ((allocs), (count))) (type)

static void
CD3D12MAAllocatorRelease (CD3D12MAAllocator *pAllocator)
{
  reinterpret_cast<D3D12MA::Allocator *> (pAllocator->pInstance)->Release ();

  free (pAllocator);
}

static const CD3D12MAAllocatorVtbl g_CD3D12MAAllocatorVtbl = { CD3D12MAAllocatorRelease };

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