#ifndef INDIR_TABLE_HPP
#define INDIR_TABLE_HPP

#include "CLEF.hpp"
#include "dyn_arr.hpp"

class clef::IndirTable {
   public:
      struct Entry {
         public:
            union {
               uint8 _data;
               struct {
                  bool _isPtr   : 1;
                  bool _isConst : 1;
                  bool _isVol   : 1;
                  bool _isAtom  : 1;
                  uint8 _rle    : 4;
               };
            };
         private:
            constexpr Entry(uint8 data):_data{data} {}
         public:
            constexpr Entry():_data() {}
            constexpr Entry(bool isptr, bool isconst, bool isvol, bool isatom):_isPtr{isptr},_isConst{isconst},_isVol{isvol},_isAtom{isatom},_rle{0} {}
            bool isRef() const { return !_isPtr; }

            bool setQuals(QualMask quals); //returns whether illegal modifiers are included

            operator bool() const { return _data; }
            bool operator==(const Entry other) const { return _data == other._data; }
            bool isSame(const Entry other) const;
      };
      static_assert(sizeof(Entry) == sizeof(uint8));
   private:
      struct EntryBlock {
         public:
            union {
               uint64 _data;
               Entry _elems[8];
            };

            // constexpr EntryBlock():_data(0) {}
         private:
            constexpr EntryBlock(uint64 d):_data(d) {}
         public:
            constexpr EntryBlock(const EntryBlock& other):_data(other._data) {}
            constexpr EntryBlock(Entry first):_data{} { _elems[0] = first; }

            Entry& operator[](ubyte i) { return _elems[i]; }
            Entry operator[](ubyte i) const { return _elems[i]; }

            EntryBlock operator&(const uint64 mask) const { return {_data & mask}; }
            EntryBlock operator^(const EntryBlock mask) const { return {_data ^ mask}; }

            operator bool() const { return _data; }
            bool operator==(const EntryBlock other) const { return _data == other._data; }
      };
      static_assert(sizeof(EntryBlock) == sizeof(uint64));
      static_assert(sizeof(EntryBlock) == sizeof(Entry) << 3);

      uint64 _size;
      EntryBlock _block0;
      mcsl::dyn_arr<EntryBlock> _otherBlocks;
   public:
      // constexpr IndirTable():_size{0},_block0{},_otherBlocks{} {}
      constexpr IndirTable(Entry firstEntry):_size{0},_block0{firstEntry},_otherBlocks{} {}
      IndirTable(const IndirTable&);
      IndirTable(IndirTable&&);

      Entry& operator[](uint64 i);
      Entry operator[](uint64 i) const;

      void append(Entry entry);
      void appendView(Entry entry);
   private:
      void __push_back(Entry entry);
   public:
      operator bool() const { return _size; }
      bool operator==(const IndirTable& other) const;
};

#endif //INDIR_TABLE_HPP