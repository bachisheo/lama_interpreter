#exec name
TARGET=iterinter
#compiler
CC=gcc

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
	$(MAKE) -C lama-v1.20/runtime 

# compile my app object file
# -c -- compile to object file
# -o -- output file
$(TARGET).o: src/$(TARGET).c mkbuild
	$(CC) $(CFLAGS) -c src/$(TARGET).c -o build/$(TARGET).o

#build my app exe (link)
$(TARGET): $(TARGET).o lama_runtime
	$(CC) $(CFLAGS) build/$(TARGET).o lama-v1.20/runtime/runtime.a -o build/$(TARGET) 


#create tmp build folder
#-p -- no error if existing
mkbuild: 
	mkdir -p build 



# -r -- recursive
clean:
	$(RM) -r build 
	$(MAKE) -C lama-v1.20/runtime clean 
	$(MAKE) -C lama-v1.20/regression clean 

bc: 
	$(MAKE) -C lama-v1.20/byterun  

test: $(TARGET)
	$(MAKE) -C lama-v1.20/regression  
	$(MAKE) -C lama-v1.20/regression/expressions 
	$(MAKE) -C lama-v1.20/regression/deep-expressions 

