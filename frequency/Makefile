TARGET=freq_count
RUNTIME=../lama-v1.20/runtime
BUILDS=../build
REGRESSION=../lama-v1.20/regression
LAMA_ROOT=../lama-v1.20
LAMAC=lamac
TEST_FILES=../lama-v1.20/performance
FREQ_COUNT=../build/freq_count
CXX=g++
CXXFLAGS=-O0 -g -m32 
all: $(TARGET)

lama_runtime: 
	$(MAKE) -C $(RUNTIME)

# compile my app object file
$(TARGET).o: $(TARGET).cpp mkbuild
	$(CXX) $(CXXFLAGS) -c $(TARGET).cpp -o $(BUILDS)/$(TARGET).o


#build my app exe (link)
$(TARGET): $(TARGET).o lama_runtime
	$(CXX) $(CXXFLAGS) $(BUILDS)/$(TARGET).o $(RUNTIME)/runtime.a -o $(BUILDS)/$(TARGET) 

test: $(TARGET)
	$(MAKE) -C $(TEST_FILES) test

mkbuild: 
	mkdir -p $(BUILDS) 

clean:
	$(RM) -r build 
	$(MAKE) -C $(RUNTIME) clean 
	$(MAKE) -C $(REGRESSION) clean 
 