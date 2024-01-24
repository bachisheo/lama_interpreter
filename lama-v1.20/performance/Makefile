TESTS=$(sort $(basename $(wildcard *.lama)))
TESTS_FREQ=$(addprefix freq, $(TESTS))
FREQ_COUNT=../../build/freq_count
RUNTIME=LAMA=../runtime 
RESULT=../../benchmarks.txt
ITER_INTER=../../build/iterinter

LAMAC=lamac

.PHONY: check $(TESTS)

all: %lama_bin

performance: $(TESTS)

frequency: $(TESTS_FREQ)

%.bc: %.lama 
	$(LAMAC) -b $<

%lama_bin: %.lama
	$(RUNTIME) $(LAMAC) $<	

$(TESTS): %: %.bc %lama_bin
	@echo $@
	`which time` -o $(RESULT) -f "LAMA BINARY RUN \t\t\t%U" ./$@ 
	$(RUNTIME) echo "0" | `which time` -o $(RESULT) -a -f "LAMA INTERPRETER RUN \t\t%U" $(LAMAC) -i $@.lama 
	$(RUNTIME) echo "0" | `which time` -o $(RESULT) -a -f "LAMA STACK MACHINE RUN \t\t%U" $(LAMAC) -s $@.lama 
	`which time` -o $(RESULT) -a -f "ITERATIVE INTERPRETER RUN \t%U" $(ITER_INTER) $@.bc 

$(TESTS_FREQ): freq% : %.bc 
	@echo "test bytecode frequency " 
	$(FREQ_COUNT) $(patsubst freq%,%,$@).bc

clean:
	$(RM) test*.log *.bc *.s *~ $(TESTS) *.i
