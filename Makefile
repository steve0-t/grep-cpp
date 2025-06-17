GCC = g++
CFLAGS = -std=c++14 -Wall -Werror -Wextra -pedantic -g
CPPCHECK = cppcheck --enable=performance,unusedFunction --error-exitcode=1 --quiet
SRCS = $(wildcard src/*.cpp)
OUTDIR = build
OBJS = $(patsubst src/%.cpp, $(OUTDIR)/%.o, $(SRCS))
EXE = main

all: check $(OUTDIR) $(EXE)

$(OUTDIR):
	@mkdir -p $(OUTDIR)

$(EXE): $(OBJS)
	@echo "Linking $@"
	$(GCC) $(OBJS) -o $@

$(OUTDIR)/%.o: src/%.cpp
	@echo "Compiling $<"
	$(GCC) $(CFLAGS) -c $< -o $@

check: $(SRCS)
	@echo "Checking code with cppcheck"
	$(CPPCHECK) $(SRCS)

.PHONY: clean
clean:
	rm -rf $(OUTDIR) $(EXE)

.PHONY: generate_test_data
generate_test_data:
	chmod +x generate_test_dirs.sh
	./generate_test_dirs.sh

.PHONY: remove_test_data
remove_test_data:
	rm -rf test_dirs
