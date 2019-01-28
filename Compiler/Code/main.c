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
	yyparse();
	countnull = 0;
	temp_no = 1;
	var_no = 1;
	lab_no = 1;
	IRHead = NULL;
	IRTail = NULL;
	place = NULL;
	label_true = NULL;
	label_false = NULL;
	irflag = 1;
	called = 0;
	smflag = 1;
	declaration = NULL;
	ARGS = NULL;
	add_read();
	add_write();

	if(flag == 0)
		semantic_analysis(stack[0]);

	if(smflag == 0)
		
		deleteAll();
	else{
		//printInterCode();
		fwriteAllObjCode(argv[2]);
	}
	return 0;
}
