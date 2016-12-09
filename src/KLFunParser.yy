%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {KLF}
%define parser_class_name {KLFunParser}

%code requires {

	#include "Command.h"
	
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
%type <intval> block



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
%token STAR
%token SLASH

%token NEWLINE

/* destructor rule for <sval> objects */
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <stringval>
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <vectival>

%%

script_file
	: atom { $$ = $1; }
	;
	
////////////////////////////
// expressions
////////////////////////////
atom : LB expression RB { $$ = $2; }
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
	
expression : atom { $$ = $1; }
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

