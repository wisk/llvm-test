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

  LlvmJitter(void) : m_Builder(getGlobalContext())
  {
    InitializeNativeTarget();
    LLVMContext &rCtxt = getGlobalContext();
    std::string ErrStr;

    if (sm_pModule          == nullptr) sm_pModule          = new Module("llvm-test_emu-and-jit", rCtxt);
    if (sm_pExecutionEngine == nullptr) sm_pExecutionEngine = EngineBuilder(sm_pModule).setErrorStr(&ErrStr).create();
    if (sm_pExecutionEngine == nullptr) throw ErrStr;
    if (sm_pTargetData      == nullptr) sm_pTargetData      = new TargetData(sm_pModule);
  }

  virtual bool GenerateCode(u8* pCode) = 0;

protected:
  IRBuilder<> m_Builder;
  static Module* sm_pModule;
  static ExecutionEngine* sm_pExecutionEngine;
  static TargetData* sm_pTargetData;

private:
};

Module*          LlvmJitter::sm_pModule          = nullptr;
ExecutionEngine* LlvmJitter::sm_pExecutionEngine = nullptr;
TargetData*      LlvmJitter::sm_pTargetData      = nullptr;

/* based on 65816 arch */
class MyLlvmJitter : public LlvmJitter
{
private:
  struct CpuContext
  {
    u16
      a,
      x,
      y;
    u8
      b,
      p;

    void Dump(std::ostream& os) const
    {
      os << std::hex
        << " a: " << static_cast<int>(a)
        << " b: " << static_cast<int>(b)
        << " x: " << static_cast<int>(x)
        << " y: " << static_cast<int>(y)
        << " p: " << static_cast<int>(p)
        << std::endl;
    }
  };

  enum
  {
    REG_A,
    REG_B,
    REG_X,
    REG_Y,
    REG_P
  };

  static void ReadRegister(void* pCpuCtxtObj, u32 Register, void* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
    auto pCpuCtxt = reinterpret_cast<CpuContext*>(pCpuCtxtObj);

    switch (Register)
    {
    case REG_A: memcpy(pData, &pCpuCtxt->a, Size); break;
    case REG_B: memcpy(pData, &pCpuCtxt->b, Size); break;
    case REG_X: memcpy(pData, &pCpuCtxt->x, Size); break;
    case REG_Y: memcpy(pData, &pCpuCtxt->y, Size); break;
    case REG_P: memcpy(pData, &pCpuCtxt->p, Size); break;
    default: assert(0 && "Unknown register!");
    }
  }

  static void WriteRegister(void* pCpuCtxtObj, u32 Register, void const* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
    auto pCpuCtxt = reinterpret_cast<CpuContext*>(pCpuCtxtObj);

    switch (Register)
    {
    case REG_A: memcpy(&pCpuCtxt->a, pData, Size); break;
    case REG_B: memcpy(&pCpuCtxt->b, pData, Size); break;
    case REG_X: memcpy(&pCpuCtxt->x, pData, Size); break;
    case REG_Y: memcpy(&pCpuCtxt->y, pData, Size); break;
    case REG_P: memcpy(&pCpuCtxt->p, pData, Size); break;
    default: assert(0 && "Unknown register!");
    }
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
      LLVMContext &rCtxt = getGlobalContext();
      auto pVoidTy       = Type::getVoidTy(rCtxt);
      auto pInt32Ty      = Type::getInt32Ty(rCtxt);
      auto pVoidPtrTy    = Type::getInt8PtrTy(rCtxt);

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

      auto pExecFunc           = Function::Create(pExecFuncTy, GlobalValue::ExternalLinkage, "execute", sm_pModule);

      auto pBbEntry            = BasicBlock::Create(rCtxt, "entry", pExecFunc);

      m_Builder.SetInsertPoint(pBbEntry);

      auto itArg = pExecFunc->arg_begin();
      Value* pCpuCtxtObjVal = itArg++;
      Value* pCpuReadVal    = itArg++;
      Value* pCpuWriteVal   = itArg++;
      Value* pMemCtxtObjVal = itArg++;
      Value* pMemReadVal    = itArg++;
      Value* pMemWriteVal   = itArg++;

      auto pRegA = GetRegister(pCpuReadVal, pCpuCtxtObjVal, REG_A);
      SetRegister(pCpuWriteVal, pCpuCtxtObjVal, REG_X, pRegA);

      m_Builder.CreateRetVoid();
      auto pJitFunction = reinterpret_cast<LlvmJitter::ExecuteCodePtr>(sm_pExecutionEngine->getPointerToFunction(pExecFunc));
      pExecFunc->dump();

      CpuContext CpuCtxt;
      memset(&CpuCtxt, 0x0, sizeof(CpuCtxt));
      CpuCtxt.a = 0x1234;

      CpuCtxt.Dump(std::cout);
      pJitFunction(&CpuCtxt, ReadRegister, WriteRegister, nullptr, ReadMemory, WriteMemory);
      CpuCtxt.Dump(std::cout);
    }
    catch (std::exception const& rExcpt)
    {
      std::cerr << rExcpt.what() << std::endl;
      return false;
    }

    return true;
  }

private:

  Value* GetRegister(Value* pCpuReadVal, Value* pCpuCtxtObjVal, u32 Register)
  {
    u32 Size = 2;
    LLVMContext &rCtxt = getGlobalContext();

    if (Register == REG_B || Register == REG_P)
      Size = 1;

    auto pRegAlloca = m_Builder.CreateAlloca(Size == 1 ? Type::getInt8Ty(rCtxt) : Type::getInt16Ty(rCtxt));
    auto pRegBuf    = m_Builder.CreateBitCast(pRegAlloca, Type::getInt8PtrTy(rCtxt));
    auto pSzVal     = ConstantInt::get(getGlobalContext(), APInt(32, Size));
    auto pRegVal    = ConstantInt::get(getGlobalContext(), APInt(32, Register));

    m_Builder.CreateCall4(pCpuReadVal, pCpuCtxtObjVal, pRegVal, pRegBuf, pSzVal);
    return pRegBuf;
  }

  void SetRegister(Value* pCpuWriteVal, Value* pCpuCtxtObjVal, u32 Register, Value* pNewValue)
  {
    u32 Size = 2;
    LLVMContext &rCtxt = getGlobalContext();

    if (Register == REG_B || Register == REG_P)
      Size = 1;

    auto pSzVal     = ConstantInt::get(getGlobalContext(), APInt(32, Size));
    auto pRegVal    = ConstantInt::get(getGlobalContext(), APInt(32, Register));

    m_Builder.CreateCall4(pCpuWriteVal, pCpuCtxtObjVal, pRegVal, pNewValue, pSzVal);
  }
};

int main(void)
{
  MyLlvmJitter Jit;
  Jit.GenerateCode(nullptr);
}