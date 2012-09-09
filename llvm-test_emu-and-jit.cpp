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

#include <stdint.h>

typedef int8_t  s8;
typedef uint8_t u8;

typedef int16_t s16;
typedef uint16_t u16;

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

using namespace llvm;

class LlvmJitter
{
public:
  typedef void *CpuContextObject;
  typedef void (*ReadRegisterPtr)(void *pCpuCtxtObj, u32 Register, void *pData, u32 Size);
  typedef void (*WriteRegisterPtr)(void *pCpuCtxtObj, u32 Register, void const*pData, u32 Size);

  typedef void *MemoryContextObject;
  typedef void (*ReadMemoryPtr)(void* pMemCtxtObj, void *pAddress, void *pData, u32 Size);
  typedef void (*WriteMemoryPtr)(void* pMemCtxtObj, void *pAddress, void const* pData, u32 Size);

  typedef void (*ExecuteCodePtr)(
    CpuContextObject    pCpuCtxtObj, ReadRegisterPtr pReadReg, WriteRegisterPtr pWriteReg,
    MemoryContextObject pMemCtxtObj, ReadMemoryPtr   pMemRead, WriteMemoryPtr   pWriteMem
  );

  virtual bool GenerateCode(u8* pCode) = 0;

protected:

private:
};

/* based on 65816 arch */
class MyLlvmJitter
{
private:
  struct CpuContext
  {
    u16 a, x, y;
    u8 b, p;
  };

  enum
  {
    REG_A,
    REG_B,
    REG_X,
    REG_Y,
    REP_P
  };

  static void ReadRegister(void* pCpuCtxtObj, u32 Register, void* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
    auto pCpuCtxt = reinterpret_cast<CpuContext*>(pCpuCtxtObj);
  }

  static void WriteRegister(void* pCpuCtxtObj, u32 Register, void const* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
  }

  static void ReadMemory(void* pMemCtxtObj, void* pAddress, void* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
  }

  static void WriteMemory(void* pMemCtxtObj, void* pAddress, void const* pData, u32)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
  }

public:
  virtual bool GenerateCode(u8* pCode)
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

      auto pVoidTy             = Type::getVoidTy(rCtxt);
      auto pInt32Ty            = Type::getInt32Ty(rCtxt);
      auto pVoidPtrTy          = Type::getInt8PtrTy(rCtxt);

      std::vector<Type *> RegParams, MemParams, ExcParams;

      RegParams.push_back(pVoidPtrTy);
      RegParams.push_back(pInt32Ty);
      RegParams.push_back(pVoidPtrTy);
      RegParams.push_back(pInt32Ty);
      auto pAccessRegFuncTy    = FunctionType::get(pVoidTy, RegParams, false);
      auto pAccessRegFuncPtrTy = PointerType::getUnqual(pAccessRegFuncTy);

      MemParams.push_back(pVoidPtrTy);
      MemParams.push_back(pVoidPtrTy);
      MemParams.push_back(pVoidPtrTy);
      MemParams.push_back(pInt32Ty);
      auto pAccessMemFuncTy    = FunctionType::get(pVoidTy, MemParams, false);
      auto pAccessMemFuncPtrTy = PointerType::getUnqual(pAccessMemFuncTy);

      ExcParams.push_back(pVoidPtrTy);
      ExcParams.push_back(pAccessRegFuncPtrTy);
      ExcParams.push_back(pAccessRegFuncPtrTy);
      ExcParams.push_back(pVoidPtrTy);
      ExcParams.push_back(pAccessMemFuncPtrTy);
      ExcParams.push_back(pAccessMemFuncPtrTy);
      auto pExecFuncTy         = FunctionType::get(pVoidTy, ExcParams, false);

      //auto pAccessRegFunc      = Function::Create(pAccessRegFuncTy, GlobalValue::ExternalLinkage, "access_register", pModule);
      //auto pAccessMemFunc      = Function::Create(pAccessMemFuncTy, GlobalValue::ExternalLinkage, "access_memory",   pModule);
      auto pExecFunc           = Function::Create(pExecFuncTy,      GlobalValue::ExternalLinkage, "execute",         pModule);

      auto pBbEntry            = BasicBlock::Create(rCtxt, "entry", pExecFunc);

      Builder.SetInsertPoint(pBbEntry);

      auto itArg = pExecFunc->arg_begin();
      Value* pCpuCtxtObjVal = itArg++;
      Value* pCpuReadVal    = itArg++;
      Value* pCpuWriteVal   = itArg++;
      Value* pMemCtxtObjVal = itArg++;
      Value* pMemReadVal    = itArg++;
      Value* pMemWriteVal   = itArg++;

      auto pRegAlloca = Builder.CreateAlloca(pInt32Ty);
      auto pRegVal = Builder.CreateBitCast(pRegAlloca, pVoidPtrTy);
      Builder.CreateCall4(pCpuReadVal, pCpuCtxtObjVal, ConstantInt::get(rCtxt, APInt(32, 0)), pRegVal, ConstantInt::get(rCtxt, APInt(32, 32)));

      Builder.CreateRetVoid();
      auto pJitFunction = reinterpret_cast<LlvmJitter::ExecuteCodePtr>(pExecutionEngine->getPointerToFunction(pExecFunc));
      pExecFunc->dump();

      CpuContext CpuCtxt;

      pJitFunction(&CpuCtxt, ReadRegister, WriteRegister, nullptr, ReadMemory, WriteMemory);
    }
    catch (std::exception const& rExcpt)
    {
      std::cerr << rExcpt.what() << std::endl;
      return false;
    }

    return true;
  }

private:
};

int main(void)
{
  MyLlvmJitter Jit;
  Jit.GenerateCode(nullptr);
}