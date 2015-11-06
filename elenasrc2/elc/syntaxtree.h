//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Syntax Tree classes
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxTreeH
#define syntaxTreeH 1

namespace _ELENA_
{

// --- SyntaxType ---

enum LexicalType
{
   lxObjectMask      = 0x100,
   lxExpressionMask  = 0x200,
   //lxCallMask        = 0x200,

   lxEnding          = -1,

   lxNone            = 0x000,

   lxRoot            = 0x001,

   //lxCodeBlock       = 0x001,

   lxExpression      = 0x301,
   lxBoxing          = 0x302,
   lxTypecasting     = 0x303,
   lxCalling         = 0x304,
   lxDirectCalling   = 0x305,
   lxSDirctCalling   = 0x306,
   lxReturning       = 0x307,
   lxStdExternalCall = 0x308,
   lxExternalCall    = 0x309,
   lxIntExtArgument  = 0x30A,
   lxExtArgument     = 0x30B,
   lxAssigning       = 0x30C,

   lxSymbol          = 0x101,
   lxConstantSymbol  = 0x102,
   lxField           = 0x103,
   lxFieldAddress    = 0x104,
   lxLocalAddress    = 0x105,
   lxLocal           = 0x106,
   lxBlockLocal      = 0x107,
   lxConstantString  = 0x108,
   lxConstantChar    = 0x109,
   lxConstantInt     = 0x10A,
   lxConstantLong    = 0x10B,
   lxConstantReal    = 0x10C,
   lxConstantClass   = 0x10D,
   lxNil             = 0x10E,
   lxCurrent         = 0x10F,
   lxResult          = 0x110,
   lxResultField     = 0x111,

   //lxExternalCall    = 0x206,
   //lxStdExternalCall = 0x207,

   //lxAlternative     = 0x401,
   //lxCatch           = 0x402,
   //lxAssigning       = 0x403,    // if argument == 0 -> assign field, otherwise copy the memory block with specified size
   //lxCondBoxing      = 0x406,    // the same like boxing except checking if the reference is stack allocated
   lxVariable        = 0x407,
   lxIntVariable     = 0x408,
   lxLongVariable    = 0x409,
   lxReal64Variable  = 0x40A,
   lxBytesVariable   = 0x40B,
   lxShortsVariable  = 0x40C,
   lxIntsVariable    = 0x40D,

   lxTarget          = 0x801,
   lxType            = 0x802,
   lxStacksafe       = 0x803,
   lxTempLocal       = 0x804,

   lxBreakpoint      = 0x1001,
   lxCol             = 0x1002,
   lxRow             = 0x1003,
   lxLength          = 0x1004,
   lxTerminal        = 0x1005,
   lxLevel           = 0x1006,
};

// --- SyntaxWriter ---

class SyntaxWriter
{
   MemoryWriter  _writer;
   Stack<size_t> _bookmarks;

public:
   int newBookmark()
   {
      _bookmarks.push(_writer.Position());

      return _bookmarks.Count();
   }

   void removeBookmark()
   {
      _bookmarks.pop();
   }

   void clear()
   {
      _writer.seek(0);
      _bookmarks.clear();
   }

   void insert(int position, LexicalType type, ref_t argument);
   void insert(int position, LexicalType type)
   {
      insert(type, 0);
   }
   void insert(LexicalType type, ref_t argument)
   {
      insert(0, type, argument);
   }
   void insert(LexicalType type)
   {
      insert(0, type, 0);
   }
   void insertChild(int position, LexicalType type, ref_t argument)
   {
      insert(position, type, argument);
      insert(position + 8, lxEnding, 0);
   }
   void insertChild(LexicalType type, ref_t argument)
   {      
      insert(lxEnding, 0);
      insert(type, argument);
   }

   void newNode(LexicalType type, ref_t argument);
   void newNode(LexicalType type)
   {
      newNode(type, 0);
   }
   void appendNode(LexicalType type, ref_t argument)
   {
      newNode(type, argument);
      closeNode();
   }
   void appendNode(LexicalType type)
   {
      newNode(type);
      closeNode();
   }

   void closeNode();

   SyntaxWriter(_Memory* dump)
      : _writer(dump)
   {
   }
};

// --- SyntaxTree ---

class SyntaxTree
{
public:
   // --- Node ---
   class Node
   {
      friend class SyntaxTree;

      SyntaxTree*   tree;
      size_t        position;

      Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument);

   public:
      LexicalType   type;
      ref_t         argument;

      bool operator == (LexicalType type)
      {
         return this->type == type;
      }
      bool operator != (LexicalType type)
      {
         return this->type != type;
      }

      void operator = (LexicalType type)
      {
         this->type = type;

         int prevPos = tree->_reader.Position();

         tree->_reader.seek(position - 8);
         *(int*)(tree->_reader.Address()) = (int)type;
         tree->_reader.seek(prevPos);
      }

      Node firstChild() const
      {
         return tree->readFirstNode(position);
      }

      Node nextNode() const
      {
         return tree->readNextNode(position);
      }

      Node prevNode() const
      {
         return tree->readPreviousNode(position);
      }

      Node()
      {
         type = lxNone;
         argument = 0;

         tree = NULL;
      }
   };

private:
   MemoryReader _reader;

   Node read();

public:
   static Node findChild(Node node, LexicalType type)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type) {
         current = current.nextNode();
      }

      return current;
   }

   static Node findChild(Node node, LexicalType type1, LexicalType type2)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2)
            return current;

         current = current.nextNode();
      }

      return current;
   }

   static Node findChild(Node node, LexicalType type1, LexicalType type2, LexicalType type3)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2 || current == type3)
            return current;

         current = current.nextNode();
      }

      return current;
   }

   static bool existChild(Node node, LexicalType type)
   {
      Node child = findChild(node, type);

      return child == type;
   }

   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);
   Node readPreviousNode(size_t position);

   SyntaxTree(_Memory* dump)
      : _reader(dump)
   {      
   }
};

} // _ELENA_

#endif // syntaxTreeH
