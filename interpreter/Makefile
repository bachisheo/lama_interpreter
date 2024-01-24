#exec name
TARGET=iterinter
#compiler
CC=gcc
RUNTIME=../lama-v1.20/runtime
BUILDS=../build
REGRESSION=../lama-v1.20/regression
LAMA_ROOT=../lama-v1.20

# -O0 -- optimization level 
# -g -- add debug symbols 
# -m32 -- 32 byte build mode 
# -fstack-protector-all ??
CFLAGS=-O0 -g -m32 -fstack-protector-all

# info about make working 
# this task will be run always, even if file don't change
# for example if it not depends on any file
#DEPENDENCY -- other tasks name!!
.PHONY: mkbuild lama_runtime

#run all tasks
all:  $(TARGET)

#use lama makefile
# -C -- where search for makefile
# $(MAKE) -- path to make command (maybe for other version running)
# create file `runtime.a` -- static library
lama_runtime: 
	$(MAKE) -C $(RUNTIME)

# compile my app object file
# -c -- compile to object file
# -o -- output file
$(TARGET).o: $(TARGET).c mkbuild
	$(CC) $(CFLAGS) -c $(TARGET).c -o $(BUILDS)/$(TARGET).o

#build my app exe (link)
$(TARGET): $(TARGET).o lama_runtime
	$(CC) $(CFLAGS) $(BUILDS)/$(TARGET).o $(RUNTIME)/runtime.a -o $(BUILDS)/$(TARGET) 


#create tmp build folder
#-p -- no error if existing
mkbuild: 
	mkdir -p $(BUILDS) 

# -r -- recursive
clean:
	$(RM) -r build 
	$(MAKE) -C $(RUNTIME) clean 
	$(MAKE) -C $(REGRESSION) clean 
 

test: $(TARGET)
	$(MAKE) -C $(REGRESSION)  
	$(MAKE) -C $(REGRESSION)/expressions 
	$(MAKE) -C $(REGRESSION)/deep-expressions 

performance: $(TARGET)
	$(MAKE) -C $(LAMA_ROOT)/performance

