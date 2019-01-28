%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include"syntax.tab.h"
	char symbol[2048];
	int symbolPoint;
	int flag;
	void insert_fix(char* name);
	void printNode();
	void insert(YYSTYPE a, char*name);
%}
%option yylineno
%{
	int yycolumn = 1;
	#define YY_USER_ACTION\
	yylloc.first_line = yylloc.last_line = yylineno;\
	yylloc.first_column = yycolumn;\
	yylloc.last_column = yycolumn+yyleng-1;\
	yycolumn += yyleng;
%}
digit [0-9]
letter [a-zA-Z_]
DINT 0|[1-9]{digit}{0,8}|[1-3]{digit}{9}|4[0-1]{digit}{8}|42[0-8]{digit}{7}|429[0-3]{digit}{6}|4294[0-8]{digit}{5}|42949[0-5]{digit}{4}|429496[0-6]{digit}{3}|4294967[01]{digit}{2}|42949672[0-8]{digit}{1}|429496729[0-5]
HINT 0[xX]0*[0-9a-fA-F]{0,8}
OINT 00*([0-7]{1,10}|[12][0-7]{10}|3[0-7]{10}) 
TYPE ((((signed)|(unsigned))(\ )+)?(int|char|long|short)|unsigned|signed|float|double|bool)(\ |\*)*
FLOAT ([1-9]{digit}*|0)(\.{digit}*)?([eE][+-]?{digit}+)?|\.{digit}+([eE][+-]?([1-9]{digit}*|0))?
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
VOID void
VOLATILE volatile
ENUM enum
STRUCT struct
UNION union
IF if
ELSE else
GOTO goto
SWITCH switch
CASE case
DO do
WHILE while
FOR for
CONTINUE continue
BREAK break
RETURN return
DEFAULT default
TYPEDEF typedef
AUTO auto
REGISTER register
EXTERN extern
STATIC static
ID {letter}({letter}|{digit}){0,31}
%%
"//" {
	char c = input();
	while(c != '\n') c = input();
	yycolumn = 1;	
	}
"/*"  {
	int comment = 0;
	char c = input();
	while((comment == 0) || c !='/'){
		if(c == '*'){
			comment = 1;		
		}
		else {
			comment = 0;
			
		}
		if(c == '\n'){
			yycolumn = 1;
		//	yylineno ++;		
		}
		else{
			yycolumn ++;		
		}
		c = input();		
	}	
     }
{VOID} {/*insert_fix("VOID");*/return VOID;}
{VOLATILE} {/*insert_fix("VOLATILE");*/return VOLATILE;}
{ENUM} {/*insert_fix("ENUM");*/return ENUM;}
{STRUCT} {/*insert_fix("STRUCT");*/return STRUCT;}
{UNION} {/*insert_fix("UNION");*/return UNION;}
{IF} {/*insert_fix("IF");*/return IF;}
{ELSE} {/*insert_fix("ELSE");*/return ELSE;}
{GOTO} {/*insert_fix("GOTO");*/return GOTO;}
{SWITCH} {/*insert_fix("SWITCH");*/return SWITCH;}
{CASE} {/*insert_fix("CASE");*/return CASE;}
{DO} {/*insert_fix("DO");*/return DO;}
{WHILE} {/*insert_fix("WHILE");*/return WHILE;}
{FOR} {/*insert_fix("FOR");*/return FOR;}
{CONTINUE} {/*insert_fix("CONTINUE");*/return CONTINUE;}
{BREAK} {/*insert_fix("BREAK");*/return BREAK;}
{RETURN} {/*insert_fix("RETURN");*/return RETURN;}
{DEFAULT} {/*insert_fix("DEFAULT");*/return DEFAULT;}
{TYPEDEF} {/*insert_fix("TYPEDEF");*/return TYPEDEF;}
{AUTO} {/*insert_fix("AUTO");*/return AUTO;}
{REGISTER} {/*insert_fix("REGISTER");*/return REGISTER;}
{EXTERN} {/*insert_fix("EXTERN");*/return EXTERN;}
{STATIC} {/*insert_fix("STATIC");*/return STATIC;}
{SEMI} {/*printf("SEMI\n");*//*insert_fix("SEMI");*/return SEMI;}
{COMMA} {/*insert_fix("COMMA");*/return COMMA;}
{ASSIGNOP} {/*printf("ASSIGNOP\n");*//*insert_fix("ASSIGNOP");*/return ASSIGNOP;}
{RELOP} {	
	
	yylval = (YYSTYPE)symbolPoint;
	strcpy(symbol+symbolPoint,yytext);
	symbolPoint += sizeof(yytext);
	symbol[symbolPoint++] = '\0';
	//insert(yylval,"RELOP");
	return RELOP;
	}
{PLUS} {/*insert_fix("PLUS");*/return PLUS;}
{MINUS} {/*printf("MINUS");*//*insert_fix("MINUS");*/return MINUS;}
{STAR} {/*printf("STAR");*//*insert_fix("MUL");*/return STAR;}
{DIV} {/*printf("DIV");*//*insert_fix("DIV");*/return DIV;}
{AND} {/*printf("AND");*//*insert_fix("AND");*/return AND;}
{OR} {/*printf("OR");*//*insert_fix("OR");*/return OR;}
{DOT} {/*printf("DOT");*//*insert_fix("DOT");*/return DOT;}
{NOT} {/*printf("NOT");*//*insert_fix("NOT");*/return NOT;}
{LP} {/*printf("LP");*//*insert_fix("LP");*/return LP;}
{RP} {/*printf("RP");*//*insert_fix("RP");*/return RP;}
{LB} {/*printf("LB");*//*insert_fix("LB");*/return LB;}
{RB} {/*printf("RB");*//*insert_fix("RB");*/return RB;}
{LC} {/*printf("LC");*//*insert_fix("LC");*/return LC;}
{RC} {/*printf("RC");*//*insert_fix("RC");*/return RC;}
{TYPE} {/*printf("TYPE");*/ //strcpy(&yylval, &yytext); 
	/*if(strcmp(yytext,"int")==0){yylval = "int";}
	else if (strcmp(yytext,"float")==0){yylval = "float";}
	else{yylval = "int";}*/
	
	yylval = (YYSTYPE)symbolPoint; 
	strcpy(symbol+symbolPoint, yytext);
//	printf("*****%s\n",symbol+symbolPoint);
	symbolPoint += strlen(yytext);
	symbol[symbolPoint++] = '\0';
	//yylval = (YYSTYPE)symbol[1];
	//insert(yylval,"TYPE");
	return TYPE;
	}
{DINT} {/*printf("INT")*/;
	yylval = (YYSTYPE)atoi(yytext); /*insert((YYSTYPE)yylval,"INT");*/return INT;}
{HINT} {sscanf(yytext,"%x",(unsigned int*)&yylval); /*insert((YYSTYPE)yylval,"INT");*/return INT;}
{OINT} {sscanf(yytext,"%o", (unsigned int*)&yylval); /*insert((YYSTYPE)yylval,"INT");*/return INT;}
{FLOAT} {/*printf("FLOAT");*/  yylval = (YYSTYPE)atof(yytext);/* insert((YYSTYPE)yylval,"FLOAT");*/return FLOAT;}

{ID} {  /*printf("ID");*///strcpy(&yylval,yytext);
	
	yylval = (YYSTYPE)symbolPoint;
	strcpy(symbol+symbolPoint,yytext);
	symbolPoint += strlen(yytext);
	symbol[symbolPoint++] = '\0';
	/*insert(yylval,"ID");*/
	return ID;
	}

\"[^\"\n]*\" {
	if (yytext[yyleng -2] == '\\'){
		yyless(yyleng-1);
		yymore();
	}else{
		yylval = (YYSTYPE)symbolPoint;
		strcpy(symbol + symbolPoint, yytext);
		symbolPoint += strlen(yytext);
		symbol[symbolPoint++] = '\0';
		return WORDS;
	}
     }


(\t|\ )+ {;}
\n {yycolumn = 1;}
. {flag ++;printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%




