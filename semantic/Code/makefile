# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -lfl -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean test
test1:
	./parser ../Test/sample1.cmm
test2:
	./parser ../Test/sample2.cmm
test3:
	./parser ../Test/sample3.cmm
test4:
	./parser ../Test/sample4.cmm
test5:
	./parser ../Test/sample5.cmm
test6:
	./parser ../Test/sample6.cmm
test7:
	./parser ../Test/sample7.cmm
test8:
	./parser ../Test/sample8.cmm
test9:
	./parser ../Test/sample9.cmm
test10:
	./parser ../Test/sample10.cmm
test11:
	./parser ../Test/sample11.cmm
test12:
	./parser ../Test/sample12.cmm
test13:
	./parser ../Test/sample13.cmm
test14:
	./parser ../Test/sample14.cmm
test15:
	./parser ../Test/sample15.cmm
test16:
	./parser ../Test/sample16.cmm
test17:
	./parser ../Test/sample17.cmm
test1b:
	./parser ../Test/sample1b.cmm
test2b:
	./parser ../Test/sample2b.cmm
test3b:
	./parser ../Test/sample3b.cmm
test4b:
	./parser ../Test/sample4b.cmm
test5b:
	./parser ../Test/sample5b.cmm
test6b:
	./parser ../Test/sample6b.cmm
test7b:
	./parser ../Test/sample7b.cmm
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
