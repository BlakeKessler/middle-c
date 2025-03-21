#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include "pretty-print.hpp"
#include "io.hpp"

#include "MAP_MACRO.h"

#define ID(fund) id = fund; break
//!NOTE: currently uses LP64 data model no matter what
clef::index<clef::FundType> clef::SyntaxTree::getFundType(const KeywordID keyword) {
   using enum FundTypeID;
   FundTypeID id;
   switch (keyword) {
      case KeywordID::VOID       : ID(VOID);
      case KeywordID::AUTO       : ID(AUTO);


      case KeywordID::CHAR       : ID(CHAR);
      case KeywordID::CHAR_UTF_8 : ID(CHAR8);
      case KeywordID::CHAR_UTF_16: ID(CHAR16);
      case KeywordID::CHAR_UTF_32: ID(CHAR32);


      case KeywordID::BOOL       : ID(BOOL);
      case KeywordID::UBYTE      : ID(UINT8);
      case KeywordID::USHORT     : ID(UINT16);
      case KeywordID::UINT       : ID(UINT32);
      case KeywordID::ULONG      : ID(UINT64);
      case KeywordID::UINT_PTR   : ID(UINT64);
      case KeywordID::UWORD      : ID(UINT64);

      case KeywordID::SIGN_T     : ID(SIGN_T);
      case KeywordID::SBYTE      : ID(SINT8);
      case KeywordID::SSHORT     : ID(SINT16);
      case KeywordID::SINT       : ID(SINT32);
      case KeywordID::SLONG      : ID(SINT64);
      case KeywordID::SINT_PTR   : ID(SINT64);
      case KeywordID::SWORD      : ID(SINT64);


      case KeywordID::UINT_8     : ID(UINT8);
      case KeywordID::UINT_16    : ID(UINT16);
      case KeywordID::UINT_32    : ID(UINT32);
      case KeywordID::UINT_64    : ID(UINT64);
      case KeywordID::UINT_128   : ID(UINT128);
      case KeywordID::UINT_256   : ID(UINT256);

      case KeywordID::SINT_8     : ID(SINT8);
      case KeywordID::SINT_16    : ID(SINT16);
      case KeywordID::SINT_32    : ID(SINT32);
      case KeywordID::SINT_64    : ID(SINT64);
      case KeywordID::SINT_128   : ID(SINT128);
      case KeywordID::SINT_256   : ID(SINT256);

      case KeywordID::FLOAT      : ID(FLOAT32);

      case KeywordID::FLOAT_16   : ID(FLOAT16);
      case KeywordID::FLOAT_32   : ID(FLOAT32);
      case KeywordID::FLOAT_64   : ID(FLOAT64);
      case KeywordID::FLOAT_128  : ID(FLOAT128);
      case KeywordID::FLOAT_256  : ID(FLOAT256);


      case KeywordID::ENUM       : ID(ENUM);
      case KeywordID::MASK       : ID(MASK);
      case KeywordID::UNION      : ID(UNION);
      case KeywordID::NAMESPACE  : ID(NAMESPACE);
      case KeywordID::INTERFACE  : ID(INTERFACE);
      case KeywordID::STRUCT     : ID(STRUCT);
      case KeywordID::CLASS      : ID(CLASS);

      default: UNREACHABLE;
   }
   return make<FundType>(id);
}
#undef ID

clef::index<clef::astNode> clef::SyntaxTree::getValueKeyword(const KeywordID keyword) {
   switch (keyword) {
      case KeywordID::THIS    : return +make<Identifier>(KeywordID::THIS);
      case KeywordID::SELF    : return +make<Identifier>(KeywordID::SELF);
      case KeywordID::TRUE    : return +make<Literal>(true);
      case KeywordID::FALSE   : return +make<Literal>(false);
      case KeywordID::NULLPTR : return +make<Literal>((void*)nullptr);

      default: UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> index) {
   astNode& node = self[index];
   switch (node.nodeType()) {
      case Identifier::nodeType():
      case Variable::nodeType():
      case Function::nodeType():
      case Type::nodeType():
      case VariadicParameter::nodeType():
      case FundamentalType::nodeType():
      case FunctionSignature::nodeType():
      case Enum::nodeType():
      case Mask::nodeType():
      case Union::nodeType():
      case Namespace::nodeType():
      case Interface::nodeType():
      case Struct::nodeType():
      case Class::nodeType():
      case GenericType::nodeType():
      case Literal::nodeType():
         return make<Expr>(op, node.nodeType(), index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case Statement::nodeType():
      case ForLoop::nodeType():
      case ForeachLoop::nodeType():
      case WhileLoop::nodeType():
      case DoWhileLoop::nodeType():
      case If::nodeType():
      case Switch::nodeType():
      case Match::nodeType():
      case TryCatch::nodeType():
      case Asm::nodeType():
         return +index;


      case Scope::nodeType():
      case ForLoopParams::nodeType():
      case ForeachLoopParams::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StatementSequence::nodeType():
      case ArgumentList::nodeType():
      case ParameterList::nodeType():
      case SpecializerList::nodeType():
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> lhsI, index<astNode> rhsI) {
   return make<Expr>(op, self[lhsI].nodeType(), self[rhsI].nodeType(), lhsI, rhsI);
}


void clef::SyntaxTree::print(mcsl::File& file) const {
   for (uint i = 1; i < _buf.size(); ++i) {
      file.printf(mcsl::FMT("\n\t%u: "), i);
      mcsl::write(mcsl::stdout, _buf[i]);
   }
   file.printf(mcsl::FMT("\n"));
}

void clef::SyntaxTree::printf(mcsl::File& file) const {
   __printf(file, (index<astNode>)(1), 0);
}

void clef::SyntaxTree::__indent(mcsl::File& file, uint indents) const {
   file.write(mcsl::PAD_CHAR, 3 * indents);
}

template<> void clef::SyntaxTree::__printf<clef::ArgList>(mcsl::File& file, index<const ArgList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ParamList>(mcsl::File& file, index<const ParamList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::SpecList>(mcsl::File& file, index<const SpecList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::Identifier>(mcsl::File& file, index<const Identifier> i, const uint indents) const {
   const Identifier& node = self[i];
   if (node.scopeName()) { //parent scope
      __printf<Identifier>(file, node.scopeName(), indents);
      file.printf(mcsl::FMT("::"));
   } else { //indent
      __indent(file, indents);
   }

   if (+node.keywordID()) { //keyword
      file.printf(mcsl::FMT("%s"), toString(node.keywordID()));
   } else { //standard identifier
      file.printf(mcsl::FMT("%s"), node.name().size(), node.name().begin());
   }

   if (node.specializer()) {
      __printf<SpecializerList>(file, node.specializer(), 0);
   }
}
template<> void clef::SyntaxTree::__printf<clef::Type>(mcsl::File& file, index<const Type> i, const uint indents) const {
   __printf<Identifier>(file, i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Expr>(mcsl::File& file, index<const Expr> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Stmt>(mcsl::File& file, index<const Stmt> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::StmtSeq>(mcsl::File& file, index<const StmtSeq> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Variable>(mcsl::File& file, index<const Variable> i, const uint indents) const {
   const Variable& node = self[i];
   __printf<Type>(file, node.type(), indents);
   __printf<Identifier>(file, i, 0);
   if (node.val()) {
      file.printf(mcsl::FMT(" = "));
      __printf<Expr>(file, node.val(), 0);
   }
}
template<> void clef::SyntaxTree::__printf<clef::VariadicParam>(mcsl::File& file, index<const VariadicParam> i, const uint indents) const {
   __printf<Type>(file, i, indents);
   file.printf(mcsl::FMT("..."));
}
template<> void clef::SyntaxTree::__printf<clef::FundamentalType>(mcsl::File& file, index<const FundamentalType> i, const uint indents) const {
   __indent(file, indents);
   file.printf(mcsl::FMT("%s"), toString(self[i].id()));
}
template<> void clef::SyntaxTree::__printf<clef::FunctionSignature>(mcsl::File& file, index<const FunctionSignature> i, const uint indents) const {
   const FuncSig& node = self[i];
   __printf<ParamList>(file, node.params(), indents);
   file.printf(mcsl::FMT(" -> "));
   __printf<Type>(file, node.returnType(), 0);
}
template<> void clef::SyntaxTree::__printf<clef::Scope>(mcsl::File& file, index<const Scope> i, const uint indents) const {
   __printf<StmtSeq>(file, i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Function>(mcsl::File& file, index<const Function> i, const uint indents) const {
   const Function& node = self[i];
   __printf<Identifier>(file, i, indents);
   __printf<FuncSig>(file, node.signature(), 0);
   file.printf(mcsl::FMT(" "));
   __printf<Scope>(file, node.procedure(), indents);
}
template<> void clef::SyntaxTree::__printf<clef::Enum>(mcsl::File& file, index<const Enum> i, const uint indents) const {
   const Enum& node = self[i];
   __indent(file, indents);
   file.printf(mcsl::FMT("enum "));
   __printf<Type>(file, i, 0);

   if (node.baseType()) {
      file.printf(mcsl::FMT(" : "));
      __printf<Type>(file, node.baseType(), 0);
   }

   file.printf(mcsl::FMT(" {\n"));
   const ParamList& members = self[node.enumerators()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(file, members[index], indents + 1);
      file.printf(mcsl::FMT(";\n"));
   }
   __indent(file, indents);
   file.printf(mcsl::FMT("};\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Mask>(mcsl::File& file, index<const Mask> i, const uint indents) const {
   const Mask& node = self[i];
   __indent(file, indents);
   file.printf(mcsl::FMT("mask "));
   __printf<Type>(file, i, 0);

   if (node.baseType()) {
      file.printf(mcsl::FMT(" : "));
      __printf<Type>(file, node.baseType(), 0);
   }
   
   file.printf(mcsl::FMT(" {\n"));
   const ParamList& members = self[node.enumerators()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(file, members[index], indents + 1);
      file.printf(mcsl::FMT(";\n"));
   }
   __indent(file, indents);
   file.printf(mcsl::FMT("};\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Union>(mcsl::File& file, index<const Union> i, const uint indents) const {
   const Union& node = self[i];
   __indent(file, indents);
   file.printf(mcsl::FMT("union "));
   __printf<Type>(file, i, 0);
   
   file.printf(mcsl::FMT(" {\n"));
   const ParamList& members = self[node.members()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(file, members[index], indents + 1);
      file.printf(mcsl::FMT(";\n"));
   }
   __indent(file, indents);
   file.printf(mcsl::FMT("};\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Namespace>(mcsl::File& file, index<const Namespace> i, const uint indents) const {
   __indent(file, indents);
   file.printf(mcsl::FMT("namespace "));
   __printf<Type>(file, i, 0);
   file.printf(mcsl::FMT(" "));
   __printf(file, self[i].spec(), indents + 1);
   file.printf(mcsl::FMT(";\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Interface>(mcsl::File& file, index<const Interface> i, const uint indents) const {
   __indent(file, indents);
   file.printf(mcsl::FMT("interface "));
   __printf<Type>(file, i, 0);
   file.printf(mcsl::FMT(" "));
   __printf(file, self[i].spec(), indents + 1);
   file.printf(mcsl::FMT(";\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Struct>(mcsl::File& file, index<const Struct> i, const uint indents) const {
   __indent(file, indents);
   file.printf(mcsl::FMT("struct "));
   __printf<Type>(file, i, 0);
   file.printf(mcsl::FMT(" "));
   __printf(file, self[i].spec(), indents + 1);
   file.printf(mcsl::FMT(";\n"));
}
template<> void clef::SyntaxTree::__printf<clef::Class>(mcsl::File& file, index<const Class> i, const uint indents) const {
   __indent(file, indents);
   file.printf(mcsl::FMT("class "));
   __printf<Type>(file, i, 0);
   file.printf(mcsl::FMT(" "));
   __printf(file, self[i].spec(), indents + 1);
   file.printf(mcsl::FMT(";\n"));
}
template<> void clef::SyntaxTree::__printf<clef::GenericType>(mcsl::File& file, index<const GenericType> i, const uint indents) const {
   __printf<Type>(file, i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Literal>(mcsl::File& file, index<const Literal> i, const uint indents) const {
   const Literal& node = self[i];
   switch (node.type()) {
      case LitType::NONE: UNREACHABLE;
      case LitType::POINTER: file.printf(mcsl::FMT("%r"), (const void*)node); break;

      case LitType::UINT: file.printf(mcsl::FMT("%u"), (ulong)node); break;
      case LitType::SINT: file.printf(mcsl::FMT("%i"), (slong)node); break;
      case LitType::FLOAT: file.printf(mcsl::FMT("%f"), (flong)node); break;

      case LitType::CHAR: file.printf(mcsl::FMT("'%c'"), (char)node); break;
      case LitType::STRING: file.printf(mcsl::FMT("\"%s\""), ((const mcsl::str_slice)node).size(), ((const mcsl::str_slice)node).begin()); break;
      case LitType::INTERP_STR: file.printf(mcsl::FMT("`%s`"), ((const mcsl::str_slice)node).size(), ((const mcsl::str_slice)node).begin()); break;
      
      case LitType::FORMAT: UNREACHABLE; //!TODO: implement
      case LitType::REGEX: UNREACHABLE; //!TODO: implement

      case LitType::TYPEID: __printf<Type>(file, (index<const Type>)node, indents); break;
   }
}
template<> void clef::SyntaxTree::__printf<clef::Decl>(mcsl::File& file, index<const Decl> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}


template<> void clef::SyntaxTree::__printf<clef::SwitchCases>(mcsl::File& file, index<const SwitchCases> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::MatchCases>(mcsl::File& file, index<const MatchCases> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Switch>(mcsl::File& file, index<const Switch> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Match>(mcsl::File& file, index<const Match> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::TryCatch>(mcsl::File& file, index<const TryCatch> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::ForLoopParams>(mcsl::File& file, index<const ForLoopParams> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForeachLoopParams>(mcsl::File& file, index<const ForeachLoopParams> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForLoop>(mcsl::File& file, index<const ForLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForeachLoop>(mcsl::File& file, index<const ForeachLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::WhileLoop>(mcsl::File& file, index<const WhileLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::DoWhileLoop>(mcsl::File& file, index<const DoWhileLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::If>(mcsl::File& file, index<const If> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Asm>(mcsl::File& file, index<const Asm> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

void clef::SyntaxTree::__printf(mcsl::File& file, index<const InterfaceSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
void clef::SyntaxTree::__printf(mcsl::File& file, index<const NamespaceSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
void clef::SyntaxTree::__printf(mcsl::File& file, index<const ObjTypeSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

#define typecase(T) case T::nodeType(): __printf<T>(file, +i, indents); break;
void clef::SyntaxTree::__printf(mcsl::File& file, index<const astNode> i, uint indents) const {
   switch (self[i].nodeType()) {
      case NodeType::NONE: break;
      case NodeType::ERROR: throwError(ErrCode::UNSPEC, mcsl::FMT("\n\033[31mERROR NODE: %u\033[39m\n"), +i); break;

      MCSL_MAP(typecase, ALL_AST_NODE_T)
   }
}
#undef typecase

#include "MAP_MACRO_UNDEF.h"

#endif //SYNTAX_TREE_CPP