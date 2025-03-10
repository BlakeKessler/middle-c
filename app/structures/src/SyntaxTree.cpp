#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include "pretty-print.hpp"
#include <cstdio>

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


void clef::SyntaxTree::print() const {
   for (uint i = 1; i < _buf.size(); ++i) {
      std::printf("\n\t%u: ", i);
      _buf[i].printf();
   }
   std::printf("\n");
}

void clef::SyntaxTree::printf() const {
   __printf((index<astNode>)(1), 0);
}

void clef::SyntaxTree::__indent(uint indents) const {
   while (indents--) {
      std::printf("   ");
   }
}

template<> void clef::SyntaxTree::__printf<clef::ArgList>(index<const ArgList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ParamList>(index<const ParamList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::SpecList>(index<const SpecList> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::Identifier>(index<const Identifier> i, const uint indents) const {
   const Identifier& node = self[i];
   if (node.scopeName()) { //parent scope
      __printf<Identifier>(node.scopeName(), indents);
      std::printf("::");
   } else { //indent
      __indent(indents);
   }

   if (+node.keywordID()) { //keyword
      std::printf("%s", toString(node.keywordID()));
   } else { //standard identifier
      std::printf("%.*s", node.name().size(), node.name().begin());
   }

   if (node.specializer()) {
      __printf<SpecializerList>(node.specializer(), 0);
   }
}
template<> void clef::SyntaxTree::__printf<clef::Type>(index<const Type> i, const uint indents) const {
   __printf<Identifier>(i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Expr>(index<const Expr> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Stmt>(index<const Stmt> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::StmtSeq>(index<const StmtSeq> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Variable>(index<const Variable> i, const uint indents) const {
   const Variable& node = self[i];
   __printf<Type>(node.type(), indents);
   __printf<Identifier>(i, 0);
   if (node.val()) {
      std::printf(" = ");
      __printf<Expr>(node.val(), 0);
   }
}
template<> void clef::SyntaxTree::__printf<clef::VariadicParam>(index<const VariadicParam> i, const uint indents) const {
   __printf<Type>(i, indents);
   std::printf("...");
}
template<> void clef::SyntaxTree::__printf<clef::FundamentalType>(index<const FundamentalType> i, const uint indents) const {
   __indent(indents);
   std::printf("%s", toString(self[i].id()));
}
template<> void clef::SyntaxTree::__printf<clef::FunctionSignature>(index<const FunctionSignature> i, const uint indents) const {
   const FuncSig& node = self[i];
   __printf<ParamList>(node.params(), indents);
   std::printf(" -> ");
   __printf<Type>(node.returnType(), 0);
}
template<> void clef::SyntaxTree::__printf<clef::Scope>(index<const Scope> i, const uint indents) const {
   __printf<StmtSeq>(i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Function>(index<const Function> i, const uint indents) const {
   const Function& node = self[i];
   __printf<Identifier>(i, indents);
   __printf<FuncSig>(node.signature(), 0);
   std::printf(" ");
   __printf<Scope>(node.procedure(), indents);
}
template<> void clef::SyntaxTree::__printf<clef::Enum>(index<const Enum> i, const uint indents) const {
   const Enum& node = self[i];
   __indent(indents);
   std::printf("enum ");
   __printf<Type>(i, 0);

   if (node.baseType()) {
      std::printf(" : ");
      __printf<Type>(node.baseType(), 0);
   }

   std::printf(" {\n");
   const ParamList& members = self[node.enumerators()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(members[index], indents + 1);
      std::printf(";\n");
   }
   __indent(indents);
   std::printf("};\n");
}
template<> void clef::SyntaxTree::__printf<clef::Mask>(index<const Mask> i, const uint indents) const {
   const Mask& node = self[i];
   __indent(indents);
   std::printf("mask ");
   __printf<Type>(i, 0);

   if (node.baseType()) {
      std::printf(" : ");
      __printf<Type>(node.baseType(), 0);
   }
   
   std::printf(" {\n");
   const ParamList& members = self[node.enumerators()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(members[index], indents + 1);
      std::printf(";\n");
   }
   __indent(indents);
   std::printf("};\n");
}
template<> void clef::SyntaxTree::__printf<clef::Union>(index<const Union> i, const uint indents) const {
   const Union& node = self[i];
   __indent(indents);
   std::printf("union ");
   __printf<Type>(i, 0);
   
   std::printf(" {\n");
   const ParamList& members = self[node.members()];
   for (uint index = 0; index < members.size(); ++index) {
      __printf<Variable>(members[index], indents + 1);
      std::printf(";\n");
   }
   __indent(indents);
   std::printf("};\n");
}
template<> void clef::SyntaxTree::__printf<clef::Namespace>(index<const Namespace> i, const uint indents) const {
   __indent(indents);
   std::printf("namespace ");
   __printf<Type>(i, 0);
   std::printf(" ");
   __printf(self[i].spec(), indents + 1);
   std::printf(";\n");
}
template<> void clef::SyntaxTree::__printf<clef::Interface>(index<const Interface> i, const uint indents) const {
   __indent(indents);
   std::printf("interface ");
   __printf<Type>(i, 0);
   std::printf(" ");
   __printf(self[i].spec(), indents + 1);
   std::printf(";\n");
}
template<> void clef::SyntaxTree::__printf<clef::Struct>(index<const Struct> i, const uint indents) const {
   __indent(indents);
   std::printf("struct ");
   __printf<Type>(i, 0);
   std::printf(" ");
   __printf(self[i].spec(), indents + 1);
   std::printf(";\n");
}
template<> void clef::SyntaxTree::__printf<clef::Class>(index<const Class> i, const uint indents) const {
   __indent(indents);
   std::printf("class ");
   __printf<Type>(i, 0);
   std::printf(" ");
   __printf(self[i].spec(), indents + 1);
   std::printf(";\n");
}
template<> void clef::SyntaxTree::__printf<clef::GenericType>(index<const GenericType> i, const uint indents) const {
   __printf<Type>(i, indents);
}
template<> void clef::SyntaxTree::__printf<clef::Literal>(index<const Literal> i, const uint indents) const {
   const Literal& node = self[i];
   switch (node.type()) {
      case LitType::NONE: UNREACHABLE;
      case LitType::POINTER: std::printf("%p", (const void*)node); break;

      case LitType::UINT: std::printf("%lu", (ulong)node); break;
      case LitType::SINT: std::printf("%ld", (slong)node); break;
      case LitType::FLOAT: std::printf("%lf", (double)node); break;

      case LitType::CHAR: std::printf("'%c'", (char)node); break;
      case LitType::STRING: std::printf("\"%.*s\"", ((const mcsl::raw_str_span)node).size(), ((const mcsl::raw_str_span)node).begin()); break;
      case LitType::INTERP_STR: std::printf("`%.*s`", ((const mcsl::raw_str_span)node).size(), ((const mcsl::raw_str_span)node).begin()); break;
      
      case LitType::FORMAT: UNREACHABLE; //!TODO: implement
      case LitType::REGEX: UNREACHABLE; //!TODO: implement

      case LitType::TYPEID: __printf<Type>((index<const Type>)node, indents); break;
   }
}
template<> void clef::SyntaxTree::__printf<clef::Decl>(index<const Decl> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}


template<> void clef::SyntaxTree::__printf<clef::SwitchCases>(index<const SwitchCases> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::MatchCases>(index<const MatchCases> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Switch>(index<const Switch> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Match>(index<const Match> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::TryCatch>(index<const TryCatch> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::ForLoopParams>(index<const ForLoopParams> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForeachLoopParams>(index<const ForeachLoopParams> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForLoop>(index<const ForLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::ForeachLoop>(index<const ForeachLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::WhileLoop>(index<const WhileLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::DoWhileLoop>(index<const DoWhileLoop> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

template<> void clef::SyntaxTree::__printf<clef::If>(index<const If> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
template<> void clef::SyntaxTree::__printf<clef::Asm>(index<const Asm> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

void clef::SyntaxTree::__printf(index<const InterfaceSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
void clef::SyntaxTree::__printf(index<const NamespaceSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}
void clef::SyntaxTree::__printf(index<const ObjTypeSpec> i, const uint indents) const {
   UNREACHABLE;
   //!TODO: implement
}

#define typecase(T) case T::nodeType(): __printf<T>(+i, indents); break
void clef::SyntaxTree::__printf(index<const astNode> i, uint indents) const {
   switch (self[i].nodeType()) {
      case NodeType::NONE: break;
      case NodeType::ERROR: throwError(ErrCode::UNSPEC, "\n\033[31mERROR NODE: %u\033[39m\n", +i); break;

      typecase(Identifier);
      typecase(Variable);
      typecase(Function);
      typecase(Type);
      typecase(VariadicParameter);
      typecase(FundamentalType);
      typecase(FunctionSignature);
      typecase(Enum);
      typecase(Mask);
      typecase(Union);
      typecase(Namespace);
      typecase(Interface);
      typecase(Struct);
      typecase(Class);
      typecase(GenericType);
      typecase(Scope);
      typecase(Literal);
      typecase(Expression);
      typecase(Declaration);
      typecase(ForLoop);
      typecase(ForeachLoop);
      typecase(WhileLoop);
      typecase(DoWhileLoop);
      typecase(If);
      typecase(Switch);
      typecase(Match);
      typecase(TryCatch);
      typecase(Asm);
      typecase(ForLoopParams);
      typecase(ForeachLoopParams);
      typecase(SwitchCases);
      typecase(MatchCases);
      typecase(Statement);
      typecase(StatementSequence);
      typecase(ArgumentList);
      typecase(ParameterList);
      typecase(SpecializerList);
   }
}
#undef typecase

#endif //SYNTAX_TREE_CPP