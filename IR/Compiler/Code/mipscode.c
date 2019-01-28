#include "compiler.h"
#include<assert.h>
#include<math.h>
int opCmp(Operand a, Operand b){
	if(a->kind!=b->kind)
		return 1;
	switch(a->kind){
		case VARIABLE:
		case TVARIABLE:
		case INT_CONSTANT:
		case ADDRESS:
		case LABEL:
			return (a->u.var_no != b->u.var_no);
		
		case FLOAT_CONSTANT:
			return (a->u.float_value != b->u.float_value);
		case NAME :
			return strcmp(a->u.name,b->u.name);
	}
}




RegDes regList[REGNUM];
VarDes* varList = NULL;
int offset = FSIZE;


void clearVarList(){
	while(varList!=NULL){
		VarDes* tem = varList;
		varList = varList->next;
		free(tem);
	}
}

void addVarToList(VarDes* p){
	assert(p!=NULL);
	p->next = varList;
	varList = p;
}

void stVar(VarDes* var, FILE* fp){
	char stCode[64];
	if(var->op->kind == VARIABLE || var->op->kind == TVARIABLE || var->op->kind == ADDRESS){
		memset(stCode, 0, sizeof(stCode));
		sprintf(stCode, "subu $v1, $fp, %d\n", var->offset);
		fputs(stCode, fp);
		memset(stCode, 0, sizeof(stCode));
		sprintf(stCode, "sw $%s, 0($v1)\n", regList[var->regNo].name);
	}
	else
		assert(0);
	regList[var->regNo].old = 0;
	regList[var->regNo].var = NULL;
	var->regNo = -1;
	fputs(stCode, fp);
}

void ldVar(VarDes* var, FILE* fp){
	char ldCode[64];
	if(var->op->kind == VARIABLE || var->op->kind == TVARIABLE || var->op->kind == ADDRESS){
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "subu $v1, $fp, %d\n", var->offset);
		fputs(ldCode, fp);
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "lw $%s, 0($v1)\n", regList[var->regNo].name);
	}
	else if(var->op->kind == INT_CONSTANT){
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "li $%s, %d\n", regList[var->regNo].name, var->op->u.int_value);
	}
	else{
		assert(0);
	}
	fputs(ldCode, fp);
}


void initAllRegDes(){
	int i;
	for(i=0; i<REGNUM; i++){
		regList[i].old = 0;
		regList[i].var = NULL;
	}
	strcpy(regList[0].name, "t0");
	strcpy(regList[1].name, "t1");
	strcpy(regList[2].name, "t2");
	strcpy(regList[3].name, "t3");
	strcpy(regList[4].name, "t4");
	strcpy(regList[5].name, "t5");
	strcpy(regList[6].name, "t6");
	strcpy(regList[7].name, "t7");
	strcpy(regList[8].name, "t8");
	strcpy(regList[9].name, "t9");
	strcpy(regList[10].name, "s0");
	strcpy(regList[11].name, "s1");
	strcpy(regList[12].name, "s2");
	strcpy(regList[13].name, "s3");
	strcpy(regList[14].name, "s4");
	strcpy(regList[15].name, "s5");
	strcpy(regList[16].name, "s6");
	strcpy(regList[17].name, "s7");

}

void rstAllReg(){
	int i;
	for(i=0; i<REGNUM; i++){
		regList[i].old = 0;
		regList[i].var = NULL;
	}
}

void freeOneReg(int regNo){
	VarDes* tem = varList;
	while(tem!=NULL){
		if(tem->regNo == regNo)
			tem->regNo = -1;
		tem = tem->next;
	}
	regList[regNo].old = 0;
	regList[regNo].var = NULL;
}

int allocateRegForOp(Operand op, FILE* fp){
	VarDes* var = varList;
	while(var!=NULL){
		if(opCmp(var->op, op)==0)
			break;
		var = var->next;
	}
	
	int isFirstUsed = 0;
	if(var == NULL){
		isFirstUsed = 1;
		var = malloc(sizeof(VarDes));
		memset(var, 0, sizeof(VarDes));
		var->op = op;
		var->regNo = -1;
		if(var->op->kind == VARIABLE || var->op->kind == TVARIABLE|| var->op->kind == ADDRESS ){
			offset += 4;
			var->offset = offset;
		}
		if(var->op->kind != INT_CONSTANT)
			addVarToList(var);
	}
	
	if(var->regNo < 0){
		int regNo = getReg(fp);
		var->regNo = regNo;
		regList[regNo].var = var;
		
		if(var->op->kind == INT_CONSTANT||(isFirstUsed==0 && var->offset>=0)){
			ldVar(var, fp);
		}
	}
	regList[var->regNo].old = 0;
	return var->regNo;
}

int getReg(FILE* fp){
	int i;
	int freedReg = -1;
	for(i=0; i<REGNUM; i++){
		if(regList[i].var!=NULL)
			regList[i].old++;
		else
			freedReg = i;
	}
	if(freedReg>0)
		return freedReg;
	int oldestReg = -1;
	int oldest = -1;
	for(i=0; i<REGNUM; i++){
		if(regList[i].old>oldest){
			oldest = regList[i].old;
			oldestReg = i;
		}
	}
	
	VarDes* var = varList;
	while(var!=NULL){
		if(var !=regList[oldestReg].var)
			var = var->next;
		else
			break;
	}
	if(var!=NULL){
		assert(oldestReg==var->regNo);
		stVar(var, fp);
	}
	else
		assert(0);
	return oldestReg;
}

void fwriteAllObjCode(char* fileName){
	initAllRegDes();
	FILE* fp = fopen(fileName, "w");
	if(fp==NULL){
		printf("ERROR: open file \"%s\" fail!",fileName);
		return;
	}
	fputs(".data\n", fp);
	fputs("_prompt: .asciiz \"Enter an integer:\"\n", fp);
	fputs("_ret: .asciiz \"\\n\"\n", fp);
	fputs(".globl main\n", fp);
	fputs(".text\n", fp);
	fputs("\n", fp);
	fputs("read:\n", fp);
	fputs("subu $sp, $sp, 8\n", fp); 
	fputs("sw $ra, 4($sp)\n", fp);
	fputs("sw $fp, 0($sp)\n", fp);
	fputs("addi $fp, $sp, 8\n", fp);
	fputs("li $v0, 4\n", fp);
	fputs("la $a0, _prompt\n", fp);
	fputs("syscall\n", fp);
	fputs("li $v0, 5\n", fp);
	fputs("syscall\n", fp);
	fputs("subu $sp, $fp, 8\n", fp);
	fputs("lw $ra, 4($sp)\n", fp);
	fputs("lw $fp, 0($sp)\n", fp);
	fputs("jr $ra\n", fp);
	fputs("\n", fp);
	fputs("write:\n", fp);
	fputs("subu $sp, $sp, 8\n", fp); 
	fputs("sw $ra, 4($sp)\n", fp);
	fputs("sw $fp, 0($sp)\n", fp);
	fputs("addi $fp, $sp, 8\n", fp);
	fputs("li $v0, 1\n", fp);
	fputs("syscall\n", fp);
	fputs("li $v0, 4\n", fp);
	fputs("la $a0, _ret\n", fp);
	fputs("syscall\n", fp);
	fputs("subu $sp, $fp, 8\n", fp);
	fputs("lw $ra, 4($sp)\n", fp);
	fputs("lw $fp, 0($sp)\n", fp);
	fputs("move $v0, $0\n", fp);
	fputs("jr $ra\n", fp);
	
	InterCodes ir = IRHead;
	while(ir!=NULL){
		fwriteOneObjCode(ir, fp);
		ir = ir->next;
	}
	fclose(fp);
}

int argc = 0;

void fwriteOneObjCode(InterCodes ir, FILE* fp){
	switch(ir->code->kind){
		case LABEL_LINE:{
			VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "label%d:\n", ir->code->u.label->u.lab_no);
			fputs(str, fp);
			break;
		}
		case FUNCTIONL:{
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "\n%s:\n", ir->code->u.label->u.name);
			fputs(str, fp);
			
			//Prologue
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $sp, $sp, %d\n", FSIZE);
			fputs(str, fp);
			fputs("sw $ra, 4($sp)\n", fp);
			fputs("sw $fp, 0($sp)\n", fp);
			fputs("addi $fp, $sp, 8\n", fp);
			clearVarList();
			rstAllReg();
			offset = FSIZE;
			
			ir = ir->next;
			int i = 0;
			while(ir->code->kind == PARAML ){
				int paramReg = allocateRegForOp(ir->code->u.label, fp);
				memset(str, 0, sizeof(str));
				if(i<4){
					sprintf(str, "move $%s, $a%d\n", regList[paramReg].name, i);
				}
				else{
					sprintf(str, "lw $%s, %d($sp)\n", regList[paramReg].name, FSIZE+(i-4)*4);
				}
				fputs(str, fp);
				i++;
				ir = ir->next;
			}
			break;
		}
		case ASSIGN:{
			Operand leftOp = ir->code->u.assign.left;
			Operand rightOp = ir->code->u.assign.right;
			int leftReg = allocateRegForOp(leftOp, fp);
			int rightReg = allocateRegForOp(rightOp, fp);
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "move $%s, $%s\n", regList[leftReg].name, regList[rightReg].name);
			fputs(str, fp);
			if(leftOp->kind == ADDRESS){
				sprintf(str, "sw $%s, -%d($fp)\n", regList[leftReg].name, regList[leftReg].var->offset);
				fputs(str, fp);
			}
			break;
		}
		case ADD:
		case SUB:
		case MUL:
		case DIVI:{
			char op[4];
			memset(op, 0, sizeof(op));
			switch(ir->code->kind){
				case ADD:
					strcpy(op, "add");
					break;
				case SUB:
					strcpy(op, "sub");
					break;
				case MUL:
					strcpy(op, "mul");
					break;
				case DIVI:
					strcpy(op, "div");
					break;
			}
			int op1Reg = allocateRegForOp(ir->code->u.binop.op1, fp);
			int op2Reg = allocateRegForOp(ir->code->u.binop.op2, fp);
			int resultReg = allocateRegForOp(ir->code->u.binop.result, fp);
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "%s $%s, $%s, $%s\n", op, regList[resultReg].name, regList[op1Reg].name, regList[op2Reg].name);
			fputs(str, fp);
			if(ir->code->u.binop.result->kind == ADDRESS)
				sprintf(str, "sw $%s, -%d($fp)\n", regList[resultReg].name ,  regList[resultReg].var->offset);
			break;
		}
		case GOTOL:{
			VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "j label%d\n", ir->code->u.label->u.lab_no);
			fputs(str, fp);
			break;
		}
		case COND_GOTO:{
			VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}
			char op[4];
			if(strcmp(ir->code->u.condop.op,"==")==0)
				strcpy(op,"beq");
			else if(strcmp(ir->code->u.condop.op,"!=")==0)
				strcpy(op,"bne");
			else if(strcmp(ir->code->u.condop.op,">")==0)
				strcpy(op,"bgt");
			else if(strcmp(ir->code->u.condop.op,"<")==0)
				strcpy(op,"blt");
			else if(strcmp(ir->code->u.condop.op,">=")==0)
				strcpy(op,"bge");
			else if(strcmp(ir->code->u.condop.op,"<=")==0)
				strcpy(op,"ble");
			else
				assert(0);
			int xReg = allocateRegForOp(ir->code->u.condop.t1, fp);
			int yReg = allocateRegForOp(ir->code->u.condop.t2, fp);
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "%s $%s, $%s, label%d\n", op, regList[xReg].name, regList[yReg].name, ir->code->u.condop.lb->u.lab_no);
			fputs(str, fp);
			break;
		}
		case RETURNL:{
			Operand retOp = ir->code->u.label;
			int retReg;
			retReg = allocateRegForOp(retOp, fp);
			//Epilogue
			fputs("subu $sp, $fp, 8\n", fp);
			fputs("lw $ra, 4($sp)\n", fp);
			fputs("lw $fp, 0($sp)\n", fp);
			char str[64];
			memset(str, 0, sizeof(str));
			if(retOp->kind==VARIABLE || retOp->kind==TVARIABLE)
				sprintf(str, "move $v0, $%s\n", regList[retReg].name);
			else if(retOp->kind==INT_CONSTANT)
				sprintf(str, "li $v0, %d\n", retOp->u.int_value);
			fputs(str, fp);
			fputs("jr $ra\n", fp);
			break;
		}
		case DEC:{
			int regNo = allocateRegForOp(ir->code->u.dec.v, fp);
		
			offset += ir->code->u.dec.len;
		
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $%s, $fp, %d\n", regList[regNo].name, offset);
			fputs(str, fp);
			
			break;
		}
		case ARGSL:{
			argc++;
			break;
		}
		case CALLL:{
			VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}
			InterCodes irPre = ir;
			char str[64];
			if(argc>4){
				memset(str, 0, sizeof(str));
				sprintf(str, "subu $sp, $fp, %d\n", offset+(argc-4)*4);
				fputs(str, fp);
			}
			else{
				memset(str, 0, sizeof(str));
				sprintf(str, "subu $sp, $fp, %d\n", offset);
				fputs(str, fp);
			}
			int i;
			for(i=0; i<argc; i++){
				irPre = irPre->prev;
				int argReg = allocateRegForOp(irPre->code->u.label, fp);
				memset(str, 0, sizeof(str));
				if(i<4){
					sprintf(str, "move $a%d, $%s\n", i, regList[argReg].name);
				}
				else{
					sprintf(str, "sw $%s, %d($sp)\n", regList[argReg].name, (i-4)*4);
				}
				fputs(str, fp);
			}
			memset(str, 0, sizeof(str));
			sprintf(str, "jal %s\n", ir->code->u.assign.right->u.name);
			fputs(str, fp);
			argc = 0;
			int retReg = allocateRegForOp(ir->code->u.assign.left, fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "move $%s, $v0\n", regList[retReg].name);
			fputs(str, fp);
			break;
		}
		case PARAML:{
			break;
		}
		case READL:{
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $sp, $fp, %d\n", offset);
			fputs(str, fp);
			fputs("jal read\n", fp);
			int retReg = allocateRegForOp(ir->code->u.label, fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "move $%s, $v0\n", regList[retReg].name);
			fputs(str, fp);
			break;
		}
		case WRITEL:{
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $sp, $fp, %d\n", offset);
			fputs(str, fp);
			int paramReg = allocateRegForOp(ir->code->u.label, fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "move $a0, $%s\n", regList[paramReg].name);
			fputs(str, fp);
			fputs("jal write\n", fp);
			break;
		}
		default:
			assert(0);
	}
}
