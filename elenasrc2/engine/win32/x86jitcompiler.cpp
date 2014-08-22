//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86jitcompiler.h"
#include "bytecode.h"

using namespace _ELENA_;

// --- ELENA Object constants ---
const int gcPageSize       = 0x0010;           // a heap page size constant

// --- ELENA built-in routines
#define CORE_EXCEPTION_TABLE 0x0001
#define CORE_GC_TABLE        0x0002
#define CORE_GC_SIZE         0x0003
#define CORE_STAT_COUNT      0x0004
#define CORE_STATICROOT      0x0005
#define CORE_VM_TABLE        0x0006
#define CORE_TLS_INDEX       0x0007
#define CORE_THREADTABLE     0x0008

#define GC_ALLOC             0x10001
#define HOOK                 0x10010
#define LOADCLASSNAME        0x10011
#define INIT_RND             0x10012
#define EVALSCRIPT           0x10013
#define LOADSYMBOL           0x10014

// preloaded gc routines
const int coreVariableNumber = 2;
const int coreVariables[coreVariableNumber] =
{
   CORE_EXCEPTION_TABLE, CORE_GC_TABLE
};

// preloaded gc routines
const int coreFunctionNumber = 6;
const int coreFunctions[coreFunctionNumber] =
{
   GC_ALLOC, HOOK, LOADCLASSNAME, INIT_RND, EVALSCRIPT, LOADSYMBOL
};

// preloaded gc commands
const int gcCommandNumber = 115;
const int gcCommands[gcCommandNumber] =
{   
   bcALoadSI, bcACallVI, bcOpen, bcBCopyA, bcMessage,
   bcALoadFI, bcASaveSI, bcASaveFI, bcClose, bcMIndex,
   bcNewN, bcNew, bcWEval, bcSwapSI, bcASwapSI,
   bcALoadBI, bcPushAI, bcCallExtR, bcPushF, bcBSRedirect,
   bcHook, bcThrow, bcUnhook, bcWName, bcClass,
   bcDLoadSI, bcDSaveSI, bcDLoadFI, bcDSaveFI, 
   bcEQuit, bcAJumpVI, bcASaveBI, bcXCallRM, 
   bcGet, bcSet, bcXSet, bcECall,
   bcRestore, bcCount, bcIfHeap, bcFlag,
   bcBLoadFI, bcReserve, bcAXSaveBI, bcBLoadSI,
   bcType, bcNEqual, bcNLess, bcNCopy, bcNAdd,
   bcNSub, bcNMul, bcNDiv, bcWEqual, bcWLess,
   bcWLen, bcNSave, bcNLoad, bcWCreate, bcCopy,
   bcWSave, bcBCreate, bcSave, bcLen, bcLoadW,
   bcLoad, bcWLoad, bcWInsert, bcESwap, bcBSwap,
   bcWToN, bcNToW, bcNAnd, bcNOr, bcNXor,
   bcLCopy, bcLCopyN, bcLEqual, bcLLess, bcLAdd,
   bcLSub, bcLMul, bcLDiv, bcLAnd, bcLOr,
   bcLXor, bcNShift, bcNNot, bcLShift, bcLToW,
   bcLNot, bcRCopy, bcRCopyL, bcRCopyN, bcREqual,
   bcRLess, bcRAdd, bcRSub, bcRMul, bcRDiv,
   bcRToW, bcInsert, bcCreate, bcWSeek, bcWAdd,
   bcWSubCopy, bcNInsert, bcSelectR, bcWToL, bcWToR,
   bcSubCopy, bcNSubCopy, bcXSeek, bcNext, bcClone,
};

// command table
void (*commands[0x100])(int opcode, x86JITScope& scope) =
{
   &compileNop, &compileBreakpoint, &compilePushB, &compilePop, &compileNop, &compilePushE, &compileDCopyVerb, &loadOneByteOp,
   &compileDCopyCount, &compileOr, &compilePushA, &compilePopA, &compileACopyB, &compilePopE, &loadOneByteOp, &compileDCopySubj,

   &compileNop, &loadOneByteLOp, &loadOneByteLOp, &compileIndexDec, &compilePopB, &loadOneByteLOp, &compileDSub, &compileQuit,
   &loadOneByteOp, &loadOneByteOp, &compileIndexInc, &loadOneByteLOp, &compileALoad, &loadOneByteOp, &compileDAdd, &loadOneByteOp,

   &compileECopyD, &compileDCopyE, &compilePushD, &compilePopD, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteLOp, &loadOneByteLOp,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteLOp, &compileNop,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteOp,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop,
   &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &loadOneByteOp,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
   &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileDCopy, &compileECopy, &loadIndexOp, &compileALoadR, &loadFPOp, &loadIndexOp, &compileIfHeap, &loadROp,
   &compileOpen, &compileQuitN, &compileBCopyR, &compileBCopyF, &compileACopyF, &compileACopyS, &compileACopyR, &compileMCopy,

   &compileJump, &loadVMTIndexOp, &loadVMTIndexOp, &compileCallR, &loadCode, &loadFunction, &compileHook, &compileNop,
   &loadVMTMIndexOp, &compileLessE, &compileNotLessE, &compileIfB, &compileElseB, &compileIfE, &compileElseE, &compileNext,

   &compilePush, &compileNop, &compilePush, &compilePushBI, &loadIndexOp, &compileNop, &compilePushFI, &loadFPOp,
   &loadIndexOp, &loadFPOp, &compilePushS, &loadIndexOp, &compileNop, &compilePushF, &compileNop, &loadIndexOp,

   &loadIndexOp, &loadIndexOp, &loadIndexOp, &loadIndexOp, &loadFPOp, &compileNop, &compileNop, &compileNop,
   &loadFPOp, &loadIndexOp, &compileNop, &compileNop, &compileASaveR, &compileALoadAI, &loadIndexOp, &loadIndexOp,

   &compilePopN, &compileNop, &compileSCopyF, &compileSetVerb, &compileSetSubj, &compileDAndN, &compileDAddN, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileCreate, &compileCreateN, &compileNop, &compileNop, &compileNop, &compileNop, &compileSelectR, &compileLessN,
   &compileIfM, &compileElseM, &compileIfR, &compileElseR, &compileIfN, &compileElseN, &compileInvokeVMT, &compileNop
};

//const int gcExtensionNumber = EXTENSION_COUNT;

// --- x86JITCompiler commands ---

inline void compileJump(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jmp   lbEnding
   if (!forwardJump) {
      scope.lh.writeJmpBack(label);
   }
   else {
      // if it is forward jump, try to predict if it is short
      if (shortJump) {
         scope.lh.writeShortJmpForward(label);
      }
      else scope.lh.writeJmpForward(label);
   }
}

inline void compileJumpX(x86JITScope& scope, int label, bool forwardJump, bool shortJump, x86Helper::x86JumpType prefix)
{
   if (!forwardJump) {
      scope.lh.writeJxxBack(prefix, label);
   }
   else {
      // if it is forward jump, try to predict if it is short
      if (shortJump) {
         scope.lh.writeShortJxxForward(label, prefix);
      }
      else scope.lh.writeJxxForward(label, prefix);
   }
}

inline void compileJumpIf(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jnz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JNZ);
}

inline void compileJumpIfNot(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JZ);
}

inline void compileJumpAbove(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // ja   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JA);
}

inline void compileJumpBelow(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jb   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JB);
}

//inline void compileJumpGreater(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jg   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JG);
//}

inline void compileJumpLess(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JL);
}

inline void compileJumpLessOrEqual(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jle   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JLE);
}

inline void compileJumpGreaterOrEqual(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jge   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JGE);
}

void _ELENA_::loadCoreOp(x86JITScope& scope, char* code)
{
   MemoryWriter* writer = scope.code;

   if (code==NULL)
      throw InternalError("Cannot load core command");

   size_t position = writer->Position();
   size_t length = *(size_t*)(code - 4);

   writer->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + offset);

      if ((key & mskTypeMask) == mskPreloaded) {
         scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      }
      else {
         //if ((key & mskAnyRef) == mskLinkerConstant) {
         //   scope.code->writeDWord(scope.helper->getLinkerConstant(key & ~mskAnyRef));
         //}
         /*else */scope.helper->writeReference(*writer, key, *(int*)(code + offset), scope.module);
      }

      relocation += 2;
      count--;
   }
   writer->seekEOF();
}

inline void _ELENA_::writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   // references should be already preloaded
   if ((reference & mskAnyRef) == mskPreloadRelCodeRef) {
      scope.helper->writeReference(*scope.code,
         scope.compiler->_preloaded.get(reference & ~mskAnyRef), true, *(int*)(code + offset));

      scope.lh.addFixableJump(offset + position, (*scope.code->Memory())[offset + position]);
   }
   else scope.helper->writeReference(*scope.code,
      scope.compiler->_preloaded.get(reference & ~mskAnyRef), false, *(int*)(code + offset));
}

void _ELENA_::loadOneByteOp(int opcode, x86JITScope& scope)
{
   MemoryWriter* writer = scope.code;

   char* code = (char*)scope.compiler->_inlines[opcode];
   size_t position = writer->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   writer->write(code, *(int*)(code - 4));

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + relocation[1]);

      if ((key & mskTypeMask) == mskPreloaded) {
         scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      }
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(scope.argument);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadOneByteLOp(int opcode, x86JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];

   // simply copy correspondent inline code
   scope.code->write(code, *(int*)(code - 4));
}

void _ELENA_::loadROp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.writeReference(*scope.code, scope.argument, 0);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadIndexOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(scope.argument << 2);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadVMTIndexOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord((scope.argument << 3) + 4);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadVMTMIndexOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFPOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(-(scope.argument << 2));
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFunction(int opcode, x86JITScope& scope)
{
   MemoryWriter* writer = scope.code;

   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + relocation[1]);

      // !! temporal, more optimal way should be used
      if (relocation[0]==-1) {
         scope.writeReference(*writer, scope.argument, *(int*)(code + offset));
      }
      //else if ((key & mskTypeMask) == mskPreloaded) {
      //   scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      //}
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadCode(int opcode, x86JITScope& scope)
{
   // if it is a symbol reference
   if ((scope.argument & mskAnyRef) == mskSymbolRef) {
      // if embedded symbol mode is on
      if (scope.embeddedSymbols) {
         SectionInfo  info = scope.getSection(scope.argument);
         MemoryReader reader(info.section);

         scope.compiler->embedSymbol(*scope.helper, reader, *scope.code, info.module);
      }
      // otherwise treat like calling a symbol
      else compileCallR(bcCallR, scope);
   }
   else {
      // otherwise a primitive code
      SectionInfo   info = scope.getSection(scope.argument);
      MemoryWriter* writer = scope.code;

      // override module
      scope.module = info.module;

      char*  code = (char*)info.section->get(0);
      size_t position = scope.code->Position();
      size_t length = *(size_t*)(code - 4);

      // simply copy correspondent inline code
      writer->write(code, length);

      // resolve section references
      int count = *(int*)(code + length);
      int* relocation = (int*)(code + length + 4);
      int key, offset;
      while (count > 0) {
         key = relocation[0];
         offset = relocation[1];

         // locate relocation position
         writer->seek(position + relocation[1]);

         scope.writeReference(*writer, key, *(int*)(code + offset));

         relocation += 2;
         count--;
      }
      // clear module overriding
      scope.module = NULL;
      scope.code->seekEOF();
   }
}

void _ELENA_::compileNop(int opcode, x86JITScope& scope)
{
   // nop command is used to indicate possible label
   // fix the label if it exists
   if (scope.lh.checkLabel(scope.tape->Position() - 1)) {
      scope.lh.fixLabel(scope.tape->Position() - 1);
   }
   // or add the label
   else scope.lh.setLabel(scope.tape->Position() - 1);
}

void _ELENA_::compileBreakpoint(int opcode, x86JITScope& scope)
{
   if (scope.debugMode)
      scope.helper->addBreakpoint(scope.code->Position());
}

void _ELENA_::compilePush(int opcode, x86JITScope& scope)
{
   // push constant | reference
   scope.code->writeByte(0x68);
   if (opcode == bcPushR) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(scope.argument);
}

void _ELENA_::compilePopE(int opcode, x86JITScope& scope)
{
   // pop ecx
   scope.code->writeByte(0x59);
}

void _ELENA_::compilePopD(int opcode, x86JITScope& scope)
{
   // pop esi
   scope.code->writeByte(0x5E);
}

void _ELENA_::compileSCopyF(int opcode, x86JITScope& scope)
{
   // lea esp, [ebp - level * 4]

   x86Helper::leaRM32disp(scope.code, x86Helper::otESP, x86Helper::otEBP, -(scope.argument << 2));
}

void _ELENA_::compileALoadAI(int opcode, x86JITScope& scope)
{
   // mov eax, [eax + __arg * 4]

   scope.code->writeWord(0x808B);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compilePushS(int opcode, x86JITScope& scope)
{
   // push [esp+offset]
   scope.code->writeWord(0xB4FF);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileJump(int opcode, x86JITScope& scope)
{
   ::compileJump(scope, scope.tape->Position() + scope.argument, (scope.argument > 0), (__abs(scope.argument) < 0x10));
}

void _ELENA_::compileHook(int opcode, x86JITScope& scope)
{
   scope.lh.writeLoadForward(scope.tape->Position() + scope.argument);
   loadOneByteOp(opcode, scope);
}

void _ELENA_::compileOpen(int opcode, x86JITScope& scope)
{
   loadOneByteLOp(opcode, scope);

   //scope.prevFSPOffs += (scope.argument << 2);
}

void _ELENA_::compileQuit(int opcode, x86JITScope& scope)
{
   scope.code->writeByte(0xC3);
}

void _ELENA_::compileQuitN(int opcode, x86JITScope& scope)
{
   scope.code->writeByte(0xC2);
   scope.code->writeWord(scope.argument << 2);
}

void _ELENA_::compileNext(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test upper boundary
   loadOneByteLOp(opcode, scope);

  // try to use short jump if offset small (< 0x10?)
   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfE(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp ecx, esi
   scope.code->writeWord(0xCE3B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileElseE(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp ecx, esi
   scope.code->writeWord(0xCE3B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileLessE(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp esi, ecx
   scope.code->writeWord(0xF13B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileNotLessE(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp esi, ecx
   scope.code->writeWord(0xF13B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpGreaterOrEqual(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfB(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp eax, edi
   scope.code->writeWord(0xC73B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileElseB(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp eax, edi
   scope.code->writeWord(0xC73B);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfM(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();
   int message = scope.resolveMessage(scope.argument);

   // cmp ecx, message
   scope.code->writeWord(0xF981);
   scope.code->writeDWord(message);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileElseM(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();
   int message = scope.resolveMessage(scope.argument);

   // cmp ecx, message
   scope.code->writeWord(0xF981);
   scope.code->writeDWord(message);

  // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfHeap(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

   // load bottom boundary
   loadOneByteOp(opcode, scope);

   // cmp eax, [ebx]
   // ja short label
   // cmp eax, esp
   // jb short label

   scope.code->writeWord(0x033B);
  // try to use short jump if offset small (< 0x10?)
   compileJumpAbove(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));

   scope.code->writeWord(0xC43B);
   compileJumpBelow(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileCreate(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   scope.argument <<= 2;

   // mov  ebx, #gc_page + (length - 1)
   scope.code->writeByte(0xBB);
   scope.code->writeDWord(align(scope.argument + scope.objectSize, gcPageSize));
   
   loadNOp(opcode, scope);

   // set vmt reference
   // mov [eax-4], vmt
   scope.code->writeWord(0x40C7);
   scope.code->writeByte(0xFC);
   scope.writeReference(*scope.code, vmtRef, 0);
}

void _ELENA_::compileCreateN(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   int size = align(scope.argument + scope.objectSize, gcPageSize);

   scope.argument = -scope.argument;  // mark object as a binary structure

   // mov  ebx, #gc_page + (size - 1)
   scope.code->writeByte(0xBB);
   scope.code->writeDWord(size);

   loadNOp(opcode, scope);

   // set vmt reference
   // mov [eax-4], vmt
   scope.code->writeWord(0x40C7);
   scope.code->writeByte(0xFC);
   scope.writeReference(*scope.code, vmtRef, 0);
}

void _ELENA_::compileSelectR(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t r1 = scope.argument;
   scope.argument = scope.tape->getDWord();

   int size = align(scope.argument + scope.objectSize, gcPageSize);

   // mov  ebx, r1
   scope.code->writeByte(0xBB);
   scope.writeReference(*scope.code, r1, 0);

   loadROp(opcode, scope);
}

void _ELENA_::compileACopyR(int opcode, x86JITScope& scope)
{
   // mov eax, r
   scope.code->writeByte(0xB8);
   if (scope.argument != 0) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(0);
}

void _ELENA_::compileBCopyR(int opcode, x86JITScope& scope)
{
   // mov edi, r
   scope.code->writeByte(0xBF);
   scope.writeReference(*scope.code, scope.argument, 0);
}

void _ELENA_::compileDCopy(int opcode, x86JITScope& scope)
{
   // mov esi, i
   scope.code->writeByte(0xBE);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileECopy(int opcode, x86JITScope& scope)
{
   // mov ecx, i
   scope.code->writeByte(0xB9);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileDAdd(int opcode, x86JITScope& scope)
{
   // add esi, ecx
   scope.code->writeWord(0xF103);
}

void _ELENA_::compileDAndN(int opcode, x86JITScope& scope)
{
   // and esi, mask
   scope.code->writeWord(0xE681);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileDAddN(int opcode, x86JITScope& scope)
{
   // add esi, n
   scope.code->writeWord(0xC681);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileDSub(int opcode, x86JITScope& scope)
{
   // sub esi, ecx
   scope.code->writeWord(0xF12B);
}

void _ELENA_::compileDCopyVerb(int opcode, x86JITScope& scope)
{
   // mov esi, ecx
   // and esi, VERB_MASK
   scope.code->writeWord(0xF18B);
   scope.code->writeWord(0xE681);
   scope.code->writeDWord(VERB_MASK | MESSAGE_MASK);   
}

void _ELENA_::compileDCopyCount(int opcode, x86JITScope& scope)
{
   // mov esi, ecx
   // and esi, VERB_MASK
   scope.code->writeWord(0xF18B);
   scope.code->writeWord(0xE681);
   scope.code->writeDWord(PARAM_MASK);   
}

void _ELENA_::compileDCopySubj(int opcode, x86JITScope& scope)
{
   // mov esi, ecx
   // and esi, VERB_MASK
   scope.code->writeWord(0xF18B);
   scope.code->writeWord(0xE681);
   scope.code->writeDWord(PARAM_MASK | SIGN_MASK | MESSAGE_MASK);   
}

void _ELENA_::compileALoad(int opcode, x86JITScope& scope)
{
   // mov eax, [eax + esi*4]
   scope.code->writeWord(0x048B);
   scope.code->writeByte(0xB0);
}

void _ELENA_::compileALoadR(int opcode, x86JITScope& scope)
{
   // mov eax, [r]
   scope.code->writeByte(0xA1);
   scope.writeReference(*scope.code, scope.argument, 0);
}

void _ELENA_::compilePushA(int opcode, x86JITScope& scope)
{
   // push eax
   scope.code->writeByte(0x50);
}

void _ELENA_::compilePushFI(int opcode, x86JITScope& scope)
{
   scope.code->writeWord(0xB5FF);
   // push [ebp-level*4]
   scope.code->writeDWord(-(scope.argument << 2));
}

void _ELENA_:: compilePushF(int opcode, x86JITScope& scope)
{
   scope.argument = -(scope.argument << 2);   

   loadNOp(bcPushF, scope);
}

void _ELENA_::compilePushB(int opcode, x86JITScope& scope)
{
   // push edi
   scope.code->writeByte(0x57);
}

void _ELENA_::compilePushE(int opcode, x86JITScope& scope)
{
   // push ecx
   scope.code->writeByte(0x51);
}

void _ELENA_::compilePushD(int opcode, x86JITScope& scope)
{
   // push esi
   scope.code->writeByte(0x56);
}

void _ELENA_::compilePushBI(int opcode, x86JITScope& scope)
{
   // push [edi + offset * 4]
   scope.code->writeWord(0xB7FF);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileCallR(int opcode, x86JITScope& scope)
{
   // call symbol
   scope.code->writeByte(0xE8);
   scope.writeReference(*scope.code, scope.argument | mskRelCodeRef, 0);
}

void _ELENA_::compilePop(int opcode, x86JITScope& scope)
{
   // pop edx
   scope.code->writeByte(0x5A);
}

void _ELENA_::compilePopA(int opcode, x86JITScope& scope)
{
   // pop eax
   scope.code->writeByte(0x58);
}

void _ELENA_::compileMCopy(int opcode, x86JITScope& scope)
{
   // mov ecx, message
   scope.code->writeByte(0xB9);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compilePopN(int opcode, x86JITScope& scope)
{
   // add esp, arg
   scope.code->writeWord(0xC481);
   scope.code->writeDWord(scope.argument << 2);

   //// lea esp, [esp + level * 4]
   //x86Helper::leaRM32disp(
   //   scope.code, x86Helper::otESP, x86Helper::otESP, scope.argument << 2);
}

void _ELENA_::compileACopyB(int opcode, x86JITScope& scope)
{
   // mov eax, edi
   scope.code->writeWord(0xF889);
}

void _ELENA_::compileASaveR(int opcode, x86JITScope& scope)
{
   // mov [ref], eax

   scope.code->writeWord(0x0589);
   scope.writeReference(*scope.code, scope.argument, 0);
}

void _ELENA_::compileInvokeVMT(int opcode, x86JITScope& scope)
{
   int message = scope.resolveMessage(scope.tape->getDWord());

   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         // resolve message offset
         scope.writeReference(*scope.code, scope.argument | mskVMTEntryOffset, message);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::compileACopyS(int opcode, x86JITScope& scope)
{
   // lea eax, [esp + index]
   x86Helper::leaRM32disp(                     
      scope.code, x86Helper::otEAX, x86Helper::otESP, scope.argument << 2);
}

void _ELENA_::compileIfR(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, r
   // jz lab

   scope.code->writeByte(0x3D);
   scope.writeReference(*scope.code, scope.argument, 0);
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileElseR(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, r
   // jz lab

   scope.code->writeByte(0x3D);
   // HOTFIX : support zero references
   if (scope.argument != 0) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(0);

   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileIfN(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp esi, n
   // jz lab

   scope.code->writeWord(0xFE81);
   scope.code->writeDWord(scope.argument);
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileElseN(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp esi, n
   // jnz lab

   scope.code->writeWord(0xFE81);
   scope.code->writeDWord(scope.argument);
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileLessN(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp esi, n
   // jz lab

   scope.code->writeWord(0xFE81);
   scope.code->writeDWord(scope.argument);
   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileIndexInc(int opcode, x86JITScope& scope)
{
   // add esi, 1
   scope.code->writeWord(0xC683);
   scope.code->writeByte(1);
}

void _ELENA_::compileIndexDec(int opcode, x86JITScope& scope)
{
   // sub esi, 1
   scope.code->writeWord(0xEE83);
   scope.code->writeByte(1);
}

void _ELENA_::compileSetVerb(int opcode, x86JITScope& scope)
{
   // and ecx, VERB_MASK
   // or  ecx, m
   scope.code->writeWord(0xE181);
   scope.code->writeDWord(~VERB_MASK);
   scope.code->writeWord(0xC981);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileSetSubj(int opcode, x86JITScope& scope)
{
   // and ecx, SUBJ_MASK
   // or  ecx, m
   scope.code->writeWord(0xE181);
   scope.code->writeDWord(~SIGN_MASK);
   scope.code->writeWord(0xC981);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileOr(int opcode, x86JITScope& scope)
{
   // or esi, ecx
   scope.code->writeWord(0xF10B);
}

void _ELENA_::compilePopB(int opcode, x86JITScope& scope)
{
   // pop edi
   scope.code->writeByte(0x5F);
}

void _ELENA_::compileECopyD(int opcode, x86JITScope& scope)
{
   // mov ecx, esi
   scope.code->writeWord(0xCE8B);
}

void _ELENA_::compileDCopyE(int opcode, x86JITScope& scope)
{
   // mov esi, ecx
   scope.code->writeWord(0xF18B);
}

void _ELENA_::compileBCopyF(int opcode, x86JITScope& scope)
{
   // lea edi, [ebp+nn]
   scope.code->writeWord(0xBD8D);
   scope.code->writeDWord(-(scope.argument << 2));
}

void _ELENA_::compileACopyF(int opcode, x86JITScope& scope)
{
   // lea eax, [ebp+nn]
   scope.code->writeWord(0x858D);
   scope.code->writeDWord(-(scope.argument << 2));
}

// --- x86JITScope ---

x86JITScope :: x86JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler, bool embeddedSymbols)
   : lh(code)
{
   this->tape = tape;
   this->code = code;
   this->helper = helper;
   this->compiler = compiler;
   this->debugMode = compiler->isDebugMode();
   this->objectSize = helper ? helper->getLinkerConstant(lnObjectSize) : 0;
   this->embeddedSymbols = embeddedSymbols;
   this->module = NULL;

//   this->prevFSPOffs = 0;
}

// --- x86JITCompiler ---

x86JITCompiler :: x86JITCompiler(bool debugMode, bool embeddedSymbolMode)
{
   _embeddedSymbolMode = embeddedSymbolMode;
   _debugMode = debugMode;
}

void x86JITCompiler :: alignCode(MemoryWriter* writer, int alignment, bool code)
{
   writer->align(VA_ALIGNMENT, code ? 0x90 : 0x00);
}

void x86JITCompiler :: writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   if (!_preloaded.exist(reference& ~mskAnyRef)) {
      MemoryWriter writer(scope.code->Memory());

      _preloaded.add(reference & ~mskAnyRef, scope.helper->getVAddress(writer, mskCodeRef));

      // due to optimization section must be ROModule::ROSection instance
      SectionInfo info = scope.helper->getPredefinedSection(ConstantIdentifier(CORE_MODULE), reference & ~mskAnyRef);
      // separate scoep should be used to prevent overlapping
      x86JITScope newScope(NULL, &writer, scope.helper, this, _embeddedSymbolMode);
      newScope.module = info.module;

      loadCoreOp(newScope, info.section ? (char*)info.section->get(0) : NULL);
   }
   _ELENA_::writePreloadedReference(scope, reference, position, offset, code);
}

void x86JITCompiler :: prepareCoreData(_ReferenceHelper& helper, _Memory* data, _Memory* rdata, _Memory* sdata)
{
   ConstantIdentifier corePackage(CORE_MODULE);

   MemoryWriter writer(data);
   MemoryWriter rdataWriter(rdata);
   MemoryWriter sdataWriter(sdata);

   x86JITScope scope(NULL, &writer, &helper, this, _embeddedSymbolMode);
   for (int i = 0 ; i < coreVariableNumber ; i++) {
      if (!_preloaded.exist(coreVariables[i])) {
         _preloaded.add(coreVariables[i], helper.getVAddress(writer, mskDataRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getPredefinedSection(corePackage, coreVariables[i]);
         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // GC SIZE Table
   _preloaded.add(CORE_GC_SIZE, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCMGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCYGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnThreadCount));

   // load GC static root
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // STAT COUNT
   _preloaded.add(CORE_STAT_COUNT, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeDWord(0);
}

void x86JITCompiler :: prepareVMData(_ReferenceHelper& helper, _Memory* data)
{
   MemoryWriter writer(data);

   // VM TABLE
   _preloaded.add(CORE_VM_TABLE, helper.getVAddress(writer, mskDataRef));

   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_Instance));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_LoadSymbol));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_LoadName));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_Interprete));
//   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_GetLastError));
}

void x86JITCompiler :: prepareCommandSet(_ReferenceHelper& helper, _Memory* code)
{
   ConstantIdentifier corePackage(CORE_MODULE);
   ConstantIdentifier commandPackage(COMMANDSET_MODULE);

   MemoryWriter writer(code);
   x86JITScope scope(NULL, &writer, &helper, this, _embeddedSymbolMode);

   for (int i = 0 ; i < coreFunctionNumber ; i++) {
      if (!_preloaded.exist(coreFunctions[i])) {
         _preloaded.add(coreFunctions[i], helper.getVAddress(writer, mskCodeRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getPredefinedSection(corePackage, coreFunctions[i]);
         scope.module = info.module;

         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // preload vm commands
   scope.helper = &helper;
   for (int i = 0 ; i < gcCommandNumber ; i++) {
      SectionInfo info = helper.getPredefinedSection(commandPackage, gcCommands[i]);

      // due to optimization section must be ROModule::ROSection instance
      _inlines[gcCommands[i]] = (char*)info.section->get(0);
   }
}

void x86JITCompiler :: setStaticRootCounter(_JITLoader* loader, int counter, bool virtualMode)
{
   if (virtualMode) {
      _Memory* data = loader->getTargetSection(mskRDataRef);

      size_t offset = ((size_t)_preloaded.get(CORE_STAT_COUNT) & ~mskAnyRef);
      (*data)[offset] = (counter << 2);
   }
   else {
 	   size_t offset = (size_t)_preloaded.get(CORE_STAT_COUNT);
 	   *(int*)offset = (counter << 2);
   }
}

void* x86JITCompiler :: getPreloadedReference(ref_t reference)
{
   return (void*)_preloaded.get(reference);
}

void x86JITCompiler :: compileThreadTable(_JITLoader* loader, int maxThreadNumber)
{
   // get target image & resolve virtual address
   MemoryWriter dataWriter(loader->getTargetSection(mskDataRef));

   // size place holder
   dataWriter.writeDWord(0);

   // reserve space for the thread table
   int position = dataWriter.Position();
   allocateArray(dataWriter, maxThreadNumber);

   // map thread table
   ConstantIdentifier reference(GC_THREADTABLE);

   loader->mapReference(reference, (void*)(position | mskDataRef), mskDataRef);
   _preloaded.add(CORE_THREADTABLE, (void*)(position | mskDataRef));
}

void x86JITCompiler :: compileTLS(_JITLoader* loader)
{
   MemoryWriter dataWriter(loader->getTargetSection(mskDataRef));

   // reserve space for TLS index
   int position = dataWriter.Position();
   allocateVariable(dataWriter);

   // map TLS index
   ConstantIdentifier tlsKey(TLS_KEY);

   loader->mapReference(tlsKey, (void*)(position | mskDataRef), mskDataRef);
   _preloaded.add(CORE_TLS_INDEX, (void*)(position | mskDataRef));

   // allocate tls section
   MemoryWriter tlsWriter(loader->getTargetSection(mskTLSRef));
   tlsWriter.writeDWord(0);   // stack frame pointer
   tlsWriter.writeDWord(0);   // stack bottom pointer
   tlsWriter.writeDWord(0);   // catch address
   tlsWriter.writeDWord(0);   // catch stack level
   tlsWriter.writeDWord(0);   // catch stack frame
   tlsWriter.writeDWord(0);   // syncronization event
   tlsWriter.writeDWord(0);   // thread flags

   // map IMAGE_TLS_DIRECTORY
   MemoryWriter rdataWriter(loader->getTargetSection(mskRDataRef));
   loader->mapReference(tlsKey, (void*)(rdataWriter.Position() | mskRDataRef), mskRDataRef);

   // create IMAGE_TLS_DIRECTORY
   rdataWriter.writeRef(mskTLSRef, 0);          // StartAddressOfRawData
   rdataWriter.writeRef(mskTLSRef, 12);         // EndAddressOfRawData
   rdataWriter.writeRef(mskDataRef, position);  // AddressOfIndex
   rdataWriter.writeDWord(0);                   // AddressOfCallBacks
   rdataWriter.writeDWord(0);                   // SizeOfZeroFill
   rdataWriter.writeDWord(0);                   // Characteristics
}

inline void compileTape(MemoryReader& tapeReader, int endPos, x86JITScope& scope)
{
   unsigned char code = 0;
   while(tapeReader.Position() < endPos) {
      // read bytecode + arguments
      code = tapeReader.getByte();
      // preload an argument if a command requires it
      if (code > MAX_SINGLE_ECODE) {
         scope.argument = tapeReader.getDWord();
      }
      commands[code](code, scope);
   }
}

void x86JITCompiler :: embedSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter, _Module* module)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);
   scope.debugMode = false;   // embedded symbol does not provide a debug info
   scope.module = module;

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);
}

void x86JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

//   // ; copy the parameter from the previous frame to simulate embedded symbol
//   // push [esp+4]
//   codeWriter.writeDWord(0x042474FF);

   compileTape(tapeReader, endPos, scope);

   // ; copy the parameter to the accumulator to simulate embedded symbol
   // ; exit the procedure
   // ret
   codeWriter.writeByte(0xC3);  

   alignCode(&codeWriter, 0x04, true);
}

void x86JITCompiler :: compileProcedure(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);
//   scope.prevFSPOffs = 4;

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   alignCode(&codeWriter, 0x04, true);
}

void x86JITCompiler :: loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section)
{
   size_t position = writer.Position();

   writer.write(section->get(0), section->Length());

   // resolve section references
   _ELENA_::RelocationMap::Iterator it(section->getReferences());
   while (!it.Eof()) {
      int arg = *it;
      writer.seek(arg + position);

      const wchar16_t* reference = binary->resolveReference(it.key() & ~mskAnyRef);

      helper.writeReference(writer, reference, it.key() & mskAnyRef);

      it++;
   }
   writer.seekEOF();
}
