#include"syntax.tab.h"
struct Node{
	YYSTYPE value;
	struct Node*siblings;
	struct Node*sons; 	
	int posBegin;
	char name[32];
};

typedef struct Node Node;
Node* stack[1024];
char symbol[2048];
int stackCount;
int flag;


typedef struct Type_* Type;
typedef struct Symbol_Entry_* Symbol_Entry;
typedef struct FieldList_* FieldList;
typedef struct ParaList_* ParaList;
typedef struct Type_Entry_* Type_Entry;
typedef struct NameNode_* NameNode;
typedef struct Declaration_* Declaration;
typedef struct FuncType_* FuncType;

struct Type_{
	enum {BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
	union{
		int basic;
		struct {Type elem; int size;}array;
		struct {
			ParaList function;
			FuncType ftp;
		}declaration;
		FieldList structure;
	}u; 
};

struct Declaration_{
	int pos;
	Symbol_Entry sy_en; 
	Declaration before;
	Declaration next;
};

struct FuncType_{
	int tag;
	Declaration pointer;
};

struct Symbol_Entry_{
	char name[32];
	Type type;
	Symbol_Entry hash_next;
	Symbol_Entry field_next;
	int field;
};

struct FieldList_{
	char name[32];
	Type type;
	FieldList tail;
};

struct ParaList_{
	Type type;
	ParaList tail;
};


struct Type_Entry_{
	char name[32];
	Type type;
	Type_Entry hash_next;
	Type_Entry field_next;
	int field;
};

struct NameNode_{
	char name[32];
	NameNode next;
};

Symbol_Entry symbol_entry_table[0x4000];
Symbol_Entry symbol_entry_field_stack[0x100];

Type_Entry type_entry_table[0x4000];
Type_Entry type_entry_field_stack[0x100];


Type TYPE_INT;
Type TYPE_FLOAT;

int cur_field ;
int countnull;//name for unnamed structure
Type local_func_type;
Declaration declaration;

void printNode();
void front();
void reduce(int i, char* name,int pos);

void delete_type_in_symbol(Type type);
void delete_symbol_entry_field(int field);
void add_symbol_entry(Symbol_Entry sy_en,int field);
Symbol_Entry search_symbol_entry(char* name, int field);


void delete_fieldList(FieldList fieldlist);
void delete_paraList(ParaList paralist);
void delete_type_entry_field(int field);
void add_type_entry(Type_Entry tp_en, int field);
Type_Entry search_type_entry(char* name, int field);


Type func_specifier(Node* specifer);
Type func_structspecifier(Node* structspecifier);
Type func_tag(Node* tag);
Type func_opttag(Node* opttag);

void func_deflist(FieldList* tail, Node* deflist, NameNode p);
FieldList* func_def(FieldList* tail, Node* def, NameNode p); 
FieldList* func_declist(FieldList *tail, Type type, Node* declist, NameNode p);
FieldList* func_dec(FieldList* tail, Type type, Node* dec, NameNode p);
FieldList* func_vardec(FieldList* tail, Type type, Node* vardec, NameNode p);

void func_extdef(Node* extdef);
//void func_fundec(Node* fundec, Type type);
void func_extdeclist(Node* extdeclist, Type type);
void func_vardec_in_extdeclist(Node* vardec, Type type);
void func_fundec(Node* fundec, Type type, int tag);
void func_varlist_in_fundec(Node* varlist, ParaList* tail);
ParaList* func_paramdec(Node* paramdec, ParaList* tail);
void func_vardec_in_fundec(Node* vardec, ParaList tail, Type type);

void func_compst(Node* compst);
void func_deflist_in_compst(Node* deflist);
void func_def_in_compst(Node* def);
void func_declist_in_compst(Node* declist, Type type);
void func_dec_in_compst(Node* dec, Type type);
Type func_vardec_in_compst(Node* vardec, Type type);


void func_stmtlist(Node* stmtlist);
void func_stmt(Node* stmt);
Type func_exp(Node* exp,char* c);

int type_judge(Type tp1, Type tp2);
int semantic_analysis(Node* p);
