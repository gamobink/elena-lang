//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenamachineH
#define elenamachineH 1

#include "libman.h"

namespace _ELENA_
{

// --- Instance ---

class Instance
{
public:
   class ImageSection : public _Memory
   {
      void*  _section;
      size_t _length;

   public:
      void init(void* section, size_t length)
      {
         _section = section;
         _length = length;
      }
      int init(void* section, IdentifierString& package)
      {
         _section = section;
         _length = 4;

         MemoryReader reader(this);
         _length = reader.getDWord();

         reader.getDWord();
         reader.readString(package);

         return reader.Position();
      }

      virtual size_t Length() const
      {
         return _length;
      }

      virtual void* get(size_t position) const;

      virtual bool read(size_t position, void* s, size_t length);

      virtual bool write(size_t position, const void* s, size_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void insert(size_t position, const void* s, size_t length)
      {
         // insert operations are not supported
      }

      virtual bool writeBytes(size_t position, char value, size_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void trim(size_t position)
      {
         // trim operations are not supported
      }

      ImageSection()
      {
         _section = NULL;
         _length = 0;
      }
   };

private:
   Path           _rootPath;
   ImageSection   _debugSection;
   size_t         _debugOffset;
   LibraryManager _loader;
   MessageMap     _verbs;

   bool loadConfig(path_t configPath);

   bool initSubjectSection(ImageSection& subjectSection);

public:
   int readCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   int loadAddressInfo(size_t retPoint, char* lineInfo, size_t length);

   int loadClassName(size_t classAddress, char* buffer, size_t length);
   int loadSubjectName(size_t subjectRef, char* buffer, size_t length);
   int loadMessageName(size_t subjectRef, char* buffer, size_t length);

   void* loadSymbol(ident_t name);
   void* loadSubject(ident_t name);

   void init(void* debugSection, path_t configPath);

   Instance(path_t rootPath);

   virtual ~Instance()
   {
   }
};

} // _ELENA_

#endif // elenamachineH

