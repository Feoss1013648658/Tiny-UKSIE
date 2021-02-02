#asign proccessor
arch?=pc
gdb?=n
ifeq ($(arch),arm)
cc?=arm-linux-gnueabihf-g++ -std=c++11
else
cc?=g++ -std=c++11
endif

ifeq ($(gdb),y)
cc+=-g
endif
 #工程里的文件夹
SRCDIRS = . ./AlphaPart ./BetaPart ./ReteNet ./Structure ./TestNode ./Utils ./GAalgorithem ./Input ..

#要生成的程序
PROGRAMS=main

#搜索每个文件夹里的cpp文件
SRCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

#每个cpp文件所对应的.o文件
OBJS=$(SRCS:.cpp=.o)

#需要的动态库
LIBS :=   ./libdatacal.a    
# -lz   -lsocket

INCLUDE := -I./AlphaPart -I./BetaPart -I./ReteNet -I./Structure -I./TestNode -I./Utils -I./GAalgorithem -I./Input -I.. -I.
%.o:%.cpp
	$(cc)  -c  -o  $@ $<
all:$(OBJS)
	$(cc)  -o   $(PROGRAMS)   $(OBJS)  $(INCLUDE) $(LIBS)
clean:
	rm  $(OBJS)
