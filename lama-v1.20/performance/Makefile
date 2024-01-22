TESTS=$(sort $(basename $(wildcard *.lama)))
ITER_INTER=../../build/iterinter
RUNTIME=LAMA=../runtime 
RESULT=../../benchmarks.txt

LAMAC=lamac

.PHONY: check $(TESTS)

check: $(TESTS)

%.bc: %.lama 
	$(LAMAC) -b $<

%lama_bin: %.lama
	$(RUNTIME) $(LAMAC) $<	

$(TESTS): %: %.bc %lama_bin
	@echo $@
	`which time` -o $(RESULT) -f "LAMA BINARY RUN \t\t\t%U" ./$@ 
	$(RUNTIME) echo "0" | `which time` -o $(RESULT) -a -f "LAMA INTERPRETER RUN \t\t%U" $(LAMAC) -i $@.lama 
	$(RUNTIME) echo "0" | `which time` -o $(RESULT) -a -f "LAMA STACK MACHINE RUN \t\t%U" $(LAMAC) -s $@.lama # -o $(RESULT)
	`which time` -o $(RESULT) -a -f "ITERATIVE INTERPRETER RUN \t%U" $(ITER_INTER) $@.bc -o $(RESULT)
clean:
	$(RM) test*.log *.bc *.s *~ $(TESTS) *.i
