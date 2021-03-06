#include <iostream>
#include <string>

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

int main(void)
{
  try
  {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    IRBuilder<> Builder(getGlobalContext());
    std::string ErrMsg;

    Module          *pModule          = new Module("llvm-test module", getGlobalContext());
    ExecutionEngine *pExecutionEngine = EngineBuilder(pModule).setUseMCJIT(true).setErrorStr(&ErrMsg).create();
    if (pExecutionEngine == nullptr) throw ErrMsg;

    std::vector<Type *> FunctionParameters;
    FunctionParameters.push_back(Type::getInt32Ty(getGlobalContext()));
    FunctionParameters.push_back(Type::getInt32Ty(getGlobalContext()));

    FunctionType *pFunctionType = FunctionType::get(Type::getInt32Ty(getGlobalContext()), FunctionParameters, false);
    Function *pFunction = Function::Create(
      pFunctionType,
      Function::ExternalLinkage,
      "add",
      pModule
    );

    BasicBlock *pBasicBlock = BasicBlock ::Create(getGlobalContext(), "entry", pFunction);

    Builder.SetInsertPoint(pBasicBlock);

    std::vector<Value *> ArgumentValues;
    for (auto Argument = pFunction->arg_begin(); Argument != pFunction->arg_end(); ++Argument)
      ArgumentValues.push_back(Argument);

    if (ArgumentValues.size() != 2)
      throw "Invalid number of parameters";

    auto pValue = Builder.CreateAdd(ArgumentValues[0], ArgumentValues[1], "op_add");

    Builder.CreateRet(pValue);

    auto pJitFunction = (int (*)(int, int))pExecutionEngine->getPointerToFunction(pFunction);

    std::cout << "Value: " << pJitFunction(10, 40) << std::endl;

    pFunction->dump();
  }
  catch (std::string const& rMsg)
  {
    std::cerr << rMsg << std::endl;
  }

  return 0;
}
