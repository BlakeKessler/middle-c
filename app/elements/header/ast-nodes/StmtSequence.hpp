#pragma once
#ifndef STMT_SEQUENCE_HPP
#define STMT_SEQUENCE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Statement.hpp"

#include "dyn_arr.hpp"

struct clef::StmtSequence : public mcsl::dyn_arr<Statement*> {
   
};

#endif //STMT_SEQUENCE_HPP