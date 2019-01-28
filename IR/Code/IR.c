#include"compiler.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

Operand new_temp(){
	Operand op = (Operand)malloc(sizeof(Operand_));
	op->kind = TVARIABLE;
	op->u.temp_no = temp_no;
	temp_no ++;
	return op;
}
Operand new_int_constant(int i){
	Operand op = (Operand)malloc(sizeof(Operand_));
	op->kind = INT_CONSTANT;
	op->u.int_value = i;
	return op;
}
Operand new_label(){
	Operand op = (Operand)malloc(sizeof(Operand_));
	op->kind = LABEL;
	op->u.lab_no = lab_no;
	lab_no++;
	return op;
}
Operand get_variable(int vn ){
	Operand op = (Operand)malloc(sizeof(Operand_));
	op->kind = VARIABLE;
	op->u.var_no = vn;
	return op;
}

InterCode new_label_intercode(Operand op){
	InterCode ic = (InterCode)malloc(sizeof(InterCode_));
	ic->kind = LABEL_LINE;
	ic->u.label = op;
	return ic;
}
InterCode new_goto_intercode(Operand op){
	InterCode ic = (InterCode)malloc(sizeof(InterCode_));
	ic->kind = GOTOL;
	ic->u.label = op;
	return ic;
}
InterCode new_assign_intercode(Operand op1, Operand op2){
	InterCode ic = (InterCode)malloc(sizeof(InterCode_));
	ic->kind = ASSIGN;
	ic->u.assign.left = op1;
	ic->u.assign.right = op2;
	return ic;
}

InterCode new_return_intercode(Operand op){
	InterCode ic = (InterCode)malloc(sizeof(InterCode_));
	ic->kind = RETURNL;
	ic->u.label = op;
	return ic;
}
void not_called_before(Operand plc){
	Operand lb1 = new_label();
	Operand lb2 = new_label();		
	label_true = lb1;
	label_false = lb2;
	Operand zero = new_int_constant(0);
	InterCode ic = new_assign_intercode(plc,zero);
	insert_ir(ic);
}


void not_called_after(Operand lb1, Operand lb2, Operand plc){
	Operand lb1_f = copy_operand(lb1);
	Operand lb2_f = copy_operand(lb2);		
	InterCode ic3 = new_label_intercode(lb1_f);
	insert_ir(ic3);
	Operand place_ff = copy_operand(plc);
	Operand one = new_int_constant(1);
	InterCode ic4 = new_assign_intercode(place_ff, one);
 	insert_ir(ic4);
	InterCode ic5 = new_label_intercode(lb2_f);
	insert_ir(ic5);
}

void copy(Operand op1, Operand op2){
	memcpy((void*)op1,(void*)op2,sizeof(Operand_));
}


Operand copy_operand(Operand op){
	Operand op1 = (Operand)malloc(sizeof(Operand_));
	memcpy((void*)op1, (void*)op, sizeof(Operand_));
	return op1;
}
void insert_ir(InterCode ic){
	InterCodes ics = (InterCodes)malloc(sizeof(InterCodes_));
	ics->code = ic;
	if(IRHead == NULL){
		IRHead = ics;
		ics->prev = NULL;
		ics->next = NULL;
	}else{
		IRTail->next = ics;
		ics->prev = IRTail;
		ics->next = NULL;
	}
	IRTail = ics;

}



void abinop(int ag){
		Operand result = IRHead->code->u.binop.result;
		switch(result->kind){
		case VARIABLE:{
			if(result->u.var_no < 0){
				printf("&v%d", -result->u.var_no);
			}
			else{
				printf("v%d", result->u.var_no);
			}
		}break;
		case TVARIABLE:{
			printf("t%d", result->u.temp_no);
		}break;
		default:{
		}
		}
		free(result);

		printf(" :=");

		Operand op1 = IRHead->code->u.binop.op1;
		switch(op1->kind){
		case VARIABLE:{
			if(op1->u.var_no < 0){
				printf(" &v%d", -op1->u.var_no);
			}
			else{
				printf(" v%d", op1->u.var_no);
			}
		}break;
		case TVARIABLE:{
			printf(" t%d", op1->u.temp_no);
		}break;
		case INT_CONSTANT:{
			printf(" #%d",op1->u.int_value);	
		}break;
		case FLOAT_CONSTANT:{
			printf(" #%lf",op1->u.float_value);
		}break;
		default:{
		}
		}
		free(op1);

		switch(ag){
		case 1:{		
			printf(" +");
		}break;
		case 2:{
			printf(" -");
		}break;
		case 3:{
			printf(" *");		
		}break;
		case 4:{
			printf(" /");
		}break;
		default:{
		}
		}
		Operand op2 = IRHead->code->u.binop.op2;			
		switch(op2->kind){
		case VARIABLE:{
			if(op2->u.var_no < 0){
				printf(" &v%d", -op2->u.var_no);
			}
			else{
				printf(" v%d", op2->u.var_no);
			}
		}break;
		case TVARIABLE:{
			printf(" t%d", op2->u.temp_no);
		}break;
		case INT_CONSTANT:{
			printf(" #%d",op2->u.int_value);	
		}break;
		case FLOAT_CONSTANT:{
			printf(" #%lf",op2->u.float_value);
		}break;
		
		default:{
		}
		}
		free(op2);
		free(IRHead->code);

}
void printInterCode(){
	while(IRHead != NULL){
		switch(IRHead->code->kind){
		case ASSIGN:{
			Operand right = IRHead->code->u.assign.right;
			Operand left = IRHead->code->u.assign.left;
			switch(left->kind){
			case VARIABLE:{
				if(left->u.var_no < 0){
					printf("&v%d", -left->u.var_no);
				}
				else{
					printf("v%d", left->u.var_no);
				}			
			}break;
			case TVARIABLE:{
				printf("t%d", left->u.temp_no);
			}break;
			case ADDRESS:{
				printf("*t%d",left->u.temp_no);
			}break;
			default:{
			}
			}
			free(left);
		
			printf(" :=");

			switch(right->kind){
			case INT_CONSTANT:{
				printf(" #%d",right->u.int_value);	
			}break;
			case FLOAT_CONSTANT:{
				printf(" #%lf",right->u.float_value);
			}break;
			case VARIABLE:{
				if(right->u.var_no < 0){
					printf(" &v%d", -right->u.var_no);
				}
				else{
					printf(" v%d",right->u.var_no);
				}			
			}break;
			case TVARIABLE:{
				printf(" t%d",right->u.temp_no);
			}break;
			case ADDRESS:{
				printf(" *t%d",right->u.temp_no);
			}break;
			default:{

			}
			}
			free(right);
			free(IRHead->code);

		}break;
		case ADD:{
			abinop(1);
		}break;
		case SUB:{
			abinop(2);
		}break;
		case MUL:{
			abinop(3);
		}break;
		case DIVI:{
			abinop(4);
		}break;
		case RETURNL:{
			printf("RETURN t%d",IRHead->code->u.label->u.temp_no);
			free(IRHead->code->u.label);
			free(IRHead->code);
		}break;
		case COND_GOTO:{
			printf("IF");
			Operand t1 = IRHead->code->u.condop.t1;
			Operand t2 = IRHead->code->u.condop.t2;
			switch(t1->kind){
			case INT_CONSTANT:{
				printf(" #%d",t1->u.int_value);	
			}break;
			case FLOAT_CONSTANT:{
				printf(" #%lf",t1->u.float_value);
			}break;
			case VARIABLE:{
				if(t1->u.var_no < 0){
					printf(" &v%d", -t1->u.var_no);
				}else{
					printf(" v%d",t1->u.var_no);
				}
			}break;
			case TVARIABLE:{
				printf(" t%d",t1->u.temp_no);
			}break;
			default:{

			}
			}
			free(t1);
			printf(" %s",IRHead->code->u.condop.op);

			switch(t2->kind){
			case INT_CONSTANT:{
				printf(" #%d",t2->u.int_value);	
			}break;
			case FLOAT_CONSTANT:{
				printf(" #%lf",t2->u.float_value);
			}break;
			case VARIABLE:{
				if(t2->u.var_no < 0){
					printf(" &v%d", -t2->u.var_no);
				}else{
					printf(" v%d",t2->u.var_no);
				}
			}break;
			case TVARIABLE:{
				printf(" t%d",t2->u.temp_no);
			}break;
			default:{

			}
			}
			free(t2);
			printf(" GOTO label");
			printf("%d", IRHead->code->u.condop.lb->u.lab_no);
			free(IRHead->code->u.condop.lb);			
			free(IRHead->code);
		}break;
		case GOTOL:{
			printf("GOTO label%d", IRHead->code->u.label->u.lab_no);
			free(IRHead->code->u.label);
			free(IRHead->code);
		}break;
		case LABEL_LINE:{
			printf("LABEL label%d :", IRHead->code->u.label->u.lab_no);
			free(IRHead->code->u.label);
			free(IRHead->code);
		}break;
		case READL:{
			printf("READ");
			switch(IRHead->code->u.label->kind){
			case VARIABLE:{
				if(IRHead->code->u.label->u.var_no < 0){
					printf(" &v%d", -IRHead->code->u.label->u.var_no);
				}else{
					printf(" v%d",IRHead->code->u.label->u.var_no);
				}
			}break;
			case TVARIABLE:{
				printf(" t%d",IRHead->code->u.label->u.temp_no);
			}break;
			default:{

			}
			}
			free(IRHead->code->u.label);
			free(IRHead->code);			
		}break;
		case CALLL:{
			Operand right = IRHead->code->u.assign.right;
			Operand left = IRHead->code->u.assign.left;
			switch(left->kind){
			case VARIABLE:{
				if(left->u.var_no < 0){
					printf("&v%d", -left->u.var_no);
				}
				else{
				printf("v%d", left->u.var_no);
				}
			}break;
			case TVARIABLE:{
				printf("t%d", left->u.temp_no);
			}break;
			default:{
			}
			}
			free(left);
		
			printf(" := CALL %s", right->u.name);
			free(right);
			free(IRHead->code);			
			
		}break;
		case ARGSL:{
			Operand ar = IRHead->code->u.label;
			printf("ARG t%d",ar->u.temp_no);
			free(ar);
			free(IRHead->code);
		}break;
		case WRITEL:{
			printf("WRITE t%d", IRHead->code->u.label->u.temp_no);
			free(IRHead->code->u.label);
			free(IRHead->code);
		}break;
		case FUNCTIONL:{
			printf("FUNCTION %s :", IRHead->code->u.label->u.name);
			free(IRHead->code->u.label);
			free(IRHead->code);
		}break;
		case PARAML:{
			Operand par = IRHead->code->u.label;
			printf("PARAM v%d",par->u.var_no);
			free(par);
			free(IRHead->code);
		}break;
		case DEC:{
			Operand p = IRHead->code->u.dec.v;
			printf("DEC v%d %d",p->u.var_no, IRHead->code->u.dec.len);
			free(p);
			free(IRHead->code);
		}break;
		default:{

		}
		} 
		printf("\n");
		InterCodes tp = IRHead;
		//printf("here\n");
		IRHead = IRHead->next;
		//printf("here\n");
		//IRHead->prev = NULL;
		free(tp);
		//printf("here\n");
	}
}



void add_read(){
	Type tp = (Type)malloc(sizeof(struct Type_));
	tp->kind = FUNCTION;
	tp->u.declaration.function = (ParaList)malloc(sizeof(struct ParaList_));
	tp->u.declaration.function->type = TYPE_INT;	
	tp->u.declaration.function->tail = NULL;
	tp->u.declaration.ftp = (FuncType)malloc(sizeof(struct FuncType_));
	tp->u.declaration.ftp->tag = 1;
	tp->u.declaration.ftp->pointer = NULL;
	Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
	sprintf(tp_en->name, "%d", countnull);
	countnull++;
	tp_en->type = tp;
	tp_en->field = 0;
	add_type_entry(tp_en, 0);

	Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
	sy_en->no = var_no;
	var_no++;
	strcpy(sy_en->name, "read" );
	sy_en->field = 0;
	sy_en->type = tp;			
	tp->u.declaration.ftp->tag = 1;
	add_symbol_entry(sy_en,0);
}

void add_write(){
	Type tp = (Type)malloc(sizeof(struct Type_));
	tp->kind = FUNCTION;
	tp->u.declaration.function = (ParaList)malloc(sizeof(struct ParaList_));
	tp->u.declaration.function->type = TYPE_INT;	
	tp->u.declaration.function->tail = (ParaList)malloc(sizeof(struct ParaList_));
	tp->u.declaration.function->tail->type = TYPE_INT;
	tp->u.declaration.function->tail->tail = NULL;
	tp->u.declaration.ftp = (FuncType)malloc(sizeof(struct FuncType_));
	tp->u.declaration.ftp->tag = 1;
	tp->u.declaration.ftp->pointer = NULL;
	Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
	sprintf(tp_en->name, "%d", countnull);
	countnull++;
	tp_en->type = tp;
	tp_en->field = cur_field;
	add_type_entry(tp_en, cur_field);

	Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
	sy_en->no = var_no;
	var_no++;
	strcpy(sy_en->name, "write" );
	sy_en->field = 0;
	sy_en->type = tp;			
	tp->u.declaration.ftp->tag = 1;
	add_symbol_entry(sy_en,0);
}	


void add_arglist(Operand op){
	ARGLIST ar = (ARGLIST)malloc(sizeof(ARGLIST_));
	ar->op = op;
	ar->next = ARGS;
	ARGS = ar;
}

void insert_arglist(){
	while(ARGS != NULL){
		InterCode ic = (InterCode)malloc(sizeof(InterCode_));
		ic->kind = ARGSL;
		ic->u.label = copy_operand(ARGS->op);
		insert_ir(ic);
		ARGLIST tmp = ARGS;
		ARGS = ARGS->next;
		free(tmp);
	}
}


void deleteAll(){
	while(IRHead!=NULL){	
		switch(IRHead->code->kind){
		case ASSIGN:
		case CALLL:
			free(IRHead->code->u.assign.left);
			free(IRHead->code->u.assign.right);
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIVI:
			free(IRHead->code->u.binop.result);
			free(IRHead->code->u.binop.op1);
			free(IRHead->code->u.binop.op2);
			break;
		case LABEL_LINE:
		case GOTOL:
		case RETURNL:
		case READL:
		case WRITEL:
		case FUNCTIONL:
		case PARAML:
		case ARGSL:
			free(IRHead->code->u.label);
			break;
		case DEC:
			free(IRHead->code->u.dec.v);
			break;
		case COND_GOTO:
			free(IRHead->code->u.condop.t1);
			free(IRHead->code->u.condop.t2);
			free(IRHead->code->u.condop.lb);
 			break;
		default:{
		}
		}
		InterCodes tmp = IRHead;
		IRHead = tmp->next;
		free(tmp);	
	}
}

/*
void optimal(){
	InterCodes p = IRHead;
	while(p!=NULL){
		InterCode code = p->code;
		switch(p->c)

	}

}*/
