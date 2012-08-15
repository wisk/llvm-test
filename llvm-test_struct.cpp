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

#include "llvm/ExecutionEngine/JIT.h"

using namespace llvm;

struct CpuContext
{
  unsigned int Reg[10];
};

int main(void)
{
  try
  {
    InitializeNativeTarget();
    LLVMContext &rCtxt = getGlobalContext();
    IRBuilder<> Builder(rCtxt);
    std::string ErrStr;

    Module *pModule = new Module("llvm-test_struct", rCtxt);
    ExecutionEngine *pExecutionEngine = EngineBuilder(pModule).setErrorStr(&ErrStr).create();
    if (pExecutionEngine == nullptr) throw ErrStr;

    std::vector<Type *> StructField;
    for (auto i = 0; i < 10; ++i) StructField.push_back(IntegerType::get(rCtxt, 32));

    StructType   *pStructType          = StructType::create(StructField, "CpuContext");
    PointerType  *pCpuContextStructPtr = PointerType::get(pStructType, 0);
    FunctionType *pFuncType            = FunctionType::get(Type::getVoidTy(rCtxt), pCpuContextStructPtr, false);

    auto pMemsetFunc = pModule->getFunction("memset");
    if (pMemsetFunc == nullptr)
    {
      std::vector<Type *> MemsetParams;
      MemsetParams.push_back(PointerType::get(IntegerType::get(rCtxt, 8), 0));
      MemsetParams.push_back(IntegerType::get(rCtxt, 32));
      MemsetParams.push_back(IntegerType::get(rCtxt, 32));
      FunctionType *pMemsetFuncType = FunctionType::get(Type::getInt32Ty(rCtxt), MemsetParams, false);
      pMemsetFunc = Function::Create(pMemsetFuncType, GlobalValue::ExternalLinkage, "memset", pModule);
    }

    Function *pFunc = Function::Create(pFuncType, Function::ExternalLinkage, "Exec", pModule);
    BasicBlock *pBscBlk = BasicBlock::Create(rCtxt, "entry", pFunc);
    Builder.SetInsertPoint(pBscBlk);
    auto pCastStructPtr = Builder.CreateBitCast(pFunc->arg_begin(), PointerType::get(IntegerType::get(rCtxt, 8), 0), "ptr_cast");
    std::vector<Value *> MemsetParamsValue;
    MemsetParamsValue.push_back(pCastStructPtr);
    MemsetParamsValue.push_back(ConstantInt::get(rCtxt, APInt(32, 0x22)));
    MemsetParamsValue.push_back(ConstantInt::get(rCtxt, APInt(32, 4 * 10))); /* WAT? we can't retrieve the size of a struct dynamically ? */
    Builder.CreateCall(pMemsetFunc, MemsetParamsValue, "call_memset");
    Builder.CreateGEP(pFunc->arg_begin(), ConstantInt::get(Type::getInt32Ty(rCtxt), 1));

    Builder.CreateRetVoid();

    auto pJitFunction = (void (*)(CpuContext *))pExecutionEngine->getPointerToFunction(pFunc);
    pFunc->dump();

    CpuContext CpuCtxt;
    memset(&CpuCtxt, 0x11, sizeof(CpuCtxt));
    std::cout << std::hex;
    std::cout << CpuCtxt.Reg[9] << std::endl;
    pJitFunction(&CpuCtxt);
    std::cout << CpuCtxt.Reg[9] << std::endl;
  }
  catch (std::exception const& rExcpt)
  {
    std::cerr << rExcpt.what() << std::endl;
    return 1;
  }

  return 0;
}