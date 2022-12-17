LD := g++

HEADERS := \
	pwncheck.hh

SOURCES := \
	pwncheck.cc

OBJECTS := $(SOURCES:.cc=.o)

CXXFLAGS := -O2
LDLIBS := -lmd

PASSWORDS := pwned-passwords-sha1-ordered-by-hash-v7

MAIN_SOURCES := \
	main.cc
MAIN_OBJECTS := $(MAIN_SOURCES:.cc=.o) $(OBJECTS)
MAIN_TARGET := pwncheck

CATCH_BASE_URL := "https://raw.githubusercontent.com/catchorg/Catch2/devel/extras/catch_amalgamated"

CATCH_BASE := catch/catch_amalgamated
CATCH := $(CATCH_BASE).hpp $(CATCH_BASE).cpp

TEST_SOURCES := \
	test.cc
TEST_OBJECTS := $(TEST_SOURCES:.cc=.o) $(CATCH_BASE).o $(OBJECTS)
TEST_TARGET := pwncheck_test
ALL_SOURCES := \
	$(SOURCES) $(MAIN_SOURCES) $(TEST_SOURCES)

all: $(MAIN_TARGET) $(PASSWORDS).txt

$(CATCH): $(CATCH_BASE).%:
	mkdir -p catch
	curl "$(CATCH_BASE_URL).$*" > $@

$(TEST_OBJECTS): $(CATCH)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) $(TEST_OBJECTS) $(LDLIBS) -o $@

$(MAIN_TARGET): $(MAIN_OBJECTS)
	$(CXX) $(LDFLAGS) $(MAIN_OBJECTS) $(LDLIBS) -o $@

test: $(TEST_TARGET)
	time -p ./$(TEST_TARGET)

example_run: $(MAIN_TARGET) $(PASSWORDS).txt
	time -p ./$(MAIN_TARGET) $(PASSWORDS).txt p4ssw0rd p4ssw0rd p4ssw0rd test

format:
	clang-format -i $(HEADERS) $(ALL_SOURCES)

tidy:
	clang-tidy -fix-errors -checks='*,-llvm-header-guard'  $(ALL_SOURCES) -- -std=c++14

$(PASSWORDS).txt: # Get some coffee, it's gonna take a while
	curl https://downloads.pwnedpasswords.com/passwords/$(PASSWORDS).7z.torrent > $(PASSWORDS).7z.torrent
	transmission-cli $(PASSWORDS).7z.torrent
	p7zip -d $(PASSWORDS).7z
	rm $(PASSWORDS).7z.torrent

clean:
	rm -rf $(MAIN_TARGET) $(TEST_TARGET) *.o

dist-clean: clean
	rm -rf catch/

apt:
	sudo apt install libmd-dev transmission-cli build-essential curl p7zip

apt-optional:
	sudo apt install clang-format clang-tidy

.PHONY: clean dist-clean test tidy format
