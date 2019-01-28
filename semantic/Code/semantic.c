#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"compiler.h"

void printNode(){
	if(flag == 0){
		int c = 0;
		Node* p = stack[0];
		front(p,0);
	}
}



unsigned int hash_pjw(char* name){
	unsigned int val = 0;
	unsigned int i = 0;
	for(; *name; ++name){
		val = (val << 2) + *name;
		if(i = val & ~0x3fff){
			val = (val ^ (i >> 12)) & 0x3fff;
		}
	}
	return val;
}



void delete_symbol_entry_field(int field){
	Symbol_Entry pointer = symbol_entry_field_stack[field];
	while(pointer != NULL){
		Symbol_Entry temp = NULL;
		temp = pointer;
		pointer = pointer->field_next;
		unsigned int pos = 0;
		pos = hash_pjw(temp->name);
		symbol_entry_table[pos] = temp->hash_next;	
		free(temp);
	}
	symbol_entry_field_stack[field] = NULL;
}



void add_symbol_entry(Symbol_Entry sy_en, int field){
	
	sy_en->field_next = symbol_entry_field_stack[field];
	symbol_entry_field_stack[field] = sy_en;
	unsigned int pos = 0;
	pos = hash_pjw(sy_en->name);
	sy_en->hash_next = symbol_entry_table[pos];
	symbol_entry_table[pos] = sy_en;					

}


Symbol_Entry search_symbol_entry(char* name, int field){
	
	unsigned int pos = hash_pjw(name);
	Symbol_Entry pointer = symbol_entry_table[pos];
	while(pointer != NULL){
		if(strcmp(pointer->name,name) == 0){
			if(pointer->field <= field)
				return pointer;
		}
		pointer = pointer->hash_next;
	}	
	return NULL;
}


void delete_fieldList(FieldList fieldlist){

	if(fieldlist == NULL)
		return;
	FieldList fls = fieldlist->tail;
	free(fieldlist);
	delete_fieldList(fls);
}

void delete_paraList(ParaList paralist){
	
	if(paralist == NULL)
		return;
	ParaList pls = paralist->tail;
	free(paralist);
	delete_paraList(pls);
}


void delete_type_entry_field(int field){
	Type_Entry pointer = type_entry_field_stack[field];
	while(pointer != NULL){
		Type_Entry temp = NULL;
		temp = pointer;
		pointer = pointer->field_next;	
		unsigned int pos = 0;
		pos = hash_pjw(temp->name);
		type_entry_table[pos] = temp->hash_next;
		if(temp->type == NULL)
			continue;
		switch(temp->type->kind){
			case STRUCTURE:{
				delete_fieldList(temp->type->u.structure);
				free(temp->type);
				break;
			}
			case ARRAY:{
				Type tp = temp->type;
				while(tp->kind == ARRAY){
					Type tmp = tp;
					tp =  tp->u.array.elem;
					free(tmp);
				}
				break;
			}
			case FUNCTION:{
				delete_paraList(temp->type->u.declaration.function);
				free(temp->type->u.declaration.ftp);
				free(temp->type);
				break; 
			}
			
		}
		free(temp);
	}
	type_entry_field_stack[field] = NULL;	
}


void add_type_entry(Type_Entry tp_en,int field){

	tp_en->field_next = type_entry_field_stack[field];
	type_entry_field_stack[field] = tp_en;
	unsigned int pos = 0;
	pos = hash_pjw(tp_en->name);
	tp_en->hash_next = type_entry_table[pos];
	type_entry_table[pos] = tp_en;

}



Type_Entry search_type_entry(char* name, int field){
	unsigned int pos = hash_pjw(name);
	Type_Entry pointer = type_entry_table[pos];
	while(pointer != NULL){
		if(strcmp(pointer->name,name) == 0){
			if(pointer->field <= field)
				return pointer;
		}
		pointer = pointer->hash_next;
	}	
	return NULL;
}





Type func_specifier(Node* specifier){
	if(strcmp(specifier->sons->name, "TYPE")==0){
		if(strncmp( (char*)(symbol+*(int*)&(specifier->sons->value)), "int",3)==0){
			return TYPE_INT;
		}
		else if(strncmp((char*)(symbol+*(int*)&specifier->sons->value),"float",5)==0){
			return TYPE_FLOAT;
		}
		else return NULL;			

	}
	else if(strcmp(specifier->sons->name,"StructSpecifier") == 0){
		//return NULL;
		return func_structspecifier(specifier->sons);
	}

	else return NULL;
}

Type func_structspecifier(Node* structspecifier){
	Node* OptTag = structspecifier->sons->siblings;
	Node* Tag = structspecifier->sons->siblings;
	if(strcmp(OptTag->name,"OptTag") == 0){
		Type tp = func_opttag(OptTag);
		return tp;
	}	
	else if(strcmp(Tag->name,"Tag") == 0){
		return func_tag(Tag);
	}
	else return NULL;
}


Type func_opttag(Node* opttag){
	
	Node* ID = opttag->sons;
	char name[32]; 
	Type tp = (Type)malloc(sizeof(struct Type_));
	Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
	
	tp_en->type = tp;
	
	NameNode p = NULL;
	if(strcmp(ID->name,"NULL") == 0){
		sprintf(name, "%d", countnull);
		countnull++;
	}
	else{
	
		Type_Entry ty = search_type_entry((char*)(symbol+*(int*)&ID->value), cur_field);
		if((ty != NULL) && ty->field == cur_field){
			sprintf(name, "%d",countnull);
			countnull ++;
			printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",opttag->posBegin,(char*)(symbol+*(int*)&ID->value));
		}
		else{
			
			strcpy(name, (char*)(symbol+*(int*)&ID->value));	
		}
	}
	
	strcpy(tp_en->name, name);
	
	p = (NameNode)malloc(sizeof(struct NameNode_));
	strcpy(p->name,"");
	p->next = NULL;
	tp_en->field = cur_field;
	tp->u.structure = NULL;
	tp->kind  = STRUCTURE;
	
	add_type_entry(tp_en, cur_field);

	func_deflist(&tp->u.structure, opttag->siblings->siblings, p);
	while(p != NULL){
		NameNode ptmp = p;
		p = p->next;
		//printf("%s\n", ptmp->name);
		free(ptmp);
	}
	
	return tp;

}

Type func_tag(Node* tag){
	Node* ID = tag->sons;
	Type_Entry tp_en = search_type_entry((char*)(symbol+*(int*)&ID->value), cur_field);
	if(tp_en == NULL){
		printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", tag->posBegin,(char*)(symbol+*(int*)&ID->value));
		return NULL;
	}
	else{
		//printf("%s\n", tp_en->name);
		return tp_en->type;
	}		
}


void func_deflist(FieldList* tail, Node* deflist, NameNode p){
	if(strcmp(deflist->sons->name,"NULL") == 0){
		*tail = NULL;
	}
	else{
		FieldList* tailnew = func_def(tail,deflist->sons,p);
		func_deflist(tailnew ,deflist->sons->siblings,p);
	}

}


FieldList* func_def(FieldList*tail, Node* def, NameNode p){
	Type type = func_specifier(def->sons);
	FieldList* fls = func_declist(tail,type,def->sons->siblings,p);
	return fls;

}

FieldList* func_declist(FieldList* tail, Type type, Node* declist, NameNode p){
	FieldList * fls = func_dec(tail, type, declist->sons,p);
	if(declist->sons->siblings != NULL){
		fls = func_declist(fls, type, declist->sons->siblings->siblings, p);
	}
	return fls;
}


FieldList* func_dec(FieldList* tail ,Type type, Node* dec, NameNode p){
	FieldList* tail1 = func_vardec(tail, type, dec->sons, p);
	if(dec->sons->siblings != NULL){
		printf("Error type 15 at Line %d: Wrong assignment.\n",dec->posBegin);
	}
	return tail1;
}

FieldList* func_vardec(FieldList* tail, Type type, Node* vardec, NameNode p){
	if(vardec->sons->siblings != NULL){		
		Type tp = (Type)malloc(sizeof(struct Type_));
		tp->kind = ARRAY;
		tp->u.array.elem = type;
		tp->u.array.size = *(int*)&(vardec->sons->siblings->siblings->value);
		return func_vardec(tail, tp, vardec->sons, p);
	}
	else{
		*tail = (FieldList)malloc(sizeof(struct FieldList_));
		char name[32]; 
		while(p->next != NULL){
			//printf("*%s\n", vardec->sons->name);
			if(strcmp(p->next->name, (char*)(symbol+*(int*)&vardec->sons->value)) == 0){
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",vardec->posBegin, (char*)(symbol+*(int*)&vardec->sons->value));
				sprintf(name, "%d", countnull);
				countnull++;
				break;
			}
			p = p->next;
		}
		if(p->next == NULL){
			p->next = (NameNode)malloc(sizeof(struct NameNode_));
			strcpy(p->next->name, (char*)(symbol+*(int*)&vardec->sons->value));
			//printf("%s\n",(char*)(symbol+*(int*)&vardec->sons->value));
			strcpy(name,p->next->name);
			p->next->next = NULL;
		}
		Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
		sprintf(tp_en->name, "%d", countnull);
		countnull++;
		tp_en->type = type;
		tp_en->field = cur_field;
		add_type_entry(tp_en, cur_field);
		(*tail)->type =type;
		strcpy((*tail)->name, name);
		return &((*tail)->tail);
	}
}


void func_extdef(Node* extdef){
	Node* specifier = extdef->sons;
	Type tp = func_specifier(specifier);
	Node* ExtDecList = specifier->siblings;
	Node* FunDec = specifier->siblings;	
	if(strcmp(FunDec->name ,"FunDec") == 0){
		if(strcmp(FunDec->siblings->name, "CompSt") == 0){	
			func_fundec(FunDec, tp,1);
			func_compst(FunDec->siblings);
		}
		else{
			func_fundec(FunDec, tp,0);
			delete_symbol_entry_field(cur_field+1);
			delete_type_entry_field(cur_field+1);
		}
	}
	else if(strcmp(ExtDecList->name, "ExtDecList") == 0){
		func_extdeclist(ExtDecList, tp);
	}

}

void func_extdeclist(Node* extdeclist, Type type){
	func_vardec_in_extdeclist(extdeclist->sons, type);
	if(extdeclist->sons->siblings != NULL){
		func_extdeclist(extdeclist->sons->siblings->siblings, type);
	}
}


void func_vardec_in_extdeclist(Node* vardec, Type type){
	if(vardec->sons->siblings != NULL){
		Type tp = (Type)malloc(sizeof(struct Type_));
		tp->kind = ARRAY;
		tp->u.array.elem = type;
		tp->u.array.size = *(int*)&(vardec->sons->siblings->siblings);
		func_vardec_in_extdeclist(vardec->sons, tp);
	}	
	else{
		Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
		char name[32];
		Symbol_Entry sy_in_table = search_symbol_entry( (char*)(symbol + *(int*)&(vardec->sons->value) ), cur_field);
		if((sy_in_table != NULL) && (sy_in_table->field == cur_field)){
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", vardec->posBegin, (char*)(symbol + *(int*)&(vardec->sons->value) ) );
			sprintf(name, "%d", countnull);
			countnull++;
		}
		else{
			strcpy(name, (char*)(symbol + *(int*)&(vardec->sons->value) ));
					
		}
		Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
		sprintf(tp_en->name, "%d", countnull);
		countnull++;
		tp_en->field = cur_field;
		tp_en->type = type;
		add_type_entry(tp_en, cur_field);
		strcpy(sy_en->name, name);
		sy_en->type = type;
		sy_en->field = cur_field;
		add_symbol_entry(sy_en, cur_field);				
	}

}

/*
void func_fundec(Node* fundec, Type type){
	Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
	char name[32];
	Symbol_Entry sy_in_table = search_symbol_entry( (char*)(symbol+*(int*)&(fundec->sons->value)) ,cur_field);
	if( (sy_in_table != NULL)&&(sy_in_table->field == cur_field)){
		printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec->posBegin, (char*)(symbol+*(int*)&(fundec->sons->value)));
		sprintf(name, "%d", countnull);
		countnull++;
	}
	else{
		strcpy(name, (char*)(symbol+*(int*)&(fundec->sons->value)));
	}
	strcpy(sy_en->name, name);
	sy_en->field = cur_field;

	Type tp = (Type)malloc(sizeof(struct Type_));
	sy_en->type = tp;
	add_symbol_entry(sy_en,cur_field);

	Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
	sprintf(tp_en->name, "%d", countnull);
	countnull++;
	tp_en->type = tp;
	tp_en->field = cur_field;
	add_type_entry(tp_en, cur_field);
		
	tp->kind = FUNCTION;
	tp->u.declaration.function = (ParaList)malloc(sizeof(struct ParaList_));
	tp->u.declaration.function->type = type;
	
	
	if(fundec->sons->siblings->siblings->siblings != NULL){
		cur_field++;
		func_varlist_in_fundec(fundec->sons->siblings->siblings, &(tp->u.declaration.function->tail));
		cur_field--;
	}
	else{
		tp->u.declaration.function->tail = NULL;
	}

	
	local_func_type = type;
}
*/

void func_fundec(Node* fundec, Type type, int tag){
	local_func_type = type;
	Symbol_Entry sy_in_table = search_symbol_entry( (char*)(symbol+*(int*)&(fundec->sons->value)) ,cur_field);
	
	Type tp = (Type)malloc(sizeof(struct Type_));
	tp->kind = FUNCTION;
	tp->u.declaration.function = (ParaList)malloc(sizeof(struct ParaList_));
	tp->u.declaration.function->type = type;	
	tp->u.declaration.ftp = (FuncType)malloc(sizeof(struct FuncType_));
	tp->u.declaration.ftp->tag = 0;
	Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
	sprintf(tp_en->name, "%d", countnull);
	countnull++;
	tp_en->type = tp;
	tp_en->field = cur_field;
	add_type_entry(tp_en, cur_field);
	
	if(fundec->sons->siblings->siblings->siblings != NULL){
		cur_field++;
		func_varlist_in_fundec(fundec->sons->siblings->siblings, &(tp->u.declaration.function->tail));
		cur_field--;
	}
	else{
		tp->u.declaration.function->tail = NULL;
	}
	
	

	if( (sy_in_table != NULL)&&(sy_in_table->field == cur_field)  ){
		//if(  (sy_in_table->type!=NULL) && (sy_in_table->type->kind != FUNCTION) ){
		//	printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec->posBegin,(char*)(symbol+*(int*)&(fundec->sons->value)) );
		//	return NULL;
		//}
		int t;
		t = type_judge(tp, sy_in_table->type);
		if(t == 1){
			if( (sy_in_table->type->u.declaration.ftp->tag == 1) && (tag==1)){
				printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec->posBegin,(char*)(symbol+*(int*)&(fundec->sons->value)) );
			}
			else if( (sy_in_table->type->u.declaration.ftp->tag == 0) && (tag==1)){
					sy_in_table->type->u.declaration.ftp->tag == 1;
					Type tp1 = sy_in_table->type;
					Declaration dcl = tp1->u.declaration.ftp->pointer;
					if(dcl->before != NULL){
						dcl->before->next = dcl->next;
					}
					else{
						declaration = dcl->next;
					}					
					if(dcl->next != NULL){
						dcl->next->before = dcl->before;
					}
					free(dcl);
			}
			else {
				sy_in_table->type->u.declaration.ftp->tag =  sy_in_table->type->u.declaration.ftp->tag || tag;
			}
		}
		else{
			if((sy_in_table->type->u.declaration.ftp->tag == 1) && (tag==1)){
				printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec->posBegin,(char*)(symbol+*(int*)&(fundec->sons->value)) );
			}
			else {
				printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", fundec->posBegin, (char*)(symbol+*(int*)&(fundec->sons->value)));
				sy_in_table->type->u.declaration.ftp->tag =  sy_in_table->type->u.declaration.ftp->tag || tag;
			}
		}		
	}
	else{
		//printf("%d\n",tag);
		Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
		strcpy(sy_en->name, (char*)(symbol+*(int*)&(fundec->sons->value)) );
		sy_en->field = cur_field;
		sy_en->type = tp;			
		tp->u.declaration.ftp->tag = tag;
		if(tag == 0){
			Declaration dlt = (Declaration)malloc(sizeof(struct Declaration_));
			tp->u.declaration.ftp->pointer = dlt;
			dlt->pos = fundec->posBegin;
			dlt->sy_en = sy_en;
			dlt->before = NULL;
			if(declaration != NULL){
				declaration->before = dlt;
			}
			dlt->next = declaration;
			declaration = dlt;
		}
		add_symbol_entry(sy_en,cur_field);
					
	}

}


void func_varlist_in_fundec(Node* varlist, ParaList* tail){

	ParaList* tail1 = func_paramdec(varlist->sons, tail);
	if(varlist->sons->siblings!=NULL){
		func_varlist_in_fundec(varlist->sons->siblings->siblings, tail1);
	}	

}

ParaList* func_paramdec(Node* paramdec, ParaList* tail){
	Type type = func_specifier(paramdec->sons);
	(*tail) = (ParaList)malloc(sizeof(struct ParaList_));
	func_vardec_in_fundec(paramdec->sons->siblings, *tail, type);
	return &((*tail)->tail);
}

void func_vardec_in_fundec(Node* vardec, ParaList tail, Type type){
	if(vardec->sons->siblings!=NULL){
		Type tp = (Type)malloc(sizeof(struct Type_));
		tp->kind = ARRAY;
		tp->u.array.size = *(int*)&(vardec->sons->siblings->siblings);
		tp->u.array.elem = type;
		func_vardec_in_fundec(vardec->sons, tail, type);
	}
	else{
		Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
		Symbol_Entry sy_in_table = search_symbol_entry((char*)(symbol+*(int*)&(vardec->sons->value)) , cur_field);
		char name[32];
		if( (sy_in_table != NULL) && (sy_in_table->field == cur_field)){
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", vardec->posBegin, (char*)(symbol+*(int*)&(vardec->sons->value)) ) ;
			sprintf(name, "%d", countnull);
			countnull++;
		}
		else{
			strcpy(name, (char*)(symbol+*(int*)&(vardec->sons->value)) );
		}

		Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
		sprintf(tp_en->name, "%d", countnull);
		countnull++;
		tp_en->field = cur_field-1;	
		tp_en->type = type;
		add_type_entry(tp_en, cur_field-1);
		tail->type = type;
		
		strcpy(sy_en->name, name);
		sy_en->field = cur_field;
		sy_en->type = type;
		add_symbol_entry(sy_en, cur_field);		
				
	}

}


void func_compst(Node* compst){

	cur_field ++;
	Node* deflist = compst->sons->siblings;
	Node* stmtlist = deflist->siblings;
	func_deflist_in_compst(deflist);
	func_stmtlist(stmtlist);
	delete_symbol_entry_field(cur_field);
	delete_type_entry_field(cur_field);
	cur_field--;
}


void func_deflist_in_compst(Node* deflist){
	if(strcmp(deflist->sons->name, "NULL")!=0){
		func_def_in_compst(deflist->sons);
		func_deflist_in_compst(deflist->sons->siblings);
	}
}


void func_def_in_compst(Node* def){
	Type type = func_specifier(def->sons);
	func_declist_in_compst(def->sons->siblings, type);
}


void func_declist_in_compst(Node* declist, Type type){
	func_dec_in_compst(declist->sons,type);
	if(declist->sons->siblings != NULL){
		func_declist_in_compst(declist->sons->siblings->siblings, type);
	}
}


void func_dec_in_compst(Node* dec, Type type){
	Type tp1 = func_vardec_in_compst(dec->sons, type);
	if(dec->sons->siblings != NULL){
		char c;
		Type tp2 = func_exp(dec->sons->siblings->siblings, &c);
		int t = type_judge(tp1,tp2);
		if(t == 0){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", dec->sons->posBegin);
		}

	}

}


Type func_vardec_in_compst(Node* vardec, Type type){

	if(vardec->sons->siblings != NULL){
		Type tp = (Type)malloc(sizeof(struct Type_));
		tp->kind = ARRAY;
		tp->u.array.elem = type;
		tp->u.array.size = *(int*)&(vardec->sons->siblings->siblings->value);
		return func_vardec_in_compst(vardec->sons, tp);	
	}
	else{
		Symbol_Entry sy_en = (Symbol_Entry)malloc(sizeof(struct Symbol_Entry_));
	        Symbol_Entry sy_in_table = search_symbol_entry((char*)(symbol+*(int*)&(vardec->sons->value) ) , cur_field);		
		
		char name[32];
		if((sy_in_table != NULL)&&(sy_in_table->field == cur_field)){
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", vardec->posBegin, (char*)(symbol+*(int*)&(vardec->sons->value)) ) ;
			sprintf(name, "%d", countnull);
			countnull++;
		}
		else{
			strcpy(name, (char*)(symbol+*(int*)&(vardec->sons->value) ));
		}
		strcpy(sy_en->name, name);
		sy_en->type = type;
		sy_en->field = cur_field;
		Type_Entry tp_en = (Type_Entry)malloc(sizeof(struct Type_Entry_));
		sprintf(tp_en->name, "%d", countnull);
		countnull++;
		//printf("%d\n",type->u.basic);
		tp_en->field = cur_field;	
		tp_en->type = type;
		add_type_entry(tp_en, cur_field);			
		add_symbol_entry(sy_en, cur_field);
		return type;

	}
}

void func_stmtlist(Node* stmtlist){
	if( strcmp(stmtlist->sons->name, "NULL") != 0 ){
		func_stmt(stmtlist->sons);
		func_stmtlist(stmtlist->sons->siblings);
	}

}	


void func_stmt(Node* stmt){
	if(strcmp(stmt->sons->name, "Exp")==0){
		char c;
		Type type = func_exp(stmt->sons, &c);			
	}
	else if(strcmp(stmt->sons->name, "CompSt") == 0){
		func_compst(stmt->sons);
	}
	else if(strcmp(stmt->sons->name, "RETURN") == 0){
		char c;
		Type type = func_exp(stmt->sons->siblings, &c);
		int t = type_judge(type, local_func_type);
		if(t == 0){
			printf("Error type 8 at Line %d: Type mismatched for return.\n", stmt->sons->siblings->posBegin);
		}
	}
	else if(strcmp(stmt->sons->name, "WHILE") == 0){
		char c;
		Type type = func_exp(stmt->sons->siblings->siblings, &c);
		func_stmt(stmt->sons->siblings->siblings->siblings->siblings);
	}
	else{
		Node* exp = stmt->sons->siblings->siblings;
		Node* stmt1 = exp->siblings->siblings;
		char c;
		func_exp(exp, &c);
		func_stmt(stmt1);
		if(stmt1->siblings != NULL){
			Node* stmt2 = stmt1->siblings->siblings;
			func_stmt(stmt2);	
		}
	}
	

}

Type func_exp(Node* exp, char* c){
	if(strcmp(exp->sons->name, "INT") == 0){
		*c = 'l';
		return TYPE_INT;
	}
	else if(strcmp(exp->sons->name, "FLOAT") == 0){
		*c = 'l';
		return TYPE_FLOAT;
	}
	else if(strcmp(exp->sons->name, "ID") == 0){
		if(exp->sons->siblings == NULL){
			Symbol_Entry sy_en = search_symbol_entry( (char*)(symbol + *(int*)&(exp->sons->value) ), cur_field );
			if(sy_en == NULL){
				printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
				*c = 'r';
				return NULL;
			}
			else{
				*c = 'r';
				return sy_en->type;
			}
			
		}
		else if(exp->sons->siblings->siblings->siblings == NULL){
			Symbol_Entry sy_en = search_symbol_entry( (char*)(symbol + *(int*)&(exp->sons->value) ), cur_field );
			if(sy_en == NULL){
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
				*c = 'l';
				return NULL;
			}
			else{
				
				*c = 'l';
				Type tp = sy_en->type;
				if(tp->kind != FUNCTION){
					printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
					return NULL;
				
				}				

				if(tp->u.declaration.function->tail != NULL){
					printf("Error type 9 at Line %d: Function \"%s\" is not applicable for the arguments.\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
					return NULL;
				}
				return tp->u.declaration.function->type;
				
				 
			}
		}
		else{
			Symbol_Entry sy_en = search_symbol_entry( (char*)(symbol + *(int*)&(exp->sons->value) ), cur_field );
			if(sy_en == NULL){
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
				*c = 'l';
				return NULL;
			}
			else{
				*c = 'l';
				Type tp = sy_en->type;
				if(tp->kind != FUNCTION){
					printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
					return NULL;
				
				}
				
				ParaList pls = sy_en->type->u.declaration.function;
				Type type_return = pls->type;
				pls = pls->tail;
				Node* args = exp->sons->siblings->siblings;
				int t = 1;
				while((t == 1) && (pls!=NULL) && (args!=NULL)){
					char c1;
					Type exptype = func_exp(args->sons ,&c1);
					Type plstype = pls->type;
					t = t & type_judge(exptype,plstype);
					pls = pls->tail;
					if(args->sons->siblings==NULL){
						args = NULL;				
					}
					else{
						args = args->sons->siblings->siblings;
					}	
				}
				if((t ==0) || (pls!=NULL) || (args != NULL)){
					printf("Error type 9 at Line %d: Function \"%s\" is not applicable for the arguments.\n", exp->posBegin, (char*)(symbol + *(int*)&(exp->sons->value) ));
					return NULL;
				}
				return type_return;
			}
		}
	}
	else if(strcmp(exp->sons->name, "LP")==0){
		return func_exp(exp->sons->siblings, c);		
	}
	else if(strcmp(exp->sons->name, "MINUS") == 0){
		*c = 'l';
		char c1;
		Type tp = func_exp(exp->sons->siblings, &c1);
		if(tp == NULL){
			return NULL;
		}
		else{
			if(tp->kind != BASIC){
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->sons->siblings->posBegin);
				return NULL;
			}
			return tp;		
		}
	}
	else if(strcmp(exp->sons->name, "NOT")==0){
		*c = 'l';
		char c1;
		Type tp = func_exp(exp->sons->siblings, &c1);
		if(tp == NULL){
			return NULL;
		}
		else{
			if(tp->kind != BASIC){
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->sons->siblings->posBegin);
				return NULL;
			}
			return tp;		
		}
	}
	else if(  (  strcmp(exp->sons->siblings->name, "AND") == 0 )||  (  strcmp(exp->sons->siblings->name, "OR") == 0 )|| (  strcmp(exp->sons->siblings->name, "RELOP") == 0 ) || (  strcmp(exp->sons->siblings->name, "PLUS") == 0 )||(  strcmp(exp->sons->siblings->name, "MINUS") == 0 )||(  strcmp(exp->sons->siblings->name, "STAR") == 0 )||(  strcmp(exp->sons->siblings->name, "DIV") == 0 )){
		*c = 'l';
		char c1,c2;
		Type tp1,tp2;
		tp1 = func_exp(exp->sons, &c1);
		tp2 = func_exp(exp->sons->siblings->siblings, &c2);
		int t = type_judge(tp1,tp2);
		if(t == 1)
			return tp1;
		else{ 
			printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->sons->posBegin);
			return NULL;
		}
	}
	else if(strcmp(exp->sons->siblings->name, "DOT") == 0){
		*c = 'r';
		Type tp1, tp2;
		char c1,c2;
		tp1 = func_exp(exp->sons, &c1);
		if(tp1 == NULL){
			return NULL;		
		}
		else{
			if(tp1->kind != STRUCTURE){
				printf("Error type 13 at Line %d: Illegal use of \".\".\n", exp->sons->posBegin);
				return NULL;
			}
			FieldList fls = tp1->u.structure;
			while(fls != NULL){
				if(strcmp(fls->name,(char*)(symbol+ *(int*)&(exp->sons->siblings->siblings->value)))==0){

					return fls->type;				
				}
				else{
					fls = fls->tail;
				}
			}
			printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",exp->sons->posBegin, (char*)(symbol+ *(int*)&(exp->sons->siblings->siblings->value)));			
			return NULL;
		}
	}
	else if(  strcmp(exp->sons->siblings->name, "ASSIGNOP") == 0  ){
		*c = 'l';
		char c1, c2;
		Type tp1,tp2;
		tp1 = func_exp(exp->sons, &c1);
		tp2 = func_exp(exp->sons->siblings->siblings, &c2);
		int t = type_judge(tp1,tp2);
		if(t == 0){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", exp->sons->posBegin);
		}
		if(c1 == 'l'){
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", exp->sons->posBegin);
		}
		if( (t == 0) || (c1 == 'l')){
			return NULL;
		}
		return tp1;
	}
	else{
		*c = 'r';
		char c1,c2;
		Type tp1,tp2;
		tp1 = func_exp(exp->sons, &c1);
		tp2 = func_exp(exp->sons->siblings->siblings, &c2);
		if(tp1 == NULL){
			return NULL;	
		}
		if(tp2 == NULL){
			return NULL;
		}
		if(tp1->kind != ARRAY){
			printf("Error type 10 at Line %d: not an array.\n", exp->sons->posBegin);
			return NULL;
		}
		if(tp2 != TYPE_INT){
			printf("Error type 12 at Line %d: not an integer.\n", exp->sons->siblings->siblings->posBegin);
			return NULL;
		}
		return tp1->u.array.elem;
	}
}


int type_judge(Type tp1, Type tp2){
	if(tp1 == NULL)
		return 0;
	if(tp2 == NULL)
		return 0;

	if(tp1 == tp2){
		return 1;
	}
	else {
		if(tp1->kind!=tp2->kind){
			return 0;
		}
		else if(tp1->kind == BASIC){
			if(tp1->u.basic == tp2->u.basic){
				return 1;
			}
			else return 0;
		}
		else if(tp1->kind == ARRAY){
			int size1 = tp1->u.array.size;
			int size2 = tp2->u.array.size;
			if(size1 != size2){
				return 0;	
			}
			else return type_judge(tp1->u.array.elem, tp2->u.array.elem);
		}
		else if(tp1->kind == STRUCTURE){
			FieldList fls1 = tp1->u.structure;
			FieldList fls2 = tp2->u.structure;
			int t =1;
			//t = t & type_judge(fls1->type,fls2->type);
			while( (t!= 0) && (fls1 != NULL) && (fls2 != NULL)){
				t = t & type_judge(fls1->type,fls2->type);
				fls1 = fls1->tail;
				fls2 = fls2->tail;
			}
			if( (t==0) ||(fls1!=NULL) || (fls2 != NULL)){
				return 0;
			}
			else return 1;
		}
		else if(tp1->kind == FUNCTION){
			ParaList pls1 = tp1->u.declaration.function;
			ParaList pls2 = tp2->u.declaration.function;
			int t = 1;
			while( (t!= 0) && (pls1 != NULL) && (pls2 != NULL)){
				t = t & type_judge(pls1->type,pls2->type);
				pls1 = pls1->tail;
				pls2 = pls2->tail;
			}
			if( (t==0) ||(pls1!=NULL) || (pls2 != NULL)){
				return 0;
			}
			else return 1;
		}
	}
	return 0;
}

void undefined_decs(){
	while(declaration != NULL){
		printf("Error type 18 at Line %d: Undefined function \"%s\".\n", declaration->pos, declaration->sy_en->name);
		Declaration tmp = declaration;
		declaration = declaration->next;
		free(tmp);
	}

}

int semantic_analysis(Node* p){
	while(p!=NULL){	
		if(strcmp(p->name,"ExtDef")==0){
			func_extdef(p);
			
		}
		else{
			semantic_analysis(p->sons);
		}
		p = p->siblings;
	}
	undefined_decs();
	cur_field = 0;
	delete_symbol_entry_field(cur_field);
	delete_type_entry_field(cur_field);
	return 0;	
}
