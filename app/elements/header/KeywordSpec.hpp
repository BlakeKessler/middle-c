#ifndef KEYWORD_SPEC_HPP
#define KEYWORD_SPEC_HPP

#include "CLEF.hpp"

#include "raw_buf_str.hpp"
#include "raw_str_span.hpp"

struct alignas(8) clef::KeywordSpec {
   private:
      KeywordID _id;
      mcsl::raw_buf_str<MAX_KEYWORD_LEN> _str;
   public:
      constexpr KeywordSpec():_id{},_str{} {}
      template<mcsl::str_t str_t> constexpr KeywordSpec(const str_t& str, const KeywordID id)
         :_id{id},_str{str} { assert(str.size() <= MAX_KEYWORD_LEN); }
      
      uint size() const { return _str.size(); }
      const mcsl::raw_str_span toString() const { return mcsl::raw_str_span(const_cast<mcsl::raw_buf_str<14>&>(_str)); }

      operator bool() const { return +_id; }
      operator KeywordID() const { return _id; }
      operator const mcsl::raw_str_span() const { return mcsl::raw_str_span(const_cast<mcsl::raw_buf_str<14>&>(_str)); }
};

#endif //KEYWORD_SPEC_HPP