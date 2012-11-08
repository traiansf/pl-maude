/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FILE_NAME_STRING = 258,
     UNINTERPRETED_STRING = 259,
     LATEX_STRING = 260,
     KW_MOD = 261,
     KW_OMOD = 262,
     KW_VIEW = 263,
     KW_PARSE = 264,
     KW_NORMALIZE = 265,
     KW_REDUCE = 266,
     KW_REWRITE = 267,
     KW_LOOP = 268,
     KW_NARROW = 269,
     KW_XG_NARROW = 270,
     KW_MATCH = 271,
     KW_XMATCH = 272,
     KW_UNIFY = 273,
     KW_GET = 274,
     KW_VARIANTS = 275,
     KW_VARIANT = 276,
     KW_EREWRITE = 277,
     KW_FREWRITE = 278,
     KW_SREWRITE = 279,
     KW_CONTINUE = 280,
     KW_SEARCH = 281,
     KW_SET = 282,
     KW_SHOW = 283,
     KW_ON = 284,
     KW_OFF = 285,
     KW_TRACE = 286,
     KW_BODY = 287,
     KW_BUILTIN = 288,
     KW_WHOLE = 289,
     KW_SELECT = 290,
     KW_DESELECT = 291,
     KW_CONDITION = 292,
     KW_SUBSTITUTION = 293,
     KW_PRINT = 294,
     KW_GRAPH = 295,
     KW_MIXFIX = 296,
     KW_FLAT = 297,
     KW_ATTRIBUTE = 298,
     KW_NEWLINE = 299,
     KW_WITH = 300,
     KW_PARENS = 301,
     KW_ALIASES = 302,
     KW_GC = 303,
     KW_TIME = 304,
     KW_STATS = 305,
     KW_TIMING = 306,
     KW_CMD = 307,
     KW_BREAKDOWN = 308,
     KW_BREAK = 309,
     KW_PATH = 310,
     KW_MODULE = 311,
     KW_MODULES = 312,
     KW_VIEWS = 313,
     KW_ALL = 314,
     KW_SORTS = 315,
     KW_OPS2 = 316,
     KW_VARS = 317,
     KW_MBS = 318,
     KW_EQS = 319,
     KW_RLS = 320,
     KW_SUMMARY = 321,
     KW_KINDS = 322,
     KW_ADVISE = 323,
     KW_VERBOSE = 324,
     KW_DO = 325,
     KW_CLEAR = 326,
     KW_PROTECT = 327,
     KW_EXTEND = 328,
     KW_INCLUDE = 329,
     KW_EXCLUDE = 330,
     KW_CONCEAL = 331,
     KW_REVEAL = 332,
     KW_COMPILE = 333,
     KW_COUNT = 334,
     KW_DEBUG = 335,
     KW_RESUME = 336,
     KW_ABORT = 337,
     KW_STEP = 338,
     KW_WHERE = 339,
     KW_CREDUCE = 340,
     KW_SREDUCE = 341,
     KW_DUMP = 342,
     KW_PROFILE = 343,
     KW_NUMBER = 344,
     KW_RAT = 345,
     KW_COLOR = 346,
     SIMPLE_NUMBER = 347,
     KW_PWD = 348,
     KW_CD = 349,
     KW_PUSHD = 350,
     KW_POPD = 351,
     KW_LS = 352,
     KW_LOAD = 353,
     KW_QUIT = 354,
     KW_EOF = 355,
     KW_ENDM = 356,
     KW_IMPORT = 357,
     KW_ENDV = 358,
     KW_SORT = 359,
     KW_SUBSORT = 360,
     KW_OP = 361,
     KW_OPS = 362,
     KW_MSGS = 363,
     KW_VAR = 364,
     KW_CLASS = 365,
     KW_SUBCLASS = 366,
     KW_MB = 367,
     KW_CMB = 368,
     KW_EQ = 369,
     KW_CEQ = 370,
     KW_RL = 371,
     KW_CRL = 372,
     KW_IS = 373,
     KW_FROM = 374,
     KW_ARROW = 375,
     KW_ARROW2 = 376,
     KW_PARTIAL = 377,
     KW_IF = 378,
     KW_LABEL = 379,
     KW_TO = 380,
     KW_COLON2 = 381,
     KW_ASSOC = 382,
     KW_COMM = 383,
     KW_ID = 384,
     KW_IDEM = 385,
     KW_ITER = 386,
     KW_LEFT = 387,
     KW_RIGHT = 388,
     KW_PREC = 389,
     KW_GATHER = 390,
     KW_METADATA = 391,
     KW_STRAT = 392,
     KW_POLY = 393,
     KW_MEMO = 394,
     KW_FROZEN = 395,
     KW_CTOR = 396,
     KW_LATEX = 397,
     KW_SPECIAL = 398,
     KW_CONFIG = 399,
     KW_OBJ = 400,
     KW_MSG = 401,
     KW_DITTO = 402,
     KW_FORMAT = 403,
     KW_ID_HOOK = 404,
     KW_OP_HOOK = 405,
     KW_TERM_HOOK = 406,
     KW_IN = 407,
     IDENTIFIER = 408,
     NUMERIC_ID = 409,
     ENDS_IN_DOT = 410,
     FORCE_LOOKAHEAD = 411
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 103 "surface.yy"

  bool yyBool;
  Int64 yyInt64;
  const char* yyString;
  Token yyToken;
  ImportModule::ImportMode yyImportMode;
  Interpreter::Flags yyFlags;
  Interpreter::PrintFlags yyPrintFlags;
  Interpreter::SearchKind yySearchKind;



/* Line 2068 of yacc.c  */
#line 219 "surface.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




