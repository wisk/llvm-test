#include <iostream>
#include <string>
#include <exception>

#include <llvm/Support/TargetSelect.h>
#include <llvm/LLVMContext.h>

#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IRBuilder.h>
#include <llvm/PassManager.h>

#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>

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

    FunctionPassManager FuncPassMgr(sm_pModule);
    FuncPassMgr.add(new TargetData(*sm_pExecutionEngine->getTargetData()));
    FuncPassMgr.add(createBasicAliasAnalysisPass());
    FuncPassMgr.add(createInstructionCombiningPass());
    FuncPassMgr.add(createReassociatePass());
    FuncPassMgr.add(createGVNPass());
    FuncPassMgr.add(createCFGSimplificationPass());
    FuncPassMgr.add(createPromoteMemoryToRegisterPass());

    FuncPassMgr.doInitialization();
  }

  virtual bool GenerateCode(u8 const* pCode, size_t SizeOfCode) = 0;

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
      y,
      pc,
      sp;
    u8
      b,
      p;

    void Dump(std::ostream& os) const
    {
      os << std::hex
        << " a: "  << static_cast<int>(a)
        << " b: "  << static_cast<int>(b)
        << " x: "  << static_cast<int>(x)
        << " y: "  << static_cast<int>(y)
        << " p: "  << static_cast<int>(p)
        << " pc: " << static_cast<int>(pc)
        << " sp: " << static_cast<int>(sp)
        << std::endl;
    }
  };

  enum
  {
    REG_A,
    REG_B,
    REG_X,
    REG_Y,
    REG_P,
    REG_PC,
    REG_SP
  };

  static void ReadRegister(void* pCpuCtxtObj, u32 Register, void* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
    auto pCpuCtxt = reinterpret_cast<CpuContext*>(pCpuCtxtObj);

    switch (Register)
    {
    case REG_A:  memcpy(pData, &pCpuCtxt->a,  Size); break;
    case REG_B:  memcpy(pData, &pCpuCtxt->b,  Size); break;
    case REG_X:  memcpy(pData, &pCpuCtxt->x,  Size); break;
    case REG_Y:  memcpy(pData, &pCpuCtxt->y,  Size); break;
    case REG_P:  memcpy(pData, &pCpuCtxt->p,  Size); break;
    case REG_PC: memcpy(pData, &pCpuCtxt->pc, Size); break;
    case REG_SP: memcpy(pData, &pCpuCtxt->sp, Size); break;
    default: assert(0 && "Unknown register!");
    }
  }

  static void WriteRegister(void* pCpuCtxtObj, u32 Register, void const* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called" << std::endl;
    auto pCpuCtxt = reinterpret_cast<CpuContext*>(pCpuCtxtObj);

    switch (Register)
    {
    case REG_A:  memcpy(&pCpuCtxt->a,  pData, Size); break;
    case REG_B:  memcpy(&pCpuCtxt->b,  pData, Size); break;
    case REG_X:  memcpy(&pCpuCtxt->x,  pData, Size); break;
    case REG_Y:  memcpy(&pCpuCtxt->y,  pData, Size); break;
    case REG_P:  memcpy(&pCpuCtxt->p,  pData, Size); break;
    case REG_PC: memcpy(&pCpuCtxt->pc, pData, Size); break;
    case REG_SP: memcpy(&pCpuCtxt->sp, pData, Size); break;
    default: assert(0 && "Unknown register!");
    }
  }

  static void ReadMemory(void* pMemCtxtObj, void* pAddress, void* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called: Address: " << pAddress << ", Data: " << pData << ", Size: " << Size << std::endl;

    memset(pData, 0xbb, Size);
  }

  static void WriteMemory(void* pMemCtxtObj, void* pAddress, void const* pData, u32 Size)
  {
    std::cout << __FUNCTION__ " called: Address: " << pAddress << ", Data: " << pData << ", Size: " << Size << std::endl;
  }

public:
  virtual bool GenerateCode(u8 const* pCode, size_t SizeOfCode)
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

      //auto pRegA = GetRegister(pCpuReadVal, pCpuCtxtObjVal, REG_A);
      //SetRegister(pCpuWriteVal, pCpuCtxtObjVal, REG_X, pRegA);

      CpuContext CpuCtxt;
      memset(&CpuCtxt, 0x0, sizeof(CpuCtxt));
      CpuCtxt.a = 0x1234;

      while (SizeOfCode--)
      {
        switch (*pCode++)
        {
#include "insn.ipp"
        }
        Add(pCpuReadVal, pCpuWriteVal, pCpuCtxtObjVal, REG_PC, 1);
      }

      m_Builder.CreateRetVoid();
      pExecFunc->dump();

      auto pJitFunction = reinterpret_cast<LlvmJitter::ExecuteCodePtr>(sm_pExecutionEngine->getPointerToFunction(pExecFunc));

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
    u32 Size = GetRegisterSize(Register);
    LLVMContext &rCtxt = getGlobalContext();

    auto pRegAlloca = m_Builder.CreateAlloca(Size == 1 ? Type::getInt8Ty(rCtxt) : Type::getInt16Ty(rCtxt));
    auto pRegBuf    = m_Builder.CreateBitCast(pRegAlloca, Type::getInt8PtrTy(rCtxt));
    auto pSzVal     = ConstantInt::get(rCtxt, APInt(32, Size));
    auto pRegVal    = ConstantInt::get(rCtxt, APInt(32, Register));

    m_Builder.CreateCall4(pCpuReadVal, pCpuCtxtObjVal, pRegVal, pRegBuf, pSzVal);
    return pRegBuf;
  }

  u32 GetRegisterSize(u32 Register)
  {
    if (Register == REG_B || Register == REG_P) return 1;
    return 2;
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

  void ReadMemory(Value* pMemReadVal, Value* pMemCtxtObjVal, Value* pAddrVal, Value* pMemBuf, u32 Size)
  {
    LLVMContext &rCtxt = getGlobalContext();

    auto pSzVal        = ConstantInt::get(rCtxt, APInt(32, Size));

    auto pAddrPtr      = m_Builder.CreateBitCast(pAddrVal, Type::getInt16PtrTy(rCtxt));
    auto pAddr         = m_Builder.CreateLoad(pAddrPtr, false);
    auto pAddrMem      = m_Builder.CreateIntToPtr(pAddr, Type::getInt8PtrTy(rCtxt));

    m_Builder.CreateCall4(pMemReadVal, pMemCtxtObjVal, pAddrMem, pMemBuf, pSzVal);
  }

  void WriteMemory(Value* pMemWriteVal, Value* pMemCtxtObjVal, Value* pAddrVal, Value* pMemBuf, u32 Size)
  {
    LLVMContext &rCtxt = getGlobalContext();

    auto pSzVal = ConstantInt::get(rCtxt, APInt(32, Size));

    m_Builder.CreateCall4(pMemWriteVal, pMemCtxtObjVal, pAddrVal, pMemBuf, pSzVal);
  }

  void Add(Value* pCpuReadVal, Value* pCpuWriteVal, Value* pCpuCtxtObjVal, u32 Register, u16 Val)
  {
    auto pRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, Register);
    auto pRegPtr = m_Builder.CreateBitCast(pRegBuf, Type::getInt16PtrTy(getGlobalContext()));
    auto pReg    = m_Builder.CreateLoad(pRegPtr, false);
    auto pRes    = m_Builder.CreateAdd(pReg, ConstantInt::get(getGlobalContext(), APInt(16, Val)));
    m_Builder.CreateStore(pRes, pRegPtr, false);
    SetRegister(pCpuWriteVal, pCpuCtxtObjVal, Register, pRegBuf);
  }

  void Sub(Value* pCpuReadVal, Value* pCpuWriteVal, Value* pCpuCtxtObjVal, u32 Register, u16 Val)
  {
    auto pRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, Register);
    auto pRegPtr = m_Builder.CreateBitCast(pRegBuf, Type::getInt16PtrTy(getGlobalContext()));
    auto pReg    = m_Builder.CreateLoad(pRegPtr, false);
    auto pRes    = m_Builder.CreateSub(pReg, ConstantInt::get(getGlobalContext(), APInt(16, Val)));
    auto pStoReg = m_Builder.CreateStore(pRes, pRegPtr, false);
    SetRegister(pCpuWriteVal, pCpuCtxtObjVal, Register, pRegBuf);
  }

  void Transfer(Value* pCpuReadVal, Value* pCpuWriteVal, Value* pCpuCtxtObjVal, u32 SrcReg, u32 DstReg)
  {
    auto pSrcRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, SrcReg);
    SetRegister(pCpuWriteVal, pCpuCtxtObjVal, DstReg, pSrcRegBuf);
  }

  void Push(Value* pCpuReadVal, Value* pCpuWriteVal, Value* pCpuCtxtObjVal, Value* pMemWriteVal, Value* pMemCtxtObjVal, u32 Reg)
  {
    LLVMContext &rCtxt = getGlobalContext();

    Sub(pCpuReadVal, pCpuWriteVal, pCpuCtxtObjVal, REG_SP, 2);
    auto pStackRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, REG_SP);
    auto pStackRegPtr = m_Builder.CreateBitCast(pStackRegBuf, Type::getInt16PtrTy(rCtxt));
    auto pStackReg    = m_Builder.CreateLoad(pStackRegPtr, false);
    auto pStack       = m_Builder.CreateIntToPtr(pStackReg, Type::getInt8PtrTy(rCtxt));

    auto pRegBuf      = GetRegister(pCpuReadVal, pCpuCtxtObjVal, Reg);

    WriteMemory(pMemWriteVal, pMemCtxtObjVal, pStack, pRegBuf, GetRegisterSize(Reg));
  }

  void Pull(Value* pCpuReadVal, Value* pCpuWriteVal, Value* pCpuCtxtObjVal, Value* pMemReadVal, Value* pMemCtxtObjVal, u32 Reg)
  {
    LLVMContext &rCtxt = getGlobalContext();

    auto pRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, Reg);
    auto pStackRegBuf = GetRegister(pCpuReadVal, pCpuCtxtObjVal, REG_SP);
    auto pAlloca = m_Builder.CreateBitCast(m_Builder.CreateAlloca(Type::getInt16Ty(rCtxt)), Type::getInt8PtrTy(rCtxt));
    ReadMemory(pMemReadVal, pMemCtxtObjVal, pStackRegBuf, pAlloca, GetRegisterSize(Reg));
    SetRegister(pCpuWriteVal, pCpuCtxtObjVal, Reg, pAlloca);

    Add(pCpuReadVal, pCpuWriteVal, pCpuCtxtObjVal, REG_SP, 2);
  }
};

int main(void)
{
  MyLlvmJitter Jit;
  auto pCode = reinterpret_cast<u8 const*>("\xee\xaa\xce\xa8\xee\x48\xab"); /* ina tax dea tay ina pha plb*/
  size_t SizeOfCode = 7;
  Jit.GenerateCode(pCode, SizeOfCode);
}