%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {KLF}
%define parser_class_name {KLFunParser}

%code requires {

	#include "Command.h"
	#include "KLFun.h"
	
	namespace KLF {
		class ParserDriver;
 		class KLFunLexer;
	}
}

%lex-param { KLFunLexer &scanner }
%parse-param { KLFunLexer &scanner }

%lex-param { ParserDriver &driver }
%parse-param { ParserDriver &driver }

%code{
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

}

/* token types */
%union {
	std::string *stringval;
	std::vector<int> *vectival;

	Command *command;
	Control *control;
	KBlock *blockval;

	int intval;
	float floatval;
	double doubleval;
}

%type <intval> script_file;

%type <blockval> atom
%type <blockval> expression
%type <blockval> block
%type <intval> statement
%type <intval> statement_list
%type <intval> script_item_list
%type <intval> simple_definition
%type <intval> handler_definition

%type <vectival> dimension_list

%token END 0 "end of file"

%token <stringval> WORD
%token <stringval> SYMBOL

%token <intval> TYPE // simple type eg int, byte ... not individually significant syntactically 
%token <intval> EVENT // similar to a lambda/function definition, but hooking into specific callbacks
%token <stringval> IDENT

%token <intval> LITERAL_INT
%token <doubleval> LITERAL_FLOAT
%token <vectival> LITERAL_TIME
%token <stringval> LITERAL_STRING

%token <command> COMMAND
%token <control> CONTROL

%token ASSGN
%token IF
%token ELSE
%token ON

%token LSQB
%token RSQB
%token LBRA
%token RBRA
%token LB
%token RB

%token COMMA
%token COLON
%token COLON2
%token SEMI
%token DOT

%token PLUS
%token MINUS
%token MULT
%token DIVIDE
%token MOD
%token POWER

%token LT
%token GT
%token LE
%token GE
%token EQ
%token NE

%token AND
%token OR
%token NOT

%token NEWLINE

%left OR
%left AND
%left NOT
%nonassoc LT GT LE GE EQ NE
%left PLUS MINUS
%left MULT DIVIDE MOD
%left NEG
%right POWER


/* destructor rule for <sval> objects */
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <stringval>
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <vectival>

%%

script_file
	: script_item_list { $$ = $1; }
	;
	
//////////////////////////////////
// statements and top-level defs
//////////////////////////////////

statement
	: IDENT ASSGN expression {
			$$ = 0;
		}
	| CONTROL ASSGN expression {
			$$ = 0;
		}
	| COMMAND {
			$$ = 0;
		}
	| LBRA statement_list RBRA {
			$$ = 0;
		}
	;

		
script_item_list
	: { $$ = 0; }
	| script_item_list statement {
			$$ = 0;
		}
	| script_item_list simple_definition {
			$$ = 0;
		}
	| script_item_list handler_definition {
			$$ = 0;
		}
	;

statement_list : { $$ = 0; }
	| statement_list statement { $$ = 0; }
	;

simple_definition
	: TYPE IDENT { 
			$$ = 0;
		}
	;

handler_definition
	: ON EVENT statement {
			$$ = 0;
		}
	;

////////////////////////////
// expressions
////////////////////////////
atom : IDENT {
			$$ = new KRValue($1);
		}
	| CONTROL {
			$$ = new KControl($1);
		}
	| LITERAL_INT {
			$$ = new KConstant($1);
		}
	| LITERAL_FLOAT {
			$$ = new KConstant($1);
		}
	| LITERAL_STRING {
			$$ = new KConstant($1);
		}
	| LITERAL_TIME {
			$$ = new KConstant($1);
		}
	;
	
expression 
	: atom { $$ = $1; }
	| expression PLUS expression	{ $$ = new KBinop(token_type::PLUS, $1, $3); }
	| expression MINUS expression	{ $$ = new KBinop(token_type::MINUS, $1, $3); }
	| expression MULT expression	{ $$ = new KBinop(token_type::MULT, $1, $3); }
	| expression MOD expression		{ $$ = new KBinop(token_type::MOD, $1, $3); }
	| expression DIVIDE expression	{ $$ = new KBinop(token_type::DIVIDE, $1, $3); }
	| MINUS expression %prec NEG	{ $$ = new KUnop(token_type::MINUS, $2); }
	| NOT expression				{ $$ = new KUnop(token_type::NOT, $2); }
	| expression LT expression		{ $$ = new KBinop(token_type::LT, $1, $3); }
	| expression GT expression		{ $$ = new KBinop(token_type::GT, $1, $3); }
	| expression NE expression		{ $$ = new KBinop(token_type::NE, $1, $3); }
	| expression EQ expression		{ $$ = new KBinop(token_type::EQ, $1, $3); }
	| expression GE expression		{ $$ = new KBinop(token_type::GE, $1, $3); }
	| expression LE expression		{ $$ = new KBinop(token_type::LE, $1, $3); }
	| expression AND expression		{ $$ = new KBinop(token_type::AND, $1, $3); }
	| expression OR expression		{ $$ = new KBinop(token_type::OR, $1, $3); }
	| expression POWER expression	{ $$ = new KBinop(token_type::POWER, $1, $3); }
	| LB expression RB				{ $$ = $2; }
	;


%%
 

void 
KLF::KLFunParser::error( const std::string &err_message )
{
   std::cerr << "Error: " << err_message << ", near line " << scanner.lineno() << "\n"; 
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

