/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.ypp"

	int yylex();
	void yyerror(const char* s);
	#include "stdio.h"
	#include "Symtab.hpp"
	#include "AuxTypes.hpp" 
	#include "hw3_output.hpp"
	#include "bp.hpp"
	#include "assert.h"
	#include <iostream>
	#include <algorithm>
	#include <set>
	extern int yylineno;

	//#define MYDB
	#ifdef MYDB
		#define YYERROR_VERBOSE 1
		#define YYDEBUG 1
		extern int yydebug;
	#endif

	SimpleSymtab symtab;
	using namespace std;

	#define check(assertion, error) do{if(!(assertion)){error; exit(1);}}while(false);

	static int loop_depth;
	
	bool isNumeralType(ExpType type){
		return type == INT_EXP || type == BYTE_EXP;
	}
	bool canImplicitCast(ExpType source_type, ExpType dst_type){
		return (source_type == dst_type)
			|| (source_type == BYTE_EXP && dst_type == INT_EXP);
	}
	bool canExplicitCast(ExpType source_type, ExpType dst_type){
		if(source_type == dst_type)
			return true;
		return isNumeralType(source_type) && isNumeralType(dst_type);
	}

	void checkMismatch(ExpType source_type, ExpType dst_type){
		check(canImplicitCast(source_type, dst_type), output::errorMismatch(yylineno));
	}
	
	void checkBool(ExpType type) {
		checkMismatch(type, BOOL_EXP);
	}

	void checkNumeralType(ExpType type){
		check(isNumeralType(type), output::errorMismatch(yylineno));
	}

	ExpType maxNumeralType(ExpType first_operand_type, ExpType second_operand_type){
		return first_operand_type == INT_EXP ? INT_EXP : second_operand_type;
	}

	void checkFuncDec(const string& func_id, const std::vector<Parameter>& parameters){
		std::set<std::string> tmp_params_set;
		for(auto it = parameters.rbegin(); it != parameters.rend(); ++it){
			auto param = *it;
			//check that the new parameter identifiers don't conflict with the id of the new function:
			check(func_id != param.id, output::errorDef(param.line_of_origin, param.id));
			//check that the new parameter identifiers dont confilict with existing ones: 
			check(symtab.declarableValidId(param.id), output::errorDef(param.line_of_origin, param.id));
			//check that there are not conflitcts between the new identifiers:
			check(tmp_params_set.count(param.id) == 0, output::errorDef(param.line_of_origin, param.id));
			tmp_params_set.insert(param.id);
		}
	}

	void checkPrototypeMismatch(const string& func_id, const std::vector<Expression*>& reverse_exp_list_in_call){
		std::vector<ExpType> exp_types_required = symtab.getFunctionType(func_id).getParameterTypes();
		//TODO: add something to remember the line of each expression so we can add the correct 
		// 		line of the expression to this error (something like 'line_of_origin').
		auto required_types_it = exp_types_required.begin();
		for(auto called_exp_it = reverse_exp_list_in_call.rbegin();
				called_exp_it != reverse_exp_list_in_call.rend(); ++called_exp_it){
			
			ExpType called_type = (*called_exp_it)->type;
			if(required_types_it == exp_types_required.end() || !canImplicitCast(called_type, *required_types_it)){
				output::errorPrototypeMismatch(yylineno, func_id, ExpTypeStringVector(exp_types_required, true));
				exit(1);
			}
			++required_types_it;
		}
		if(required_types_it != exp_types_required.end()){
				output::errorPrototypeMismatch(yylineno, func_id, ExpTypeStringVector(exp_types_required, true));
				exit(1);			
		}
	}
	void checkMainMissing(){
		if (symtab.callableValidId("main")) {
			const FunctionType& func = symtab.getFunctionType("main");
			if (func.return_type != VOID_EXP || !func.getParameterTypes().empty()){
				output::errorMainMissing();
				exit(1);
			} 
		} else {
			output::errorMainMissing();
			exit(1);
		}
	}

	void checkByteTooLarge(int b) {
		if (b > 255 || b < 0) {
			output::errorByteTooLarge(yylineno, to_string(b));
			exit(1);
		}
	}

	bool isDigit(char c){
		return '0' <= c && c <= '9';
	}

	bool constValueIsNumLiteral(const string& literal){
		for(char c: literal){
			if(!isDigit(c))
				return false;
		}
		return true;
	}

	int cur_parsed_func_start_label_offset;
	CodeBuffer& cb = CodeBuffer::instance();

#line 198 "parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INT = 3,                        /* INT  */
  YYSYMBOL_VOID = 4,                       /* VOID  */
  YYSYMBOL_BYTE = 5,                       /* BYTE  */
  YYSYMBOL_B = 6,                          /* B  */
  YYSYMBOL_BOOL = 7,                       /* BOOL  */
  YYSYMBOL_CONST = 8,                      /* CONST  */
  YYSYMBOL_TRUE = 9,                       /* TRUE  */
  YYSYMBOL_FALSE = 10,                     /* FALSE  */
  YYSYMBOL_RETURN = 11,                    /* RETURN  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_BREAK = 13,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 14,                  /* CONTINUE  */
  YYSYMBOL_SC = 15,                        /* SC  */
  YYSYMBOL_STRING = 16,                    /* STRING  */
  YYSYMBOL_COMMA = 17,                     /* COMMA  */
  YYSYMBOL_ID = 18,                        /* ID  */
  YYSYMBOL_NUM = 19,                       /* NUM  */
  YYSYMBOL_ASSIGN = 20,                    /* ASSIGN  */
  YYSYMBOL_OR = 21,                        /* OR  */
  YYSYMBOL_AND = 22,                       /* AND  */
  YYSYMBOL_RELOP = 23,                     /* RELOP  */
  YYSYMBOL_BINOP = 24,                     /* BINOP  */
  YYSYMBOL_NOT = 25,                       /* NOT  */
  YYSYMBOL_LPAREN = 26,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 27,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 28,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 29,                    /* RBRACE  */
  YYSYMBOL_IF = 30,                        /* IF  */
  YYSYMBOL_ELSE = 31,                      /* ELSE  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_Program = 33,                   /* Program  */
  YYSYMBOL_OpenScope = 34,                 /* OpenScope  */
  YYSYMBOL_CloseScope = 35,                /* CloseScope  */
  YYSYMBOL_OpenLoop = 36,                  /* OpenLoop  */
  YYSYMBOL_CloseLoop = 37,                 /* CloseLoop  */
  YYSYMBOL_Block = 38,                     /* Block  */
  YYSYMBOL_Funcs = 39,                     /* Funcs  */
  YYSYMBOL_FuncDecl = 40,                  /* FuncDecl  */
  YYSYMBOL_41_1 = 41,                      /* $@1  */
  YYSYMBOL_42_2 = 42,                      /* $@2  */
  YYSYMBOL_RetType = 43,                   /* RetType  */
  YYSYMBOL_Formals = 44,                   /* Formals  */
  YYSYMBOL_FormalsList = 45,               /* FormalsList  */
  YYSYMBOL_LineCapture = 46,               /* LineCapture  */
  YYSYMBOL_Statements = 47,                /* Statements  */
  YYSYMBOL_Call = 48,                      /* Call  */
  YYSYMBOL_ExpList = 49,                   /* ExpList  */
  YYSYMBOL_InvocationExp = 50,             /* InvocationExp  */
  YYSYMBOL_Type = 51,                      /* Type  */
  YYSYMBOL_TypeAnnotation = 52,            /* TypeAnnotation  */
  YYSYMBOL_Exp = 53,                       /* Exp  */
  YYSYMBOL_NumericExp = 54,                /* NumericExp  */
  YYSYMBOL_Label = 55,                     /* Label  */
  YYSYMBOL_CondLabel = 56,                 /* CondLabel  */
  YYSYMBOL_StatementLabel = 57,            /* StatementLabel  */
  YYSYMBOL_BoolExp = 58,                   /* BoolExp  */
  YYSYMBOL_Statement = 59,                 /* Statement  */
  YYSYMBOL_OpenStatment = 60,              /* OpenStatment  */
  YYSYMBOL_ClosedStatment = 61,            /* ClosedStatment  */
  YYSYMBOL_IfStart = 62,                   /* IfStart  */
  YYSYMBOL_63_3 = 63,                      /* $@3  */
  YYSYMBOL_WhileStart = 64,                /* WhileStart  */
  YYSYMBOL_65_4 = 65,                      /* $@4  */
  YYSYMBOL_SimpleStatement = 66,           /* SimpleStatement  */
  YYSYMBOL_67_5 = 67,                      /* $@5  */
  YYSYMBOL_68_6 = 68,                      /* $@6  */
  YYSYMBOL_VarDecStart = 69                /* VarDecStart  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   148

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  133

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   189,   189,   191,   192,   193,   194,   196,   201,   202,
     205,   206,   205,   228,   229,   231,   232,   234,   235,   237,
     239,   240,   247,   253,   262,   265,   268,   280,   281,   282,
     284,   285,   287,   288,   289,   301,   302,   313,   314,   317,
     336,   337,   342,   343,   344,   346,   359,   372,   394,   403,
     408,   415,   416,   420,   426,   433,   442,   443,   450,   458,
     458,   463,   463,   469,   470,   477,   500,   511,   514,   514,
     519,   519,   546,   550,   555
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INT", "VOID", "BYTE",
  "B", "BOOL", "CONST", "TRUE", "FALSE", "RETURN", "WHILE", "BREAK",
  "CONTINUE", "SC", "STRING", "COMMA", "ID", "NUM", "ASSIGN", "OR", "AND",
  "RELOP", "BINOP", "NOT", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "IF",
  "ELSE", "$accept", "Program", "OpenScope", "CloseScope", "OpenLoop",
  "CloseLoop", "Block", "Funcs", "FuncDecl", "$@1", "$@2", "RetType",
  "Formals", "FormalsList", "LineCapture", "Statements", "Call", "ExpList",
  "InvocationExp", "Type", "TypeAnnotation", "Exp", "NumericExp", "Label",
  "CondLabel", "StatementLabel", "BoolExp", "Statement", "OpenStatment",
  "ClosedStatment", "IfStart", "$@3", "WhileStart", "$@4",
  "SimpleStatement", "$@5", "$@6", "VarDecStart", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
#endif

#define YYPACT_NINF (-76)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-20)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     110,   -76,   -76,   -76,   -76,    16,   -76,   110,     1,   -76,
     -76,   -76,   -76,    12,     4,   -76,   -76,   -76,   113,    20,
      33,    25,    39,    30,    45,    41,   -76,    48,   -76,    11,
      94,   -76,   -76,   -76,   -76,   -76,   -76,    57,   -76,    30,
     -76,   -76,   -76,    85,    63,    71,    -2,    91,   113,     5,
      30,   -76,   -76,    85,    59,    85,   -76,   -76,   -76,    55,
     124,    85,    54,   -76,   100,   -76,   -76,   104,   -76,   -76,
      85,    66,   -76,    82,   -76,    85,   -76,    78,    30,   100,
     -76,   100,   -76,   -76,   105,   -13,   -76,   -76,    85,    85,
     116,   -76,    22,   -76,   106,   117,   100,   -76,    75,   -76,
     107,   -76,   -76,   108,   -76,   109,    85,   -76,    85,    85,
     115,   -76,   -76,   -76,   -76,    85,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   103,    10,   -76,    30,   -76,   -76,   -76,
     -76,   -76,   -76
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       9,    27,    14,    28,    29,     0,     2,     9,     0,    13,
       1,     8,    10,     0,    31,    30,    11,    16,     0,     0,
       0,     0,    17,    44,     0,     0,     3,     0,    63,    44,
      31,    20,    51,    52,     3,     5,    56,    31,    43,    44,
      42,    12,    21,    68,     0,     0,     0,     0,     0,     0,
      44,     3,    18,     0,    44,     0,    49,    50,    35,    34,
      40,     0,     0,    33,    70,    37,    38,     0,    72,    73,
       0,     0,    67,     0,    64,     0,     4,    52,    44,    61,
       4,    59,    41,    48,     0,     0,    42,    42,     0,     0,
       0,    69,     0,    23,     0,    24,    26,    74,     0,    53,
       0,     4,     4,     0,     7,     0,     0,    32,     0,     0,
      47,    39,    71,    66,    22,     0,    65,     3,     6,     6,
      62,    60,    36,    46,    45,    25,    44,    55,    58,     4,
       4,    54,    57
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -76,   -76,   -34,   -75,   -76,    18,   -76,   133,   -76,   -76,
     -76,   -76,   -76,   111,   -76,   102,   112,    28,   -76,   -12,
     114,   -40,   -76,    42,   -76,   -76,   -76,   -22,   -74,   -49,
     -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     5,    39,    99,    51,   127,    28,     6,     7,    13,
      19,     8,    16,    17,    24,    29,    63,    94,    95,     9,
      18,    96,    65,    55,    53,    30,    66,    31,    32,    33,
      34,   105,    35,   103,    36,    67,    90,    49
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      50,    77,   100,    64,   101,   104,    20,    42,    86,    87,
      88,    89,    15,    79,   107,    81,    10,    78,    70,    12,
      74,    83,    85,    25,    71,    75,   118,   119,    76,   102,
      92,   -15,    42,    88,    89,    98,    73,   113,    14,    26,
      41,    27,    25,    86,    87,    88,    89,    21,   110,   111,
      84,    22,   129,    23,   131,   132,   -19,     1,    26,     3,
      27,     4,    37,    56,    57,    15,   122,    38,   123,   124,
      58,    25,    59,    60,    40,    56,    57,   130,    68,    61,
      62,    71,    58,   126,    59,    60,    69,    26,    80,    27,
     116,    61,    62,    93,    56,    57,    86,    87,    88,    89,
      97,    58,    15,    59,    60,    43,    72,    44,    45,    -4,
      61,    62,    46,     1,     2,     3,     1,     4,     3,    91,
       4,    86,    87,    88,    89,    87,    88,    89,   108,   109,
      82,   112,   106,   114,   115,   120,   121,   128,   117,    89,
      11,    54,    47,   125,    48,     0,     0,     0,    52
};

static const yytype_int16 yycheck[] =
{
      34,    50,    77,    43,    78,    80,    18,    29,    21,    22,
      23,    24,     8,    53,    27,    55,     0,    51,    20,    18,
      15,    61,    62,    12,    26,    20,   101,   102,    50,    78,
      70,    27,    54,    23,    24,    75,    48,    15,    26,    28,
      29,    30,    12,    21,    22,    23,    24,    27,    88,    89,
      62,    18,   126,    28,   129,   130,    17,     3,    28,     5,
      30,     7,    17,     9,    10,     8,   106,    26,   108,   109,
      16,    12,    18,    19,    26,     9,    10,   126,    15,    25,
      26,    26,    16,   117,    18,    19,    15,    28,    29,    30,
      15,    25,    26,    27,     9,    10,    21,    22,    23,    24,
      18,    16,     8,    18,    19,    11,    15,    13,    14,    31,
      25,    26,    18,     3,     4,     5,     3,     7,     5,    15,
       7,    21,    22,    23,    24,    22,    23,    24,    86,    87,
       6,    15,    27,    27,    17,    27,    27,   119,    31,    24,
       7,    39,    30,   115,    30,    -1,    -1,    -1,    37
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     7,    33,    39,    40,    43,    51,
       0,    39,    18,    41,    26,     8,    44,    45,    52,    42,
      51,    27,    18,    28,    46,    12,    28,    30,    38,    47,
      57,    59,    60,    61,    62,    64,    66,    17,    26,    34,
      26,    29,    59,    11,    13,    14,    18,    48,    52,    69,
      34,    36,    45,    56,    47,    55,     9,    10,    16,    18,
      19,    25,    26,    48,    53,    54,    58,    67,    15,    15,
      20,    26,    15,    51,    15,    20,    59,    61,    34,    53,
      29,    53,     6,    53,    51,    53,    21,    22,    23,    24,
      68,    15,    53,    27,    49,    50,    53,    18,    53,    35,
      35,    60,    61,    65,    35,    63,    27,    27,    55,    55,
      53,    53,    15,    15,    27,    17,    15,    31,    35,    35,
      27,    27,    53,    53,    53,    49,    34,    37,    37,    60,
      61,    35,    35
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    35,    36,    37,    38,    39,    39,
      41,    42,    40,    43,    43,    44,    44,    45,    45,    46,
      47,    47,    48,    48,    49,    49,    50,    51,    51,    51,
      52,    52,    53,    53,    53,    53,    53,    53,    53,    54,
      54,    54,    55,    56,    57,    58,    58,    58,    58,    58,
      58,    59,    59,    60,    60,    60,    61,    61,    61,    63,
      62,    65,    64,    66,    66,    66,    66,    66,    67,    66,
      68,    66,    66,    66,    69
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     0,     0,     0,     5,     2,     0,
       0,     0,    10,     1,     1,     0,     1,     3,     6,     0,
       1,     2,     4,     3,     1,     3,     1,     1,     1,     1,
       1,     0,     3,     1,     1,     1,     4,     1,     1,     3,
       1,     2,     0,     0,     0,     4,     4,     3,     2,     1,
       1,     1,     1,     4,     8,     6,     1,     8,     6,     0,
       6,     0,     6,     1,     3,     5,     5,     3,     0,     4,
       0,     5,     3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* OpenScope: %empty  */
#line 191 "parser.ypp"
                                {symtab.pushScope();}
#line 1356 "parser.tab.cpp"
    break;

  case 4: /* CloseScope: %empty  */
#line 192 "parser.ypp"
                                {symtab.popScope();}
#line 1362 "parser.tab.cpp"
    break;

  case 5: /* OpenLoop: %empty  */
#line 193 "parser.ypp"
                                {++loop_depth;}
#line 1368 "parser.tab.cpp"
    break;

  case 6: /* CloseLoop: %empty  */
#line 194 "parser.ypp"
                                {--loop_depth;}
#line 1374 "parser.tab.cpp"
    break;

  case 7: /* Block: LBRACE OpenScope Statements RBRACE CloseScope  */
#line 196 "parser.ypp"
                                                                              {
						(yyval.run_block) = (yyvsp[-2].run_block);
					}
#line 1382 "parser.tab.cpp"
    break;

  case 10: /* $@1: %empty  */
#line 205 "parser.ypp"
                                           {check(symtab.declarableValidId(*(yyvsp[0].id)), output::errorDef(yylineno, *(yyvsp[0].id)));}
#line 1388 "parser.tab.cpp"
    break;

  case 11: /* $@2: %empty  */
#line 206 "parser.ypp"
                                                       {	
						checkFuncDec(*(yyvsp[-3].id), *(yyvsp[0].formals_list));
						symtab.declareFunc(*(yyvsp[-3].id), (yyvsp[-4].exp_type), (yyvsp[0].formals_list));
						cb.emitFuncDecl(*(yyvsp[-3].id));
						cb.emit("%sp = alloca [50 x i32]");
						cur_parsed_func_start_label_offset = cb.emit("br label @");
						//std::vector<Backpatch> bpv = cb.makelist(Backpatch(cb.emit("br label @"), FIRST));
					}
#line 1401 "parser.tab.cpp"
    break;

  case 12: /* FuncDecl: RetType ID $@1 LPAREN Formals $@2 RPAREN LBRACE Statements RBRACE  */
#line 213 "parser.ypp"
                                                                          {
						symtab.finishFunc();
						cb.bpatch(cb.makelist(Backpatch(cur_parsed_func_start_label_offset, FIRST)), (yyvsp[-1].run_block)->start_label);
						//TODO: add support for users not having to put 'return' at the end of his functions.
						
						Backpatch func_end_bp(cb.emit("br label @"), FIRST);
						std::string func_end_label = cb.genLabel("func_end");
						cb.bpatch((yyvsp[-1].run_block)->nextlist, func_end_label);
						cb.bpatch(cb.makelist(func_end_bp), func_end_label);
						cb.emit("ret "+cb.IrDefaultTypedValue((yyvsp[-9].exp_type)));
						
						cb.emit("}");
						delete (yyvsp[-1].run_block);
					}
#line 1420 "parser.tab.cpp"
    break;

  case 13: /* RetType: Type  */
#line 228 "parser.ypp"
                                     {(yyval.exp_type) = (yyvsp[0].exp_type);}
#line 1426 "parser.tab.cpp"
    break;

  case 14: /* RetType: VOID  */
#line 229 "parser.ypp"
                                              {(yyval.exp_type) = VOID_EXP;}
#line 1432 "parser.tab.cpp"
    break;

  case 15: /* Formals: %empty  */
#line 231 "parser.ypp"
                                {(yyval.formals_list) = new std::vector<Parameter>();}
#line 1438 "parser.tab.cpp"
    break;

  case 16: /* Formals: FormalsList  */
#line 232 "parser.ypp"
                                                     {(yyval.formals_list) = (yyvsp[0].formals_list);}
#line 1444 "parser.tab.cpp"
    break;

  case 17: /* FormalsList: TypeAnnotation Type ID  */
#line 234 "parser.ypp"
                                               {(yyval.formals_list) = new std::vector<Parameter>(); (yyval.formals_list)->push_back(Parameter(*(yyvsp[0].id), (yyvsp[-1].exp_type), yylineno, (yyvsp[-2].is_const))); delete (yyvsp[0].id);}
#line 1450 "parser.tab.cpp"
    break;

  case 18: /* FormalsList: TypeAnnotation Type ID LineCapture COMMA FormalsList  */
#line 235 "parser.ypp"
                                                                                              {(yyval.formals_list) = (yyvsp[0].formals_list); (yyval.formals_list)->push_back(Parameter(*(yyvsp[-3].id), (yyvsp[-4].exp_type), (yyvsp[-2].line_number), (yyvsp[-5].is_const))); delete (yyvsp[-3].id);}
#line 1456 "parser.tab.cpp"
    break;

  case 19: /* LineCapture: %empty  */
#line 237 "parser.ypp"
                        {(yyval.line_number) = yylineno;}
#line 1462 "parser.tab.cpp"
    break;

  case 20: /* Statements: Statement  */
#line 239 "parser.ypp"
                                          {(yyval.run_block) = (yyvsp[0].run_block);}
#line 1468 "parser.tab.cpp"
    break;

  case 21: /* Statements: Statements Statement  */
#line 240 "parser.ypp"
                                                              {
						cb.bpatch((yyvsp[-1].run_block)->nextlist, (yyvsp[0].run_block)->start_label);
						(yyval.run_block) = new RunBlock((yyvsp[-1].run_block)->start_label);
						(yyval.run_block)->nextlist = (yyvsp[0].run_block)->nextlist;
						delete (yyvsp[0].run_block);
					}
#line 1479 "parser.tab.cpp"
    break;

  case 22: /* Call: ID LPAREN ExpList RPAREN  */
#line 247 "parser.ypp"
                                                         {
						check(symtab.callableValidId(*(yyvsp[-3].id)), output::errorUndefFunc(yylineno, *(yyvsp[-3].id)));
						checkPrototypeMismatch(*(yyvsp[-3].id), *(yyvsp[-1].exp_list));
						(yyval.expression) = cb.emitFunctionCall(*(yyvsp[-3].id), *(yyvsp[-1].exp_list));
						delete (yyvsp[-3].id);
					}
#line 1490 "parser.tab.cpp"
    break;

  case 23: /* Call: ID LPAREN RPAREN  */
#line 253 "parser.ypp"
                                                          {
						check(symtab.callableValidId(*(yyvsp[-2].id)), output::errorUndefFunc(yylineno, *(yyvsp[-2].id)));
						//there are no arguments used in the call so std::vector is empty:
						std::vector<Expression*> call_arg_types = {};
						checkPrototypeMismatch(*(yyvsp[-2].id), call_arg_types);
						(yyval.expression) = cb.emitFunctionCall(*(yyvsp[-2].id), std::vector<Expression*>());
						delete (yyvsp[-2].id);	
					}
#line 1503 "parser.tab.cpp"
    break;

  case 24: /* ExpList: InvocationExp  */
#line 262 "parser.ypp"
                                              {
						(yyval.exp_list) = new std::vector<Expression*>(); (yyval.exp_list)->push_back((yyvsp[0].expression));
					}
#line 1511 "parser.tab.cpp"
    break;

  case 25: /* ExpList: InvocationExp COMMA ExpList  */
#line 265 "parser.ypp"
                                                                     {(yyval.exp_list) = (yyvsp[0].exp_list); (yyval.exp_list)->push_back((yyvsp[-2].expression));}
#line 1517 "parser.tab.cpp"
    break;

  case 26: /* InvocationExp: Exp  */
#line 268 "parser.ypp"
                            {
						if((yyvsp[0].expression)->type == BOOL_EXP){
							BoolExp* bool_exp = dynamic_cast<BoolExp*>((yyvsp[0].expression));
							assert(bool_exp);
							(yyval.expression) = new RegStoredExp(BOOL_EXP, "add i32 0, "+bool_exp->storeAsRawReg());
							delete (yyvsp[0].expression);
						} else {
							(yyval.expression) = (yyvsp[0].expression);
						}
					}
#line 1532 "parser.tab.cpp"
    break;

  case 27: /* Type: INT  */
#line 280 "parser.ypp"
                                    {(yyval.exp_type) = INT_EXP;}
#line 1538 "parser.tab.cpp"
    break;

  case 28: /* Type: BYTE  */
#line 281 "parser.ypp"
                                              {(yyval.exp_type) = BYTE_EXP;}
#line 1544 "parser.tab.cpp"
    break;

  case 29: /* Type: BOOL  */
#line 282 "parser.ypp"
                                              {(yyval.exp_type) = BOOL_EXP;}
#line 1550 "parser.tab.cpp"
    break;

  case 30: /* TypeAnnotation: CONST  */
#line 284 "parser.ypp"
                              {(yyval.is_const) = true;}
#line 1556 "parser.tab.cpp"
    break;

  case 31: /* TypeAnnotation: %empty  */
#line 285 "parser.ypp"
                                          {(yyval.is_const) = false;}
#line 1562 "parser.tab.cpp"
    break;

  case 32: /* Exp: LPAREN Exp RPAREN  */
#line 287 "parser.ypp"
                                                  {(yyval.expression) = (yyvsp[-1].expression);}
#line 1568 "parser.tab.cpp"
    break;

  case 33: /* Exp: Call  */
#line 288 "parser.ypp"
                                               {(yyval.expression) = (yyvsp[0].expression);}
#line 1574 "parser.tab.cpp"
    break;

  case 34: /* Exp: ID  */
#line 289 "parser.ypp"
                                             {
						const string& id = *(yyvsp[0].id);
						check(symtab.rvalValidId(id), output::errorUndef(yylineno, id));
						if(symtab.isConst(id) && constValueIsNumLiteral(symtab.getConstValue(id))){
							//get the constant value from the symtable and set it to the value of the expression:
							(yyval.expression) = new NumericExp(symtab.getVariableType(id), symtab.getConstValue(id), false);
						} else {
							//load value from stack:
							(yyval.expression) = cb.emitLoadVar(id);
						}
						delete (yyvsp[0].id);
					}
#line 1591 "parser.tab.cpp"
    break;

  case 35: /* Exp: STRING  */
#line 301 "parser.ypp"
                                                 {(yyval.expression) = new StrExp();}
#line 1597 "parser.tab.cpp"
    break;

  case 36: /* Exp: LPAREN Type RPAREN Exp  */
#line 302 "parser.ypp"
                                                                 {
						check(canExplicitCast((yyvsp[0].expression)->type, (yyvsp[-2].exp_type)), output::errorMismatch(yylineno));
						NumericExp* num_exp = dynamic_cast<NumericExp*>((yyvsp[0].expression));
						assert(num_exp);
						
						if((yyvsp[-2].exp_type) == BYTE_EXP)
							num_exp->convertToByte();
						else if((yyvsp[-2].exp_type) == INT_EXP)
							num_exp->convertToInt();
						(yyval.expression) = num_exp;
					}
#line 1613 "parser.tab.cpp"
    break;

  case 39: /* NumericExp: Exp BINOP Exp  */
#line 317 "parser.ypp"
                                              {//TODO: add support for overflow and div by 0 protection.
						checkNumeralType((yyvsp[-2].expression)->type);
						NumericExp* numeric_e1 = dynamic_cast<NumericExp*>((yyvsp[-2].expression));
						assert(numeric_e1);
						checkNumeralType((yyvsp[0].expression)->type);
						NumericExp* numeric_e2 = dynamic_cast<NumericExp*>((yyvsp[0].expression));
						assert(numeric_e2);
						
						ExpType max_type = maxNumeralType(numeric_e1->type, numeric_e2->type);
						if(max_type == INT_EXP){
							numeric_e1->convertToInt();
							numeric_e2->convertToInt();
						}

						string rvalue_exp = cb.binopRvalFormat(numeric_e1->reg, numeric_e2->reg, max_type, (yyvsp[-1].binop));
						(yyval.expression) = new NumericExp(max_type, rvalue_exp);
						delete (yyvsp[-2].expression);
						delete (yyvsp[0].expression);
					}
#line 1637 "parser.tab.cpp"
    break;

  case 40: /* NumericExp: NUM  */
#line 336 "parser.ypp"
                                              {(yyval.expression) = new NumericExp(INT_EXP, " "+std::to_string((yyvsp[0].number_literal)), false);}
#line 1643 "parser.tab.cpp"
    break;

  case 41: /* NumericExp: NUM B  */
#line 337 "parser.ypp"
                                                {
						checkByteTooLarge((yyvsp[-1].number_literal));
						(yyval.expression) = new NumericExp(BYTE_EXP, " "+std::to_string((yyvsp[-1].number_literal)), false);
					}
#line 1652 "parser.tab.cpp"
    break;

  case 42: /* Label: %empty  */
#line 342 "parser.ypp"
                                {(yyval.label) = new string(cb.genLabel("parse_label"));}
#line 1658 "parser.tab.cpp"
    break;

  case 43: /* CondLabel: %empty  */
#line 343 "parser.ypp"
                                {(yyval.label) = new string(cb.genLabel("cond"));}
#line 1664 "parser.tab.cpp"
    break;

  case 44: /* StatementLabel: %empty  */
#line 344 "parser.ypp"
                        {(yyval.label) = new string(cb.genLabel("statement"));}
#line 1670 "parser.tab.cpp"
    break;

  case 45: /* BoolExp: Exp AND Label Exp  */
#line 346 "parser.ypp"
                                                  {
						checkMismatch((yyvsp[-3].expression)->type, BOOL_EXP);
						checkMismatch((yyvsp[0].expression)->type, BOOL_EXP);
						BoolExp* exp1 = dynamic_cast<BoolExp*>((yyvsp[-3].expression));
						assert(exp1);
						BoolExp* exp2 = dynamic_cast<BoolExp*>((yyvsp[0].expression));
						assert(exp2);
						
						cb.bpatch(exp1->truelist, *(yyvsp[-1].label));
						(yyval.expression) = new BoolExp(exp2->truelist, cb.merge(exp1->falselist, exp2->falselist));
					
						delete (yyvsp[-3].expression); delete (yyvsp[-1].label); delete (yyvsp[0].expression);
					}
#line 1688 "parser.tab.cpp"
    break;

  case 46: /* BoolExp: Exp OR Label Exp  */
#line 359 "parser.ypp"
                                                          {
						checkMismatch((yyvsp[-3].expression)->type, BOOL_EXP);
						checkMismatch((yyvsp[0].expression)->type, BOOL_EXP);
						BoolExp* exp1 = dynamic_cast<BoolExp*>((yyvsp[-3].expression));
						assert(exp1);
						BoolExp* exp2 = dynamic_cast<BoolExp*>((yyvsp[0].expression));
						assert(exp2);

						cb.bpatch(exp1->falselist, *(yyvsp[-1].label));
						(yyval.expression) = new BoolExp(cb.merge(exp1->truelist, exp2->truelist), exp2->falselist);

						delete (yyvsp[-3].expression); delete (yyvsp[-1].label); delete (yyvsp[0].expression);
					}
#line 1706 "parser.tab.cpp"
    break;

  case 47: /* BoolExp: Exp RELOP Exp  */
#line 372 "parser.ypp"
                                                        {
						check(isNumeralType((yyvsp[-2].expression)->type) && isNumeralType((yyvsp[0].expression)->type), output::errorMismatch(yylineno));
						NumericExp* exp1 = dynamic_cast<NumericExp*>((yyvsp[-2].expression));
						assert(exp1);
						NumericExp* exp2 = dynamic_cast<NumericExp*>((yyvsp[0].expression));
						assert(exp2);
						
						ExpType operand_type = maxNumeralType(exp1->type, exp2->type);
						if(operand_type == INT_EXP){
							exp1->convertToInt();
							exp2->convertToInt();
						}
						std::string cond_rval = cb.relopRvalFormat(exp1->reg, exp2->reg, operand_type, (yyvsp[-1].relop));
						std::string cond_reg = cb.getFreshReg();
						cb.emit(cond_reg+" = "+cond_rval);
						int br_address = cb.emit("br i1 "+cond_reg+", label @, label @");
						
						(yyval.expression) = new BoolExp(cb.makelist(Backpatch(br_address, FIRST))
							, cb.makelist(Backpatch(br_address, SECOND)));
						
						delete (yyvsp[-2].expression); delete (yyvsp[0].expression);
					}
#line 1733 "parser.tab.cpp"
    break;

  case 48: /* BoolExp: NOT Exp  */
#line 394 "parser.ypp"
                                                  {
						checkMismatch((yyvsp[0].expression)->type, BOOL_EXP);
						BoolExp* exp = dynamic_cast<BoolExp*>((yyvsp[0].expression));
						assert(exp);
						auto tmp = exp->truelist;
						exp->truelist = exp->falselist;
						exp->falselist = tmp;
						(yyval.expression) = exp;
					}
#line 1747 "parser.tab.cpp"
    break;

  case 49: /* BoolExp: TRUE  */
#line 403 "parser.ypp"
                                               {
						int position = cb.emit("br label @");
						Backpatch bp_details(position, FIRST);
						(yyval.expression) = new BoolExp(CodeBuffer::makelist(bp_details), cb.makeEmptyList());
					}
#line 1757 "parser.tab.cpp"
    break;

  case 50: /* BoolExp: FALSE  */
#line 408 "parser.ypp"
                                                {
						int position = cb.emit("br label @");
						Backpatch bp_details(position, FIRST);
						(yyval.expression) = new BoolExp(cb.makeEmptyList(), CodeBuffer::makelist(bp_details));
					}
#line 1767 "parser.tab.cpp"
    break;

  case 51: /* Statement: OpenStatment  */
#line 415 "parser.ypp"
                                             {(yyval.run_block) = (yyvsp[0].run_block);}
#line 1773 "parser.tab.cpp"
    break;

  case 52: /* Statement: ClosedStatment  */
#line 416 "parser.ypp"
                                                         {(yyval.run_block) = (yyvsp[0].run_block);}
#line 1779 "parser.tab.cpp"
    break;

  case 53: /* OpenStatment: IfStart OpenScope Statement CloseScope  */
#line 420 "parser.ypp"
                                                               {
						cb.bpatch((yyvsp[-3].branch_block)->truelist, (yyvsp[-1].run_block)->start_label);
						(yyval.run_block) = new RunBlock((yyvsp[-3].branch_block)->cond_label);
						(yyval.run_block)->nextlist = cb.merge((yyvsp[-3].branch_block)->falselist, (yyvsp[-1].run_block)->nextlist);
						delete (yyvsp[-3].branch_block); delete (yyvsp[-1].run_block);
					}
#line 1790 "parser.tab.cpp"
    break;

  case 54: /* OpenStatment: IfStart OpenScope ClosedStatment CloseScope ELSE OpenScope OpenStatment CloseScope  */
#line 426 "parser.ypp"
                                                                                                                             {
						cb.bpatch((yyvsp[-7].branch_block)->truelist, (yyvsp[-5].run_block)->start_label);
						cb.bpatch((yyvsp[-7].branch_block)->falselist, (yyvsp[-1].run_block)->start_label);
						(yyval.run_block) = new RunBlock((yyvsp[-7].branch_block)->cond_label);
						(yyval.run_block)->nextlist = cb.merge((yyvsp[-5].run_block)->nextlist, (yyvsp[-1].run_block)->nextlist);
						delete (yyvsp[-7].branch_block); delete (yyvsp[-5].run_block); delete (yyvsp[-1].run_block);
					}
#line 1802 "parser.tab.cpp"
    break;

  case 55: /* OpenStatment: WhileStart OpenLoop OpenScope OpenStatment CloseScope CloseLoop  */
#line 433 "parser.ypp"
                                                                                                          {
						cb.bpatch((yyvsp[-5].branch_block)->truelist, (yyvsp[-2].run_block)->start_label);
						cb.bpatch((yyvsp[-2].run_block)->nextlist, (yyvsp[-5].branch_block)->cond_label);
						(yyval.run_block) = new RunBlock((yyvsp[-5].branch_block)->cond_label);
						(yyval.run_block)->nextlist = (yyvsp[-5].branch_block)->falselist;
						delete (yyvsp[-5].branch_block); delete (yyvsp[-2].run_block);
					}
#line 1814 "parser.tab.cpp"
    break;

  case 56: /* ClosedStatment: SimpleStatement  */
#line 442 "parser.ypp"
                                        {(yyval.run_block) = (yyvsp[0].run_block);}
#line 1820 "parser.tab.cpp"
    break;

  case 57: /* ClosedStatment: IfStart OpenScope ClosedStatment CloseScope ELSE OpenScope ClosedStatment CloseScope  */
#line 443 "parser.ypp"
                                                                                                                               {
						cb.bpatch((yyvsp[-7].branch_block)->truelist, (yyvsp[-5].run_block)->start_label);
						cb.bpatch((yyvsp[-7].branch_block)->falselist, (yyvsp[-1].run_block)->start_label);
						(yyval.run_block) = new RunBlock((yyvsp[-7].branch_block)->cond_label);
						(yyval.run_block)->nextlist = cb.merge((yyvsp[-5].run_block)->nextlist, (yyvsp[-1].run_block)->nextlist);
						delete (yyvsp[-7].branch_block); delete (yyvsp[-5].run_block); delete (yyvsp[-1].run_block);
					}
#line 1832 "parser.tab.cpp"
    break;

  case 58: /* ClosedStatment: WhileStart OpenLoop OpenScope ClosedStatment CloseScope CloseLoop  */
#line 450 "parser.ypp"
                                                                                                            {
						cb.bpatch((yyvsp[-5].branch_block)->truelist, (yyvsp[-2].run_block)->start_label);
						cb.bpatch((yyvsp[-2].run_block)->nextlist, (yyvsp[-5].branch_block)->cond_label);
						(yyval.run_block) = new RunBlock((yyvsp[-5].branch_block)->cond_label);
						(yyval.run_block)->nextlist = (yyvsp[-5].branch_block)->falselist;
						delete (yyvsp[-5].branch_block); delete (yyvsp[-2].run_block);
					}
#line 1844 "parser.tab.cpp"
    break;

  case 59: /* $@3: %empty  */
#line 458 "parser.ypp"
                                                    {checkBool((yyvsp[0].expression)->type);}
#line 1850 "parser.tab.cpp"
    break;

  case 60: /* IfStart: IF LPAREN Label Exp $@3 RPAREN  */
#line 458 "parser.ypp"
                                                                                  {
						(yyval.branch_block) = new BranchBlock(*(yyvsp[-3].label), (yyvsp[-2].expression));
						delete (yyvsp[-3].label); delete (yyvsp[-2].expression);
					}
#line 1859 "parser.tab.cpp"
    break;

  case 61: /* $@4: %empty  */
#line 463 "parser.ypp"
                                                           {checkBool((yyvsp[0].expression)->type);}
#line 1865 "parser.tab.cpp"
    break;

  case 62: /* WhileStart: WHILE LPAREN CondLabel Exp $@4 RPAREN  */
#line 463 "parser.ypp"
                                                                                         {
						(yyval.branch_block) = new BranchBlock(*(yyvsp[-3].label), (yyvsp[-2].expression));
						delete (yyvsp[-3].label); delete (yyvsp[-2].expression);
					}
#line 1874 "parser.tab.cpp"
    break;

  case 63: /* SimpleStatement: Block  */
#line 469 "parser.ypp"
                              {(yyval.run_block) = (yyvsp[0].run_block);}
#line 1880 "parser.tab.cpp"
    break;

  case 64: /* SimpleStatement: StatementLabel VarDecStart SC  */
#line 470 "parser.ypp"
                                                                        {
						check(!(yyvsp[-1].dec_info).is_const, output::errorConstDef(yylineno));
						symtab.declareVar(*(yyvsp[-1].dec_info).id, (yyvsp[-1].dec_info).raw_type);
						cb.emitStoreVar(*(yyvsp[-1].dec_info).id, "0");
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-2].label));
						delete (yyvsp[-1].dec_info).id;
					}
#line 1892 "parser.tab.cpp"
    break;

  case 65: /* SimpleStatement: StatementLabel VarDecStart ASSIGN Exp SC  */
#line 477 "parser.ypp"
                                                                                   {
						ExpType id_type = (yyvsp[-3].dec_info).raw_type;
						checkMismatch((yyvsp[-1].expression)->type, (yyvsp[-3].dec_info).raw_type);
						if(id_type == INT_EXP)
							dynamic_cast<NumericExp*>((yyvsp[-1].expression))->convertToInt();
						
						if((yyvsp[-3].dec_info).is_const){
							//TODO: what if Exp is not a nuemrical expresion ?
							assert(id_type == BOOL_EXP || id_type == INT_EXP || id_type == BYTE_EXP);
							std::string reg_or_literal = id_type == BOOL_EXP
								? dynamic_cast<BoolExp*>((yyvsp[-1].expression))->storeAsRawReg()
								: dynamic_cast<NumericExp*>((yyvsp[-1].expression))->storeAsRawReg();
							symtab.declareConstVar(*(yyvsp[-3].dec_info).id, id_type, reg_or_literal);
							if(!constValueIsNumLiteral(reg_or_literal))//store the variable on the stack:
								cb.emitStoreVar(*(yyvsp[-3].dec_info).id, reg_or_literal);		
						} else {
							symtab.declareVar(*(yyvsp[-3].dec_info).id, id_type);
							cb.emitStoreVar(*(yyvsp[-3].dec_info).id, (yyvsp[-1].expression));	
						}
						
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-4].label));
						delete (yyvsp[-3].dec_info).id; delete (yyvsp[-1].expression);
					}
#line 1920 "parser.tab.cpp"
    break;

  case 66: /* SimpleStatement: StatementLabel ID ASSIGN Exp SC  */
#line 500 "parser.ypp"
                                                                          {
						check(symtab.containsVar(*(yyvsp[-3].id)), output::errorUndef(yylineno, *(yyvsp[-3].id)));
						check(!symtab.isConst(*(yyvsp[-3].id)), output::errorConstMismatch(yylineno));
						ExpType id_type = symtab.getVariableType(*(yyvsp[-3].id));
						checkMismatch((yyvsp[-1].expression)->type, id_type);
						if(id_type == INT_EXP)
							dynamic_cast<NumericExp*>((yyvsp[-1].expression))->convertToInt();
						cb.emitStoreVar(*(yyvsp[-3].id), (yyvsp[-1].expression));
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-4].label));
						delete (yyvsp[-3].id); delete (yyvsp[-1].expression);
					}
#line 1936 "parser.tab.cpp"
    break;

  case 67: /* SimpleStatement: StatementLabel Call SC  */
#line 511 "parser.ypp"
                                                                 {
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-2].label));
					}
#line 1944 "parser.tab.cpp"
    break;

  case 68: /* $@5: %empty  */
#line 514 "parser.ypp"
                                                                {checkMismatch(VOID_EXP, symtab.getCurrentlyParsedFuncType().return_type);}
#line 1950 "parser.tab.cpp"
    break;

  case 69: /* SimpleStatement: StatementLabel RETURN $@5 SC  */
#line 514 "parser.ypp"
                                                                                                                                               {
						(yyval.run_block) = RunBlock::newSinkBlockEndingHere(*(yyvsp[-3].label));
						ExpType ret_type = symtab.getCurrentlyParsedFuncType().return_type;
						cb.emit("ret "+cb.IrDefaultTypedValue(ret_type)); 
					}
#line 1960 "parser.tab.cpp"
    break;

  case 70: /* $@6: %empty  */
#line 519 "parser.ypp"
                                                                    {
						checkMismatch((yyvsp[0].expression)->type, symtab.getCurrentlyParsedFuncType().return_type);
						check((yyvsp[0].expression)->type != VOID_EXP, output::errorMismatch(yylineno));
						check((yyvsp[0].expression)->type != STRING_EXP, output::errorMismatch(yylineno));
					}
#line 1970 "parser.tab.cpp"
    break;

  case 71: /* SimpleStatement: StatementLabel RETURN Exp $@6 SC  */
#line 523 "parser.ypp"
                                             {
						(yyval.run_block) = RunBlock::newSinkBlockEndingHere(*(yyvsp[-4].label));
						ExpType ret_type = symtab.getCurrentlyParsedFuncType().return_type;
						if(ret_type == INT_EXP)
							dynamic_cast<NumericExp*>((yyvsp[-2].expression))->convertToInt();
						switch((yyvsp[-2].expression)->type){
						case INT_EXP:
						case BYTE_EXP:{
							NumericExp* numeric_exp = dynamic_cast<NumericExp*>((yyvsp[-2].expression));
							assert(numeric_exp);
							cb.emit("ret "+cb.IrType(numeric_exp->type)+numeric_exp->reg);}
							break;
						case BOOL_EXP:{
							BoolExp* bool_exp = dynamic_cast<BoolExp*>((yyvsp[-2].expression));
							assert(bool_exp);
							string res_reg = bool_exp->storeAsReg();
							cb.emit("ret i1 "+res_reg);}
							break;
						default:
							assert(false);
						}
						delete (yyvsp[-2].expression);
					}
#line 1998 "parser.tab.cpp"
    break;

  case 72: /* SimpleStatement: StatementLabel BREAK SC  */
#line 546 "parser.ypp"
                                                                  {//TODO: add support for break.
						check(loop_depth!=0, output::errorUnexpectedBreak(yylineno));
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-2].label));
					}
#line 2007 "parser.tab.cpp"
    break;

  case 73: /* SimpleStatement: StatementLabel CONTINUE SC  */
#line 550 "parser.ypp"
                                                                     {//TODO: add support for break.
						check(loop_depth!=0, output::errorUnexpectedContinue(yylineno));
						(yyval.run_block) = RunBlock::newBlockEndingHere(*(yyvsp[-2].label));
					}
#line 2016 "parser.tab.cpp"
    break;

  case 74: /* VarDecStart: TypeAnnotation Type ID  */
#line 555 "parser.ypp"
                                               {
						check(symtab.declarableValidId(*(yyvsp[0].id)), output::errorDef(yylineno, *(yyvsp[0].id)));
						(yyval.dec_info) = {.is_const = (yyvsp[-2].is_const), .raw_type = (yyvsp[-1].exp_type), .id = (yyvsp[0].id)};
					}
#line 2025 "parser.tab.cpp"
    break;


#line 2029 "parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 560 "parser.ypp"

void yyerror(const char* s){
	output::errorSyn(yylineno);
	exit(1);
}

void declareLibraryFuncs(){
	//FunctionType creates a shared pinter for these allocations:
	std::vector<Parameter>* print_params = new std::vector<Parameter>();
	std::vector<Parameter>* printi_params = new std::vector<Parameter>();

	//in print/i line of origin and is_const parameters are irrelevant, but have to be given some value:
	print_params->push_back(Parameter("str", STRING_EXP, 0, true));
	printi_params->push_back(Parameter("i", INT_EXP, 0, true));
	symtab.declareFunc("print", VOID_EXP, print_params);
	symtab.finishFunc(false);
	symtab.declareFunc("printi", VOID_EXP, printi_params);
	symtab.finishFunc(false);
}

int main(){
	#ifdef MYDB
		yydebug = 1;
	#endif
	symtab = SimpleSymtab();
	declareLibraryFuncs();
	#ifndef OLDT
	cb.emitLibFuncs();
	#endif

	loop_depth = 0;
	yyparse();
	
	checkMainMissing();
	#ifdef OLDT
	output::endScope();//this is the global scope.
	symtab.printFuncDecls();
	#else
	cb.printGlobalBuffer();
	cb.printCodeBuffer();
	#endif

	return 0;
}
