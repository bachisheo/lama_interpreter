TESTS=$(sort $(basename $(wildcard *.lama)))
TESTS_BC=$(sort $(basename $(wildcard *.bc)))
FREQ_COUNT=../../build/freq_count
RUNTIME=LAMA=../runtime 
RESULT=../../benchmarks.txt

LAMAC=lamac

.PHONY: check $(TESTS)

all: $(TESTS)

check: $(TESTS)

$(TESTS):
	@echo "performance/$@"
	$(LAMAC) -b $@.lama

test: $(TESTS_BC)

$(TESTS_BC):
	@echo "test bytecode frequency "
	$(FREQ_COUNT) $@.bc

clean:
	$(RM) test*.log *.bc *.s *~ $(TESTS) *.i
