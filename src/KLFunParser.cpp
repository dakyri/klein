// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "KLFunParser.hpp"

// User implementation prologue.

#line 51 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 26 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:413

	#include <iostream>
	#include <cstdlib>
	#include <fstream>
	#include <string>
	#include <vector>

	using namespace std;
   /* include for all driver functions */
	#include "ParserDriver.h"
  
   /* this is silly, but I can't figure out a way around */
	static int yylex(KLF::KLFunParser::semantic_type *yylval,
					KLF::KLFunLexer &scanner,
					KLF::ParserDriver &driver);


#line 71 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:479
namespace KLF {
#line 138 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:479

  /// Build a parser object.
  KLFunParser::KLFunParser (KLFunLexer &scanner_yyarg, ParserDriver &driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

  KLFunParser::~KLFunParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  KLFunParser::syntax_error::syntax_error (const std::string& m)
    : std::runtime_error (m)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  KLFunParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  KLFunParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
  {
    value = other.value;
  }


  template <typename Base>
  inline
  KLFunParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v)
    : Base (t)
    , value (v)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  KLFunParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  inline
  KLFunParser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  KLFunParser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  KLFunParser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  KLFunParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
  }

  // by_type.
  inline
  KLFunParser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  KLFunParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  KLFunParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  KLFunParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  KLFunParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  KLFunParser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  KLFunParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  KLFunParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  KLFunParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  KLFunParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  KLFunParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  KLFunParser::symbol_number_type
  KLFunParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  KLFunParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  KLFunParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  KLFunParser::stack_symbol_type&
  KLFunParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }


  template <typename Base>
  inline
  void
  KLFunParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 3: // WORD

#line 145 "D:\src\cpp\Klein\src\KLFunParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 348 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:614
        break;

      case 4: // SYMBOL

#line 145 "D:\src\cpp\Klein\src\KLFunParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 355 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:614
        break;

      case 7: // IDENT

#line 145 "D:\src\cpp\Klein\src\KLFunParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 362 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:614
        break;

      case 11: // LITERAL_STRING

#line 145 "D:\src\cpp\Klein\src\KLFunParser.yy" // lalr1.cc:614
        { if ((yysym.value.stringval))  { delete ((yysym.value.stringval)); ((yysym.value.stringval)) = nullptr; } }
#line 369 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  KLFunParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  KLFunParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  KLFunParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  KLFunParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  KLFunParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  KLFunParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  KLFunParser::debug_level_type
  KLFunParser::debug_level () const
  {
    return yydebug_;
  }

  void
  KLFunParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline KLFunParser::state_type
  KLFunParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  KLFunParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  KLFunParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  KLFunParser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, scanner, driver));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 151 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			Sym *s = stab.define(driver.name, T_KLF);
			stab.enterScope(s);
		}
#line 602 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 3:
#line 154 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			stab.leaveScope();
		}
#line 610 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 4:
#line 164 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			Sym *s = stab.find(*(yystack_[2].value.stringval));
			if (s != nullptr) {
				(yylhs.value.blockval) = new KVarAssBlock(s, (yystack_[0].value.blockval));
			} else {
				driver.addErrorMessage((*(yystack_[2].value.stringval))+" not found near line.");
			}
		}
#line 623 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 5:
#line 172 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KCtlAssBlock((yystack_[2].value.control), (yystack_[0].value.blockval));
		}
#line 631 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 6:
#line 175 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KCmdBlock((yystack_[0].value.command));
		}
#line 639 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 178 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = (yystack_[1].value.listval);
		}
#line 647 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 187 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			driver.name = *(yystack_[0].value.stringval);
		}
#line 655 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 190 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			driver.sustainTime = (yystack_[0].value.intval);
		}
#line 663 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 193 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			driver.clickTime = (yystack_[0].value.intval);
		}
#line 671 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 199 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.intval) = 0; }
#line 677 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 200 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.intval) = 0;
		}
#line 685 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 15:
#line 203 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.intval) = 0;
		}
#line 693 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 206 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.intval) = 0;
		}
#line 701 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 211 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.listval) = new KListBlock();
		}
#line 709 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 214 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yystack_[1].value.listval)->add((yystack_[0].value.blockval));
			(yylhs.value.listval) = (yystack_[1].value.listval);
		}
#line 718 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 221 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { 
			Sym *s = stab.define(*(yystack_[0].value.stringval), (yystack_[1].value.intval));
		}
#line 726 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 227 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			if (driver.fun) {
				if (!driver.fun->setEventHandler((yystack_[1].value.intval), (yystack_[0].value.blockval))) {
					driver.addErrorMessage("Error while setting handler");
				}
			} else {
				driver.addErrorMessage("No Functional defined while setting handler");
				delete (yystack_[0].value.blockval);
			}
		}
#line 741 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 21:
#line 242 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			Sym *s = stab.find(*(yystack_[0].value.stringval));
			if (s == nullptr) {
				driver.addErrorMessage((*(yystack_[0].value.stringval))+" no defined near line "+to_string(scanner.lineno()));
			}
			(yylhs.value.blockval) = new KRValue(s);
		}
#line 753 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 249 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KControl((yystack_[0].value.control));
		}
#line 761 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 252 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KConstant((yystack_[0].value.intval));
		}
#line 769 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 255 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KConstant((yystack_[0].value.doubleval));
		}
#line 777 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 258 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KConstant((yystack_[0].value.stringval));
		}
#line 785 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 261 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    {
			(yylhs.value.blockval) = new KConstant((yystack_[0].value.timeval));
		}
#line 793 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 267 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = (yystack_[0].value.blockval); }
#line 799 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 268 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::PLUS, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 805 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 269 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::MINUS, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 811 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 270 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::MULT, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 817 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 271 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::MOD, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 823 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 272 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::DIVIDE, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 829 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 273 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KUnop(token_type::MINUS, (yystack_[0].value.blockval)); }
#line 835 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 274 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KUnop(token_type::NOT, (yystack_[0].value.blockval)); }
#line 841 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 275 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::LT, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 847 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 276 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::GT, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 853 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 277 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::NE, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 859 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 278 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::EQ, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 865 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 39:
#line 279 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::GE, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 871 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 280 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::LE, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 877 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 41:
#line 281 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::AND, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 883 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 42:
#line 282 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::OR, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 889 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 283 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = new KBinop(token_type::POWER, (yystack_[2].value.blockval), (yystack_[0].value.blockval)); }
#line 895 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 284 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:859
    { (yylhs.value.blockval) = (yystack_[1].value.blockval); }
#line 901 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
    break;


#line 905 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  KLFunParser::error (const syntax_error& yyexc)
  {
    error (yyexc.what());
  }

  // Generate an error message.
  std::string
  KLFunParser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char KLFunParser::yypact_ninf_ = -25;

  const signed char KLFunParser::yytable_ninf_ = -1;

  const signed char
  KLFunParser::yypact_[] =
  {
     -25,     6,    19,   -25,    -3,     9,    16,   -25,   -25,   -25,
     -25,   -25,     0,     7,     8,   -25,    10,    29,   -25,   -25,
     -25,   -25,   -25,    -7,    -7,    69,    67,   -25,   -25,   -25,
     -25,   -25,   -25,    -7,    -7,    -7,   -25,    61,    61,   -25,
     -25,   -25,    28,    -1,    88,    -7,    -7,    -7,    -7,    -7,
      -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,    -7,   -25,
     -24,   -24,    -1,    -1,    -1,    -1,   100,   100,   100,   100,
     100,   100,    88,    75
  };

  const unsigned char
  KLFunParser::yydefact_[] =
  {
       8,     0,     2,     1,     0,     0,     0,    13,     9,    10,
      12,    11,     3,     0,     0,     6,     0,     0,    17,    14,
      15,    16,    19,     0,     0,     0,     0,    21,    23,    24,
      26,    25,    22,     0,     0,     0,    27,     4,     5,    20,
       7,    18,     0,    33,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    44,
      28,    29,    30,    32,    31,    43,    35,    36,    40,    39,
      38,    37,    41,    42
  };

  const signed char
  KLFunParser::yypgoto_[] =
  {
     -25,   -25,   -25,    -4,   -25,   -25,   -25,   -25,   -25,   -25,
     -25,    -5
  };

  const signed char
  KLFunParser::yydefgoto_[] =
  {
      -1,     1,     7,    19,     2,     8,    12,    26,    20,    21,
      36,    37
  };

  const signed char
  KLFunParser::yytable_[] =
  {
      27,    28,    29,    30,    31,    13,     3,    14,     9,    32,
      47,    48,    49,    50,    22,    15,    16,    10,    33,    38,
      17,    39,    41,    18,    11,    23,    34,    24,    42,    43,
      44,     4,     5,     6,     0,    25,    50,     0,     0,    35,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    59,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    14,     0,    14,     0,     0,     0,
       0,     0,    15,    16,    15,    16,     0,     0,     0,     0,
      18,    40,    18,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    45,    46,    47,    48,    49,    50,    -1,    -1,
      -1,    -1,    -1,    -1
  };

  const signed char
  KLFunParser::yycheck_[] =
  {
       7,     8,     9,    10,    11,     5,     0,     7,    11,    16,
      34,    35,    36,    37,     7,    15,    16,     8,    25,    24,
      20,    25,    26,    23,     8,    17,    33,    17,    33,    34,
      35,    12,    13,    14,    -1,     6,    37,    -1,    -1,    46,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    26,    -1,    -1,    -1,    -1,    -1,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,     7,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    15,    16,    15,    16,    -1,    -1,    -1,    -1,
      23,    24,    23,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43
  };

  const unsigned char
  KLFunParser::yystos_[] =
  {
       0,    50,    53,     0,    12,    13,    14,    51,    54,    11,
       8,     8,    55,     5,     7,    15,    16,    20,    23,    52,
      57,    58,     7,    17,    17,     6,    56,     7,     8,     9,
      10,    11,    16,    25,    33,    46,    59,    60,    60,    52,
      24,    52,    60,    60,    60,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60
  };

  const unsigned char
  KLFunParser::yyr1_[] =
  {
       0,    49,    51,    50,    52,    52,    52,    52,    53,    53,
      54,    54,    54,    55,    55,    55,    55,    56,    56,    57,
      58,    59,    59,    59,    59,    59,    59,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60
  };

  const unsigned char
  KLFunParser::yyr2_[] =
  {
       0,     2,     0,     3,     3,     3,     1,     3,     0,     2,
       2,     2,     2,     0,     2,     2,     2,     0,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const KLFunParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "WORD", "SYMBOL", "TYPE",
  "EVENT", "IDENT", "LITERAL_INT", "LITERAL_FLOAT", "LITERAL_TIME",
  "LITERAL_STRING", "ATTRIB_NAME", "ATTRIB_MULTICLICK", "ATTRIB_SUSTAIN",
  "COMMAND", "CONTROL", "ASSGN", "IF", "ELSE", "ON", "LSQB", "RSQB",
  "LBRA", "RBRA", "LB", "RB", "COMMA", "COLON", "COLON2", "SEMI", "DOT",
  "PLUS", "MINUS", "MULT", "DIVIDE", "MOD", "POWER", "LT", "GT", "LE",
  "GE", "EQ", "NE", "AND", "OR", "NOT", "NEWLINE", "NEG", "$accept",
  "script_file", "$@1", "statement", "script_attribute_list",
  "script_attribute", "script_item_list", "statement_list",
  "simple_definition", "handler_definition", "atom", "expression", YY_NULLPTR
  };


  const unsigned short int
  KLFunParser::yyrline_[] =
  {
       0,   151,   151,   151,   164,   172,   175,   178,   184,   184,
     187,   190,   193,   199,   200,   203,   206,   211,   214,   221,
     227,   242,   249,   252,   255,   258,   261,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284
  };

  // Print the state stack on the debug stream.
  void
  KLFunParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  KLFunParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  KLFunParser::token_number_type
  KLFunParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48
    };
    const unsigned int user_token_number_max_ = 303;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 5 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:1167
} // KLF
#line 1296 "D:\\src\\cpp\\Klein\\src\\KLFunParser.cpp" // lalr1.cc:1167
#line 288 "D:\\src\\cpp\\Klein\\src\\KLFunParser.yy" // lalr1.cc:1168

 

void 
KLF::KLFunParser::error( const std::string &err_message )
{
   driver.addErrorMessage(driver.formatMessage(err_message)); 
}


/* include for access to scanner.yylex */
#include "KLFunLexer.h"
static int 
yylex( KLF::KLFunParser::semantic_type *yylval,
	   KLF::KLFunLexer &scanner,
	   KLF::ParserDriver &driver )
{
   return( scanner.yylex(yylval) );
}

