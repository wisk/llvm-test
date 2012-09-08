#include <iostream>
#include <string>
#include <exception>

#include <llvm/Support/TargetSelect.h>
#include <llvm/LLVMContext.h>

#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/IRBuilder.h>

#include <llvm/Function.h>
#include <llvm/BasicBlock.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>

#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetData.h>

#include "llvm/ExecutionEngine/JIT.h"

typedef char s8;
typedef unsigned char u8;

typedef unsigned int u32;
typedef unsigned __int64 u64;

using namespace llvm;

class LlvmJitter
{
public:
  typedef void *CpuContextObject;
  typedef void (*ReadRegisterPtr)(u32 Register, void *pData, size_t Size);
  typedef void (*WriteRegisterPtr)(u32 Register, void const*pData, size_t Size);

  typedef void *MemoryContextObject;
  typedef void (*ReadMemoryPtr)(void *pAddress, void *pData, size_t Size);
  typedef void (*WriteMemoryPtr)(void *pAddress, void const* pData, size_t Size);

  typedef void (*ExecuteCodePtr)(
    CpuContextObject pCpuCtxtObj, ReadRegisterPtr pReadReg, WriteRegisterPtr pWriteReg,
    MemoryContextObject pMemCtxtObj, ReadMemoryPtr pMemRead, WriteMemoryPtr pWriteMem
  );

  virtual bool GenerateCode(u8* pCode, ExecuteCodePtr &ExecCode) = 0;

protected:

private:
};

int main(void)
{
  try
  {
    InitializeNativeTarget();
    LLVMContext &rCtxt = getGlobalContext();
    IRBuilder<> Builder(rCtxt);
    std::string ErrStr;

    Module *pModule = new Module("llvm-test_emu-and-jit", rCtxt);
    ExecutionEngine *pExecutionEngine = EngineBuilder(pModule).setErrorStr(&ErrStr).create();
    if (pExecutionEngine == nullptr) throw ErrStr;

    TargetData CurTargetData(pModule);
  }
  catch (std::exception const& rExcpt)
  {
    std::cerr << rExcpt.what() << std::endl;
    return 1;
  }

  return 0;
}