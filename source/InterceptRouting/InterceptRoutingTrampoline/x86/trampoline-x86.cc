#include "common/macros/platform_macro.h"
#if defined(TARGET_ARCH_IA32)

#include "dobby_internal.h"

#include "core/modules/assembler/assembler-ia32.h"
#include "core/modules/codegen/codegen-ia32.h"

#include "InstructionRelocation/x86/X86InstructionRelocation.h"

#include "InterceptRouting/ExtraInternalPlugin/NearBranchTrampoline/NearMemoryArena.h"
#include "InterceptRouting/ExtraInternalPlugin/RegisterPlugin.h"

using namespace zz::x86;

static void **AllocIndirectStub(addr_t branch_address) {
  WritableDataChunk *forwardStub = NULL;

  forwardStub =
      NearMemoryArena::AllocateDataChunk((addr_t)branch_address, (size_t)2 * 1024 * 1024 * 1024, (int)sizeof(void *));
  if (forwardStub == nullptr) {
    ERROR_LOG("Not found near forward stub");
    return NULL;
  }

  return (void **)forwardStub->address;
}

CodeBufferBase *GenerateNormalTrampolineBuffer(addr_t from, addr_t to) {
  CodeBufferBase *result = NULL;

  DLOG("Generate trampoline => %p", to);

  TurboAssembler turbo_assembler_((void *)from);
#define _ turbo_assembler_.

  // branch
  void **branch_stub = AllocIndirectStub(from);
  *branch_stub       = (void *)to;

  CodeGen codegen(&turbo_assembler_);
  codegen.JmpNearIndirect((uint64_t)branch_stub);

  result = turbo_assembler_.GetCodeBuffer()->copy();
  return result;
}

CodeBufferBase *GenerateNearTrampolineBuffer(InterceptRouting *routing, addr_t src, addr_t dst) {
  DLOG("x86 near branch trampoline enable default");
  return NULL;
}

#endif