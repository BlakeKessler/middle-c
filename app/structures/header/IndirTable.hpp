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
            static constexpr uint8 ONE_TOTAL_REPEAT = 0;
            static constexpr uint8 MAX_TOTAL_REPEATS = 0b111;

            constexpr Entry(uint8 data):_data{data} {}

            constexpr Entry():_data() {}
            constexpr Entry(Type type, bool isconst, bool isvol, bool isatom):_type{type},_isConst{isconst},_isVol{isvol},_isAtom{isatom},_rle{ONE_TOTAL_REPEAT} {}
            
            Type    type() const { return _type; }
            bool   isPtr() const { return _type == PTR;   }
            bool   isRef() const { return _type == REF;   }
            bool isSlice() const { return _type == SLICE; }
            bool   isArr() const { return _type == ARR;   }
            
            bool decaysToFund() const { return _type != REF; } //!NOTE: check that this still works if new indirect types are added
            bool decaysToPtr()  const { return _type != REF; } //!NOTE: check that this still works if new indirect types are added

            bool isConst() const { return _isConst && !isArr(); }
            bool isVolatile() const { return _isVol; }
            bool isAtomic() const { return _isAtom; }

            bool extIsIndex() const { return isArr() && _isConst; }

            uint8 extraRepeats() const { debug_assert(!isArr() || extIsIndex()); return _rle; }
            uint8 totalRunLength() const { debug_assert(!isArr() || extIsIndex()); return _rle + 1; }
            uint8 arrSizeBytes() const { debug_assert(isArr() && !extIsIndex()); return _rle + 1; }

            bool setQuals(QualMask quals); //returns whether illegal modifiers are included

            explicit operator bool() const { return _data; }
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
            EntryBlock& operator=(const EntryBlock& other) { return *new (this) EntryBlock(other); }

            Entry& operator[](ubyte i) { return _elems[i]; }
            Entry operator[](ubyte i) const { return _elems[i]; }

            EntryBlock operator&(const uint64 mask) const { return {_data & mask}; }
            EntryBlock operator^(const EntryBlock mask) const { return {_data ^ mask._data}; }

            operator uint64() const { return _data; }
            explicit operator bool() const { return _data; }
            bool operator==(const EntryBlock other) const { return _data == other._data; }
      };
      static_assert(sizeof(EntryBlock) == sizeof(uint64));
      static_assert(sizeof(EntryBlock) == sizeof(Entry) << 3);

      uint64 _size;
      EntryBlock _block0;
      mcsl::dyn_arr<EntryBlock> _otherBlocks;
      ubyte _backExtraBytes;

      enum __ctorNames {
         MAKE_DEREF
      };
      template<__ctorNames ctorName> struct inPlace{};
      template<__ctorNames ctorName> IndirTable(const IndirTable&, inPlace<ctorName>);
   public:
      // constexpr IndirTable():_size{0},_block0{},_otherBlocks{} {}
      constexpr IndirTable(Entry firstEntry):_size{1},_block0{firstEntry},_otherBlocks{},_backExtraBytes{0} {}
      IndirTable(const IndirTable&);
      IndirTable(IndirTable&&);

      IndirTable(const IndirTable&, Entry);
      static IndirTable makeAppended(const IndirTable& table, Entry entry) {
         return IndirTable(table, entry);
      }
      static IndirTable makeDeref(const IndirTable& ptr);

      IndirTable& operator=(const IndirTable&);
      IndirTable& operator=(IndirTable&&);

      uint64 size() const { return _size; }

      void release();

      Entry& operator[](uint64 i);
      Entry operator[](uint64 i) const;
      Entry& back() { return self[_size - 1 - _backExtraBytes]; }
      Entry back() const { return self[_size - 1 - _backExtraBytes]; }

      struct ArrExt {
         union {
            index<Expr> i;
            uint64 size;
         };
         bool isIndex;

         static ArrExt make(uint64 i = 0, bool isI = false) {
            return {.i = i, .isIndex = isI};
         }
      };
      ArrExt arrExtent(uint64 i) const;
      uint entryByteCount(uint64 i) const;

      void appendArrExtent(uint64 extent, ubyte byteCount);
      void append(Entry entry);
      void appendView(Entry entry);
   private:
      void __push_back(Entry entry);
   public:
      explicit operator bool() const { return _size; }
      bool operator==(const IndirTable& other) const;

      auto hash() const {
         return mcsl::hash_algos::rapid(
            _otherBlocks.begin(),
            _otherBlocks.size() * sizeof(EntryBlock),
            (uint64)_block0
         );
      }
};

template<> struct std::hash<clef::IndirTable> {
   static inline auto operator()(const clef::IndirTable& table) {
      return table.hash();
   }
};

#endif //INDIR_TABLE_HPP