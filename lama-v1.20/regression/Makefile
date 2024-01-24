TESTS=$(sort $(filter-out test111, $(basename $(wildcard test*.lama))))

LAMAC=lamac
ITER_INTER=../../build/iterinter

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.bc
	@echo "regression/$@ "
	$(ITER_INTER) $< < $@.input > $@.log && diff $@.log orig/$@.log


#generate bytecode for lama file
%.bc: %.lama 
	$(LAMAC) -b $<

clean:
	$(RM) *.bc test*.log *.s *.sm *~ $(TESTS) *.i 
	$(MAKE) clean -C expressions
	$(MAKE) clean -C deep-expressions
