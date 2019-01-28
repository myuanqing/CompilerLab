#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compiler.h"
void yyrestart();

/*	basic: 
	int---1
	float---2	


*/
struct Type_ TYPE_INT_ = {
	kind : BASIC,
	.u.basic = 1
};
struct Type_ TYPE_FLOAT_ = {
	kind : BASIC,
	.u.basic = 2
};

Type TYPE_INT = &TYPE_INT_;
Type TYPE_FLOAT = &TYPE_FLOAT_;

int main(int argc, char**argv){
	if(argc <= 1)return 1;
	FILE* f = fopen(argv[1],"r");
	if(!f){
		perror( argv[1] );
		return 1;
	}
	yyrestart(f);
	//yydebug = 1;
	yyparse();
	//printNode();
	countnull = 0;
	declaration = NULL;
	if(flag == 0)
		semantic_analysis(stack[0]);
	//test();
	//while(yylex() != 0);
	//printf("here\n");
	return 0;
}
