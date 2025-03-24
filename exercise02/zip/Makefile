USERNAME = csbb3947
EXERCISE = 02

EXCLUDE_PATTERNS = "**.vscode/*" "**.idea/*" "**__MACOSX/*" "**.DS_Store/*" "**.dSYM/*"

ARCHIVE= "../exc$(EXERCISE)_$(USERNAME).zip"

.PHONY: all
all: zip

.PHONY: zip
zip: clean
	$(RM) $(ARCHIVE)
	zip -r $(ARCHIVE) . --exclude $(EXCLUDE_PATTERNS)

.PHONY: clean
clean:
	@for dir in task?; do \
	  $(MAKE) -C "$$dir" clean; \
	done
