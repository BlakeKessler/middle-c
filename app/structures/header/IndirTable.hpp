#ifndef INDIR_TABLE_HPP
#define INDIR_TABLE_HPP

#include "CLEF.hpp"
#include "dyn_arr.hpp"

class clef::IndirTable {
   public:
      struct Entry {
         public:
            enum Type : uint8 {
               PTR   = 0,
               REF   = 1,
               SLICE = 2,
               ARR   = 3,

               __all_bits_set = 3
            };
            union {
               uint8 _data;
               struct {
                  Type _type    : 2;
                  bool _isConst : 1;
                  bool _isVol   : 1;
                  bool _isAtom  : 1;
                  uint8 _rle    : 3;
               };
            };
            //!NOTE: non-const array literal is meaningless
            //!TODO: isArr() && _isConst -> has an RLE byte?
            static constexpr uint8 ONE_TOTAL_REPEAT = 0;
            static constexpr uint8 MAX_TOTAL_REPEATS = 0b111;
         private:
            constexpr Entry(uint8 data):_data{data} {}
         public:
            constexpr Entry():_data() {}
            constexpr Entry(Type type, bool isconst, bool isvol, bool isatom):_type{type},_isConst{isconst},_isVol{isvol},_isAtom{isatom},_rle{ONE_TOTAL_REPEAT} {}
            
            Type    type() const { return _type; }
            bool   isPtr() const { return _type == PTR;   }
            bool   isRef() const { return _type == REF;   }
            bool isSlice() const { return _type == SLICE; }
            bool   isArr() const { return _type == ARR;   }

            bool isConst() const { return _isConst && !isArr(); }

            uint8 extraRepeats() const { debug_assert(!isArr()); return _rle; }
            uint8 totalRunLength() const { debug_assert(!isArr()); return _rle + 1; }
            uint8 arrSizeBytes() const { debug_assert(isArr()); return _rle + 1; }

            bool setQuals(QualMask quals); //returns whether illegal modifiers are included

            operator bool() const { return _data; }
            bool operator==(const Entry other) const { return _data == other._data; }
            bool isSame(const Entry other) const;
         private:
            static uint64 readArrExtent(Entry* addr, ubyte byteC);
            static uint64 readArrExtent(Entry* addr1, Entry* addr2, ubyte byteC1, ubyte byteC2);
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

      IndirTable& operator=(const IndirTable&);
      IndirTable& operator=(IndirTable&&);

      void release();

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