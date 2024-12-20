#pragma once
#ifndef STMT_SEQUENCE_HPP
#define STMT_SEQUENCE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Statement.hpp"

#include "dyn_arr.hpp"

struct clef::StmtSequence : public mcsl::dyn_arr<Statement*> {
   private:
      bool _isClosed{};

   public:


      bool isClosed() const { return _isClosed; }

      void  open() { _isClosed = false; }
      void close() { _isClosed = true; }
};

#endif //STMT_SEQUENCE_HPP