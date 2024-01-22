TESTS=$(sort $(basename $(wildcard generated*.lama)))

LAMAC=lamac
ITER_INTER=../../build/iterinter

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/deep-expressions/$@"
	$(ITER_INTER) $< < $@.input > $@.log && diff $@.log orig/$@.log

#generate bytecode for lama file
%.bc: %.lama 
	$(LAMAC) -b $<

clean:
	rm -f *.bc *.log *.s *~
	find . -maxdepth 1 -type f -not -name '*.*' -not -name 'Makefile' -delete
