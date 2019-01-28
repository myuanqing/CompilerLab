%{
	#include "lex.yy.c"
	#include <stdio.h>
	#include <string.h>
	#include "compiler.h"

	Node* root = NULL;
	Node* stack[1024];
	char symbol[2048]={0};
	int symbolPoint = 0;
	int stackCount = 0;
	//Node* buildTree();
	//void insert(YYSTYPE value, char* name, int pos);
	//void reduce(int i, char* name,int pos);
	int yyerror();
	//void printNode();
	int flag = 0;
	int myline = 0;
	//YYERROR_VERBOSE = 1;
	//yymsg = "hhh";
%}
%union {
	int type_int;
	float type_float;
	double type_double;
	char* type_string;
}

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%token <type_string>WORDS
%token VOID
%token VOLATILE
%token ENUM
%token STRUCT
%token UNION
%token IF
%token ELSE
%token GOTO
%token SWITCH
%token CASE
%token DO
%token WHILE
%token FOR
%token CONTINUE
%token BREAK
%token RETURN
%token DEFAULT
%token TYPEDEF
%token AUTO
%token REGISTER
%token EXTERN
%token STATIC
%token SEMI
%token COMMA
%token <type_string>RELOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token DOT
%token NOT
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token <type_string>TYPE
%token <type_int>INT 
%token <type_double>FLOAT
%token <type_string>ID
%left ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB
//%type  Exp 
%%

Program:			ExtDefList 			{reduce(1,"Program",*(int*)&@1);}			
			;

ExtDefList:		 	ExtDef 		ExtDefList	{reduce(2,"ExtDefList",*(int*)&@1);}
			|			{insert_fix("NULL");reduce(1,"ExtDefList",0);}	
			;

ExtDef:				Specifier	ExtDecList	SEMI{insert_fix("SEMI");}    {reduce(3,"ExtDef",*(int*)&@1);}
		        | 	Specifier 	SEMI		{insert_fix("SEMI");}{reduce(2,"ExtDef",*(int*)&@1);}
			|       Specifier	FunDec		CompSt		{reduce(3,"ExtDef",*(int*)&@1);}
			|	Specifier	FunDec		SEMI{insert_fix("SEMI");}    {reduce(3,"ExtDef",*(int*)&@1);}
		//	|	Specifier	ExtDecList error	{if (myline != yylineno){ printf("Wrong Definition\n"); myline = yylineno;} }SEMI	
			;

ExtDecList:			VarDec		{reduce(1,"ExtDecList",*(int*)&@1);}
			|	VarDec		COMMA{insert_fix("COMMA");}  ExtDecList {reduce(3,"ExtDecList",*(int*)&@1);}		
			;

							
Specifier:			TYPE{insert((YYSTYPE)$1,"TYPE");}		{reduce(1,"Specifier",*(int*)&@1);}
			|	StructSpecifier {reduce(1,"Specifier",*(int*)&@1);}
			;

StructSpecifier:		STRUCT R	 OptTag	    LC{insert_fix("LC");}	DefList	  RC{insert_fix("RC");}{reduce(5,"StructSpecifier",*(int*)&@1);}
			|	STRUCT R	  Tag           {reduce(2,"StructSpecifier",*(int*)&@1);}
			;
R:				{insert_fix("STRUCT");}
			;

OptTag:				ID  W {stack[stackCount-1]->value = (YYSTYPE)$1;}   {reduce(1,"OptTag",*(int*)&@1);}
			|      {insert_fix("NULL");reduce(1,"OptTag",0);}
			;

Tag:				ID  W {stack[stackCount-1]->value = (YYSTYPE)$1;}   {reduce(1,"Tag",*(int*)&@1);}
			;

VarDec:				ID W {stack[stackCount-1]->value = (YYSTYPE)$1;}    {reduce(1,"VarDec",*(int*)&@1);}
			|	VarDec	LB{insert_fix("LB");}	INT{insert((YYSTYPE)$4,"INT");}  RB{insert_fix("RB");}{reduce(4,"VarDec",*(int*)&@1);}
			;

FunDec:				ID W LP{insert_fix("LP");}  VarList RP {insert_fix("RP");}{stack[stackCount-4]->value =(YYSTYPE)$1;} { reduce(4,"FunDec",*(int*)&@1);}
			|	ID W LP{insert_fix("LP");} RP{insert_fix("RP");} {stack[stackCount-3]->value =(YYSTYPE)$1;} {reduce(3,"FunDec",*(int*)&@1);}			
			;				
		


VarList:			ParamDec COMMA{insert_fix("COMMA");} VarList  {reduce(3,"VarList",*(int*)&@1);}
			|	ParamDec {reduce(1,"VarList",*(int*)&@1);}
			;

ParamDec:			Specifier VarDec {reduce(2,"ParamDec",*(int*)&@1);}
			;

CompSt:				LC{insert_fix("LC");} DefList StmtList RC{insert_fix("RC");} {reduce(4,"CompSt",*(int*)&@1);}		
			|	LC error{if (myline != yylineno){ printf("Wrong CompSt\n"); myline = yylineno;}} DefList StmtList RC
			//|	LC DefList error RC{printf("Wrong StmtList");}
			;

StmtList:			Stmt StmtList  {reduce(2,"StmtList",*(int*)&@1);}
			|	{insert_fix("NULL");reduce(1,"StmtList",0);}		
			;

Stmt:				Exp SEMI {insert_fix("SEMI");reduce(2,"Stmt",*(int*)&@1);}
			|	CompSt {reduce(1,"Stmt",*(int*)&@1);}
			|	RETURN{insert_fix("RETURN");} Exp SEMI {insert_fix("SEMI");}{reduce(3,"Stmt",*(int*)&@1);}
			| 	IF  S LP U  Exp RP V Stmt  {reduce(5,"Stmt",*(int*)&@1);}  %prec LOWER_THAN_ELSE
			|	IF  S LP U  Exp RP V Stmt ELSE{insert_fix("ELSE");} Stmt{reduce(7,"Stmt",*(int*)&@1);}
			|	IF  S LP U Exp RP V error{if (myline != yylineno){ printf("Missing \";\".\n"); myline = yylineno;}} ELSE Stmt
			|	WHILE{insert_fix("WHILE");} LP{insert_fix("LP");} Exp RP{insert_fix("RP");} Stmt  {reduce(5,"Stmt",*(int*)&@1);}
		//	|	RETURN{insert_fix("RETURN");}
		//	|	error {if (myline != yylineno){ printf("Wrong Statement.\n"); myline = yylineno;} } SEMI	
			;	

S:				{insert_fix("IF");}
			; 

U:				{insert_fix("LP");}			
			;

V:				{insert_fix("RP");}
			;		

DefList:			Def DefList {reduce(2,"DefList",*(int*)&@1);}
			|	{insert_fix("NULL");reduce(1,"DefList",0);}
		//	|	 error SEMI{printf("Error Definition.\n");}
			;

Def:				Specifier DecList SEMI  {insert_fix("SEMI");}{reduce(3,"Def",*(int*)&@1);}	
			|	Specifier DecList error {if (myline != yylineno){ printf("Missing \";\". \n"); myline = yylineno;} }SEMI 
			;

DecList:			Dec  {reduce(1,"DecList",*(int*)&@1);}
			|	Dec COMMA{insert_fix("COMMA");} DecList {reduce(3,"DecList",*(int*)&@1);}						
			;

Dec:				VarDec {reduce(1, "Dec",*(int*)&@1);}
			|	VarDec ASSIGNOP{insert_fix("ASSIGNOP");} Exp {reduce(3,"Dec",*(int*)&@1);}
			;

Exp:
				Exp		ASSIGNOP{insert_fix("ASSIGNOP");}		Exp			{reduce(3,"Exp",*(int*)&@1);}
			|	Exp 		OR{insert_fix("OR");}   	Exp		  	{reduce(3,"Exp",*(int*)&@1);}
			| 	Exp 		AND{insert_fix("AND");}   	Exp		  	{reduce(3,"Exp",*(int*)&@1);}
			|	Exp		RELOP{insert((YYSTYPE)$2,"RELOP");}		Exp	{reduce(3,"Exp",*(int*)&@1);}
			| 	Exp 		PLUS{insert_fix("PLUS");}  	Exp			{reduce(3,"Exp",*(int*)&@1);}
			|	Exp 		MINUS{insert_fix("MINUS");}	Exp		  	{reduce(3,"Exp",*(int*)&@1);}
			|	Exp		STAR{insert_fix("MUL");} 	Exp		  	{reduce(3,"Exp",*(int*)&@1);}
			| 	Exp             DIV{insert_fix("DIV");}   	Exp            		{reduce(3,"Exp",*(int*)&@1);}
			|	LP{insert_fix("LP");}   		Exp             	RP{insert_fix("RP");} 		{reduce(3,"Exp",*(int*)&@1);}
			|	MINUS{insert_fix("MINUS");} Exp              	{reduce(2,"Exp",*(int*)&@1);}
			|	NOT{insert_fix("NOT");}  		Exp              	{reduce(2,"Exp",*(int*)&@1);}
			|	ID W   LP {insert_fix("LP");}   	Args          		RP{insert_fix("RP");}{stack[stackCount-4]->value =(YYSTYPE) $1;}  {reduce(4,"Exp",*(int*)&@1);}
			|	ID W   LP{insert_fix("LP");}   	RP{insert_fix("RP");}   {stack[stackCount-3]->value = (YYSTYPE)$1;}{reduce(3,"Exp",*(int*)&@1);}
			|	Exp             LB{insert_fix("LB");}    	Exp           		RB{insert_fix("RB");}   {reduce(4,"Exp",*(int*)&@1);}
			|	Exp 		DOT{insert_fix("DOT");}   	ID {insert((YYSTYPE)$4,"ID");}		{reduce(3,"Exp",*(int*)&@1);} 			
			|	ID {insert((YYSTYPE)$1,"ID");}  {reduce(1,"Exp",*(int*)&@1);}
			|	INT  {insert((YYSTYPE)$1,"INT");}{reduce(1,"Exp",*(int*)&@1);}   
			|	FLOAT{insert((YYSTYPE)$1,"FLOAT");}{reduce(1,"Exp",*(int*)&@1);}			
			|	WORDS{insert((YYSTYPE)$1,"WORDS");}
			|	Exp LB error {if (myline != yylineno){ printf("Missing \"]\".\n"); myline = yylineno;} } RB
			//|	Exp error RP {printf("Wrong expression.\n");}
			;

Args:				Exp          	COMMA {insert_fix("COMMA");}  	Args          		{reduce(3,"Args",*(int*)&@1);}
			|	Exp            	{reduce(1,"Args",*(int*)&@1);}
			;				

W:				{insert_fix("ID");}
			;
%%

void insert_fix(char* name){
	if(flag == 0){
	Node* cur  = (Node*)malloc(sizeof(Node));
	cur->siblings = NULL;
	cur->sons = NULL;
	cur->posBegin = 0;
	strcpy(cur->name, name);
	stack[stackCount++] = cur;
	}
	//printf("&\t");
}

void insert(YYSTYPE value, char*name){
	if(flag == 0){
	Node* cur  = (Node*)malloc(sizeof(Node));
	cur->value = value;
	cur->siblings = NULL;
	cur->sons = NULL;
	cur->posBegin = 0;
	strcpy(cur->name, name);
	stack[stackCount++] = cur;
	}
	//printf("&\t");
}


void reduce(int count, char* name, int pos){
	if(flag == 0){
	Node* cur = (Node*)malloc(sizeof(Node));
	cur->sons = stack[stackCount-count];
	cur->siblings = NULL;
	cur->posBegin = pos;
	strcpy(cur->name,name);
	Node* first = stack[stackCount-count];
	for(int i = 1;i < count; ++i){
		first->siblings = stack[stackCount-count+i];
		first = first->siblings;			
	}	
	stackCount -= count;
	stack[stackCount++] = cur;
	//printf("&\t");
	}
}

void front(Node* p, int c){
	
	while(p!=NULL){
		if( p->sons == NULL || strcmp(p->sons->name,"NULL" )!=0 || p->sons->siblings != NULL){		
			for(int i = 0;i < c;++i){
				printf("  ");			
			}
			printf("%s",p->name);
			if(p->posBegin != 0){
				printf(" (%d)",p->posBegin);
			}
			if(strcmp(p->name,"ID")==0){
				printf(": %s\n",(char*)(symbol+*(int*)&p->value));	
			}
			else if(strcmp(p->name,"INT")==0){
				printf(": %d\n",*(int*)&p->value);
			}	
			else if(strcmp(p->name,"FLOAT")==0){
				printf(": %lf\n",*(double*)&p->value);
			}
			else if(strcmp(p->name,"TYPE")==0){
				printf(": %s\n",(char*)(symbol+*(int*)&p->value));
			}
			else if(strcmp(p->name,"RELOP")==0){
				printf(": %s\n",(char*)(symbol+*(int*)&p->value));
			}
			else if(strcmp(p->name,"WORDS")==0){
				printf(": %s\n",(char*)(symbol+*(int*)&p->value));			
			}
			else{
				printf("\n");	
			}
			front(p->sons, c+1);
		}	
		p = p->siblings;
	}

}


int yyerror(char* msg ){
	flag ++;
	printf("Error type B at Line %d: ", yylineno);
	return 0;
}


