SRCSER=src/serial
SRCPAR=src/parallel
SRCMIC=src/mic
SRCGEN=src/gen

BIN=bin/
BUILD=build/

.PHONY: clean makedirs all $(SRCCOM) $(SRCSER) $(SRCPAR)

all: clean makedirs ser par mic gen
	rm -rf $(BUILD)

ser:
	$(MAKE) -C $(SRCSER)

par:
	$(MAKE) -C $(SRCPAR)

mic:
	$(MAKE) -C $(SRCMIC)

gen:
	$(MAKE) -C $(SRCGEN)

makedirs:
	mkdir -p $(BIN) $(BUILD)

clean:
	rm -rf $(BIN) $(BUILD)
