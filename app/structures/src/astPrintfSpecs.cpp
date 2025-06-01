#ifndef CLEF_AST_PRINTF_SPECS_CPP
#define CLEF_AST_PRINTF_SPECS_CPP

#include "SyntaxTree.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"
#include "io.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define TNB(expr) clef::astTNB{obj.tree, expr, obj.indents}
#define TNB_INDENT(expr) clef::astTNB{obj.tree, expr, obj.indents + 1}
#define TNB_AST(expr) TNB((clef::index<const clef::astNode>)expr)
#define TNB_CAST(T) clef::astTNB<clef::T>(obj.tree, +obj.i, obj.indents)
#define TNB_CAST2(T, expr) clef::astTNB<clef::T>(obj.tree, expr, obj.indents)
#define TNB_CAST_INDENT(T) TNB_INDENT(clef::index<const clef::T>(obj.i))

//!TODO: probably shouldn't just return 0 without doing anything when printing with `%b`

#define PRINT_ARR_S(arr, before)\
for (auto elem : spec.arr()) {                                                   \
   charsPrinted += file.printf(FMT("%S" before "%s;"), obj.indents, TNB(elem));  \
}
#define PRINT_ARR_B(arr)\
charsPrinted += file.printf(FMT("%b"), spec.arr().size());  \
for (auto elem : spec.arr()) {                              \
   charsPrinted += file.printf(FMT("%b"), TNB(elem));       \
}

uint mcsl::writef(File& file, const clef::astTNB<clef::ObjTypeSpec> obj, char mode, FmtArgs args) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ObjTypeSpec& spec = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      // PRINT_ARR_S(inheritedTypes, "") //handled by caller
      PRINT_ARR_S(friendTypes, "friend type ")
      PRINT_ARR_S(memberTypes,)
      PRINT_ARR_S(staticVars, "static ")
      PRINT_ARR_S(members,)
      PRINT_ARR_S(staticFuncs, "static ")
      PRINT_ARR_S(methods,)
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      uint charsPrinted = 0;
      PRINT_ARR_B(inheritedTypes)
      PRINT_ARR_B(staticVars)
      PRINT_ARR_B(staticFuncs)
      PRINT_ARR_B(memberTypes)
      PRINT_ARR_B(members)
      PRINT_ARR_B(methods)
      PRINT_ARR_B(friendTypes)
      return charsPrinted;
   }
   else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ObjTypeSpec>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(File& file, const clef::astTNB<clef::InterfaceSpec> obj, char mode, FmtArgs args) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const InterfaceSpec& spec = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      // PRINT_ARR_S(inheritedInterfaces, "") //handled by caller
      PRINT_ARR_S(staticFuncs, "static ")
      PRINT_ARR_S(methods,)
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      uint charsPrinted = 0;
      PRINT_ARR_B(inheritedInterfaces)
      PRINT_ARR_B(staticFuncs)
      PRINT_ARR_B(methods)
      return charsPrinted;
   }
   else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ObjTypeSpec>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(File& file, const clef::astTNB<clef::NamespaceSpec> obj, char mode, FmtArgs args) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const NamespaceSpec& spec = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      PRINT_ARR_S(types,)
      PRINT_ARR_S(vars,)
      PRINT_ARR_S(funcs,)
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      uint charsPrinted = 0;
      PRINT_ARR_B(vars)
      PRINT_ARR_B(funcs)
      PRINT_ARR_B(types)
      return charsPrinted;
   }
   else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ObjTypeSpec>"), mode);
   }
   UNREACHABLE;
}

#undef PRINT_ARR_B
#undef PRINT_ARR_S

#undef TNB_CAST_INDENT
#undef TNB_CAST2
#undef TNB_CAST
#undef TNB_AST
#undef TNB_INDENT
#undef TNB

#pragma GCC diagnostic pop

#endif //CLEF_AST_PRINTF_SPECS_CPP