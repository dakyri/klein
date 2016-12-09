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
	
	int intval;
	float floatval;
	double doubleval;
}

%type <intval> script_file;

%type <intval> atom
%type <intval> expression
%type <intval> statement
%type <intval> statement_list
%type <intval> script_item_list
%type <intval> block
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
%left MULT DIVIDE
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
	| script_item_list statement { $$ = 0; }
	| script_item_list simple_definition { $$ = 0; }
	| script_item_list handler_definition { $$ = 0; }
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
			$$ = 0;
		}
	| CONTROL {
			$$ = 0;
		}
	| LITERAL_INT {
			$$ = 0;
		}
	| LITERAL_FLOAT {
			$$ = 0;
		}
	| LITERAL_STRING {
			$$ = 0;
		}
	| LITERAL_TIME {
			$$ = 0;
		}
	;
	
expression 
	: atom { $$ = $1; }
	| expression PLUS expression	{ $$ = $1 + $3; }
	| expression MINUS expression	{ $$ = $1 - $3; }
	| expression MULT expression	{ $$ = $1 * $3; }
	| expression DIVIDE expression	{ $$ = $1 / $3; }
	| MINUS expression %prec NEG	{ $$ = -$2; }
	| NOT expression				{ $$ = !$2; }
	| expression LT expression		{ $$ = $1 < $3; }
	| expression GT expression		{ $$ = $1 < $3; }
	| expression NE expression		{ $$ = $1 == $3; }
	| expression EQ expression		{ $$ = $1 != $3; }
	| expression GE expression		{ $$ = $1 >= $3; }
	| expression LE expression		{ $$ = $1 <= $3; }
	| expression AND expression		{ $$ = $1 && $3; }
	| expression OR expression		{ $$ = $1 || $3; }
	| expression POWER expression	{ $$ = $1 ^ $3; }
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

