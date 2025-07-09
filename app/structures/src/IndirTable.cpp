#ifndef INDIR_TABLE_CPP
#define INDIR_TABLE_CPP

#include "IndirTable.hpp"

clef::IndirTable::IndirTable(const IndirTable& other): 
   _size(other._size),
   _block0(other._block0),
   _otherBlocks(other._otherBlocks),
   _backExtraBytes(other._backExtraBytes) {

}
clef::IndirTable::IndirTable(IndirTable&& other): 
   _size(other._size),
   _block0(other._block0),
   _otherBlocks(std::move(other._otherBlocks)),
   _backExtraBytes(other._backExtraBytes) {
      if (this != &other) {
         other.release();
      }
}

clef::IndirTable& clef::IndirTable::operator=(const IndirTable& other) {
   return *new (this) IndirTable(other);
}
clef::IndirTable& clef::IndirTable::operator=(IndirTable&& other) {
   return *new (this) IndirTable(std::move(other));
}

void clef::IndirTable::release() {
   _otherBlocks.release();
}

clef::IndirTable::Entry& clef::IndirTable::operator[](uint64 i) {
   assume(i < _size);
   if (i < 8) {
      return _block0[i];
   }
   return ((Entry*)_otherBlocks.begin())[i - 8];
}
clef::IndirTable::Entry clef::IndirTable::operator[](uint64 i) const {
   assume(i < _size);
   if (i < 8) {
      return _block0[i];
   }
   return ((Entry*)_otherBlocks.begin())[i - 8];
}

bool clef::IndirTable::operator==(const IndirTable& other) const {
   if (_size != other._size) {
      return false;
   }
   if (_size == 0) {
      return true;
   }
   if (_block0 != other._block0) { //relies on _block0 being zero-initialized
      return false;
   }

   uint i = _size >> 3;
   if (!(_size & 7)) {
      goto LOOP;
   }
   if (i--) {
      {
         using enum mcsl::sys_endian;
         uint64 mask;
         if constexpr (SYS == LITTLE) {
            mask = ~0ull >> ((7 & -_size) << 3);
         } else {
            mask = ~0ull << ((7 & -_size) << 3);
         }
         if ((_otherBlocks[i] ^ other._otherBlocks[i]) & mask) {
            return false;
         }
      }
      LOOP:
      while (i--) {
         if (_otherBlocks[i] != other._otherBlocks[i]) {
            return false;
         }
      }
   }
   return true;
}

void clef::IndirTable::append(Entry entry) {
   debug_assert(_size);
   if (Entry& back = self.back(); !back.isArr() && entry.isSame(back) && back._rle != Entry::MAX_TOTAL_REPEATS) {
      ++back._rle;
   } else {
      debug_assert(entry._rle == Entry::ONE_TOTAL_REPEAT);
      __push_back(entry);
      _backExtraBytes = 0;
   }
}
void clef::IndirTable::appendView(Entry entry) {
   debug_assert(_size);
   Entry& back = self.back();
   if (back.isArr()) { TODO; }
   if (!back.isConst()) {
      if (back._rle != Entry::ONE_TOTAL_REPEAT) {
         --back._rle;
         Entry newBack = back;
         newBack._isConst = true;
         newBack._rle = Entry::ONE_TOTAL_REPEAT;
         __push_back(newBack);
      } else {
         back._isConst = true;
      }
   }
   append(entry);
}
void clef::IndirTable::__push_back(Entry entry) {
   debug_assert(_size);
   if (_size < 8) {
      _block0[_size] = entry;
   } else {
      if (_size & 7) {
         _otherBlocks.back()[_size & 7] = entry;
      } else {
         _otherBlocks.emplace_back(entry);
      }
   }
   ++_size;
}
#include "pretty-print.hpp"
bool clef::IndirTable::Entry::setQuals(QualMask quals) {
   using enum QualMask;
   //allowed qualifiers (besides VIEW, which is encoded in the previous entry instead of the current one)
   _isConst = +(quals & CONST);
   _isVol = +(quals & VOLATILE);
   _isAtom = +(quals & ATOMIC);

   //check for other qualifiers
   constexpr QualMask badMask = ~(CONST | VIEW | VOLATILE | ATOMIC);
   return +(quals & badMask);
}
bool clef::IndirTable::Entry::isSame(const Entry other) const {
   constexpr Entry mask(__all_bits_set, true, true, true);
   return (_data ^ other._data) & mask._data;
}

//get the extent of the entry at index i
clef::IndirTable::ArrExt clef::IndirTable::arrExtent(uint64 i) const {
   //get entry
   Entry entry = self[i];

   //check that the entry represents an array
   if (!entry.isArr()) {
      return ArrExt{
         .size = 0,
         .isIndex = false
      };
   }

   //calculate the number of bytes
   ubyte bytes = entry.extIsIndex() ? sizeof(index<Expr>) : entry.arrSizeBytes();
   debug_assert(bytes);
   debug_assert(i + bytes < _size);
   
   //read extent //!NOTE: modifies i
   uint64 n = 0;
   do {
      --bytes;
      n = n << 8 | self[++i]._data;
   } while (bytes);

   //return
   return ArrExt::make(n, entry.extIsIndex());
}
//append the extent of an array to the table
void clef::IndirTable::appendArrExtent(uint64 ext, ubyte bytes) {
   //safety and debug checks
   assume(bytes);
   debug_assert(bytes <= sizeof(ext));
   debug_assert(back().isArr());

   //update _backExtraBytes
   _backExtraBytes = bytes;

   //ensure the correct byte order (big-endian)
   ext = mcsl::byteswap(ext);
   ext >>= 8 * bytes;
   //push bytes
   do {
      __push_back((uint8)ext);
      ext >>= 8;
   } while (--bytes);
}

#endif //INDIR_TABLE_CPP