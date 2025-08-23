#ifndef EXPR_HPP
#define EXPR_HPP

#include "CLEF.hpp"
#include "Literal.hpp"
#include "Proc.hpp"
#include "Attr.hpp"

struct clef::Identifier {
   using SyntaxNode_t = void;
   Symbol* symbol = nullptr;
   Args* gens = nullptr;

   constexpr explicit operator bool() const { return symbol; }
};
struct clef::Label {
   using SyntaxNode_t = void;
   const mcsl::str_slice name;

   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
   Label& operator=(const Label& other) { return *new (this) Label(other); }
   #pragma GCC diagnostic pop
};
class clef::Args : public mcsl::dyn_arr<Expr*> {
   using SyntaxNode_t = void;
};
struct clef::Ternary {
   using SyntaxNode_t = void;
   Expr* cond;
   Expr* trueVal;
   Expr* falseVal;
};
struct clef::If {
   using SyntaxNode_t = void;
   Expr* cond;
   Proc* proc;
   Expr* elseExpr = nullptr;
   Label name = {};
   Identifier retType = {};
};
struct clef::While {
   using SyntaxNode_t = void;
   Expr* cond;
   Proc* proc;
   Proc* elseProc = nullptr;
   Label name = {};
   Identifier retType = {};
};
struct clef::DoWhile {
   using SyntaxNode_t = void;
   Expr* cond;
   Proc* proc;
   Proc* elseProc = nullptr;
   Label name = {};
   Identifier retType = {};
};
struct clef::For {
   using SyntaxNode_t = void;
   Expr* decl;
   Expr* cond;
   Expr* inc;
   Proc* proc;
   Proc* elseProc = nullptr;
   Label name = {};
   Identifier retType = {};
};
struct clef::Foreach {
   using SyntaxNode_t = void;
   Expr* it;
   Expr* container;
   Proc* proc;
   Proc* elseProc = nullptr;
   Label name = {};
   Identifier retType = {};
};
struct clef::Switch {
   using SyntaxNode_t = void;
   struct Case { Expr* val; Expr* firstStmt; };
   Expr* cond;
   Proc* proc;
   mcsl::dyn_arr<Case> cases;
   Label name = {};
   Identifier retType = {};
};
struct clef::Match {
   using SyntaxNode_t = void;
   struct Case { Expr* val; Proc* proc; };
   Expr* cond;
   mcsl::dyn_arr<Case> cases;
   Label name = {};
   Identifier retType = {};
};

struct clef::Expr {
   public:
      using SyntaxNode_t = void;
      enum Type : uint8 {
         null,

         EXPR,
         FUNC,
         MACRO,
         LIT,
         IDEN,
         DECL,
         TYPE_DECL,
         LABEL,
         ARGS,
         TERNARY,
         IF,
         WHILE,
         DO_WHILE,
         FOR,
         FOREACH,
         SWITCH,
         MATCH,
      };
   private:
      union M {
         struct {
            Expr* lhs;
            Expr* rhs;
            OpID op;
         } expr;
         struct {
            Func* fn;
            Overload* ov;
         } fn;
         struct {
            Macro* fn;
            Overload* ov;
         } macro;
         Literal lit;
         Identifier iden;
         struct {
            Identifier iden; //Symbol::isType(name.symbol->symbolType()) → anonymous parameter
            Expr* val;
         } decl;
         struct {
            Identifier iden;
         } typeDecl;
         Label label;
         Args* args;
         Ternary* ternary;
         If* ifExpr;
         While* whileExpr;
         DoWhile* doWhileExpr;
         For* forExpr;
         Foreach* foreachExpr;
         Switch* switchExpr;
         Match* matchExpr;
      } m = {.decl = {.iden = {}, .val = {}}}; static_assert(sizeof(m.decl) == sizeof(m));
      Attr* _attrs;
      Type _type;
      Expr(Identifier typeName, Attr* attrs, std::in_place_t):
         m{},_attrs{attrs},_type{TYPE_DECL} {
            m.typeDecl = {.iden = typeName};
      }
   public:
      Expr(const Expr& other) { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Expr)); }
      Expr(Attr* attrs = nullptr):
         m{},_attrs{attrs},_type{null} {
            debug_assert(!((uoverlong*)this)[0]);
      }
      Expr(Expr* lhs, Expr* rhs, OpID op, Attr* attrs = nullptr):
         m{},_attrs{attrs},_type{EXPR} {
            m.expr = {.lhs = lhs, .rhs = rhs, .op = op};
      }
      Expr(Func* fn, Overload* ov, Attr* attrs = nullptr):
         m{},_attrs{attrs},_type{FUNC} {
            m.fn = {.fn = fn, .ov = ov};
      }
      Expr(mcsl::pair<Func*, Overload*> fn, Attr* attrs = nullptr): Expr(fn.first, fn.second, attrs) {}
      Expr(Macro* fn, Overload* ov, Attr* attrs = nullptr):
         m{},_attrs{attrs},_type{MACRO} {
            m.macro = {.fn = fn, .ov = ov};
      }
      Expr(mcsl::pair<Macro*, Overload*> fn, Attr* attrs = nullptr): Expr(fn.first, fn.second, attrs) {}
      Expr(Identifier iden, Expr* val, Attr* attrs = nullptr):
         m{},_attrs{attrs},_type{DECL} {
            m.decl = {.iden = iden, .val = val};
      }
      #define DEF_CTOR(T, name, exprType) \
         Expr(T name, Attr* attrs = nullptr):   \
            m{},_attrs{attrs},_type{exprType} { \
               m.name = name;                   \
         }
      DEF_CTOR(Literal, lit, LIT)
      DEF_CTOR(Identifier, iden, IDEN)
      DEF_CTOR(Label, label, LABEL)
      DEF_CTOR(Args*, args, ARGS)
      DEF_CTOR(Ternary*, ternary, TERNARY)
      DEF_CTOR(If*, ifExpr, IF)
      DEF_CTOR(While*, whileExpr, WHILE)
      DEF_CTOR(DoWhile*, doWhileExpr, DO_WHILE)
      DEF_CTOR(For*, forExpr, FOR)
      DEF_CTOR(Foreach*, foreachExpr, FOREACH)
      DEF_CTOR(Switch*, switchExpr, SWITCH)
      DEF_CTOR(Match*, matchExpr, MATCH)
      #undef DEF_CTOR
      static Expr makeTypeDecl(Identifier typeName, Attr* attrs = nullptr) {
         return Expr(typeName, attrs, std::in_place);
      }

      Type type() const { return _type; }
      Attr* attrs() { return _attrs; }
      const Attr* attrs() const { return _attrs; }

      #define DEF_GET(name) \
         auto& name() { return m.name; } \
         const auto& name() const { return m.name; }
      DEF_GET(expr)
      DEF_GET(lit)
      DEF_GET(iden)
      DEF_GET(label)
      DEF_GET(decl)
      DEF_GET(typeDecl)
      #undef DEF_GET
      #define DEF_GET(name) \
         auto& name() { return *m.name; } \
         const auto& name() const { return *m.name; }
      DEF_GET(args)
      DEF_GET(ternary)
      DEF_GET(ifExpr)
      DEF_GET(whileExpr)
      DEF_GET(doWhileExpr)
      DEF_GET(forExpr)
      DEF_GET(foreachExpr)
      DEF_GET(switchExpr)
      DEF_GET(matchExpr)
      #undef DEF_GET
};

#include "io.hpp"
namespace mcsl {
   #define DEF_WRITE(Type) uint writef(File&, const clef::Type&, char, FmtArgs)
   DEF_WRITE(Expr);
   DEF_WRITE(Literal);
   DEF_WRITE(Identifier);
   DEF_WRITE(Label);
   DEF_WRITE(Args);
   DEF_WRITE(Ternary);
   DEF_WRITE(If);
   DEF_WRITE(While);
   DEF_WRITE(DoWhile);
   DEF_WRITE(For);
   DEF_WRITE(Foreach);
   DEF_WRITE(Switch);
   DEF_WRITE(Match);
   #undef DEF_WRITE
};

#endif