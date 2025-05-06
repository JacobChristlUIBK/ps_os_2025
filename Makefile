# Use this file at your own risk.
# Always ensure that the required files are actually put into the archive,
# and that no additional files are archived.
# You can use the zipinfo command to check the file structure of an existing archive.

# --- ADJUST BELOW ----------------------------------------------------------------------------------

# Number of people: set to 1, 2 or 3 (how many zips to create)
PEOPLE ?= 2

# Usernames for the zip filenames
USERNAMES = csbaXXXX csbbXXXX csbcXXXX

# Names and matriculation numbers for group.txt (only used if PEOPLE > 1)
# Underscores in PERSONS are replaced by spaces in the output.
PERSONS = Max_Mustermann Gordon_Freeman Alyx_Vance
MAT_NUMS = 12345678 87654321 98765432

# Exercise number / folder
EXERCISE = 04

# ---------------------------------------------------------------------------------------------------

# This excludes some common directories automatically.
# Also ignores all binaries and README.md.
EXCLUDE_PATTERNS = \
    "**.vscode/*" \
    "**.idea/*" \
    "**__MACOSX/*" \
    "**.DS_Store/*" \
    "**.dSYM/*" \
    "**/*.o" \
    "**/a.out" \
    "README.md"

# Derived list of archive filenames:
ARCHIVES = $(addprefix ./exc$(EXERCISE)_,$(addsuffix .zip,$(USERNAMES)))

# --- PHONY TARGETS -----------------

.PHONY: all prepare clean group format setperms zip

# all: run the full workflow
all: prepare zip

# prepare: clean, generate group.txt (if needed), format code, set perms
prepare: clean group format setperms

# clean: run each sub-task's clean, remove old group.txt
clean:
	@echo "Cleaning task folders in exercise$(EXERCISE)..."
	@for dir in ./exercise$(EXERCISE)/task_*; do \
	  if [ -d "$$dir" ]; then \
	    -$(MAKE) -C "$$dir" clean || echo "Warning: no clean target in $$dir"; \
	  fi; \
	done
	@rm -f exercise$(EXERCISE)/group.txt

# group: create group.txt if PEOPLE > 1, otherwise skip
group:
	@if [ $(PEOPLE) -le 1 ]; then \
	  echo "Single submission detected: skipping group.txt."; \
	else \
	  echo "Creating group.txt in exercise$(EXERCISE)..."; \
	  rm -f exercise$(EXERCISE)/group.txt; \
	  IDX=`seq 1 $(PEOPLE)`; \
	  for i in $$IDX; do \
	    mat=`echo "$(MAT_NUMS)"  | cut -d' ' -f $$i`; \
	    raw=`echo "$(PERSONS)" | cut -d' ' -f $$i`; \
	    name=`echo $$raw | tr '_' ' '`; \
	    echo "$$mat $$name" >> exercise$(EXERCISE)/group.txt; \
	  done; \
	fi

# format: apply clang-format to every .c file if clang-format is installed
format:
	@echo "Formatting .c files in exercise$(EXERCISE)..."
	@if command -v clang-format >/dev/null 2>&1; then \
	  find exercise$(EXERCISE) -type f -name "*.c" -exec clang-format -i {} \;; \
	else \
	  echo "clang-format not found, skipping formatting."; \
	fi

# setperms: make all files in the exercise world-readable
setperms:
	@echo "Setting world-readable permissions in exercise$(EXERCISE)..."
	@find exercise$(EXERCISE) -type f -exec chmod a+r {} \;

# zip: package into submission/*.zip for each person
zip: prepare
	@mkdir -p submission
	@IDX=`seq 1 $(PEOPLE)`; \
	for i in $$IDX; do \
	  archive=`echo "$(ARCHIVES)" | cut -d' ' -f $$i`; \
	  rm -f ./submission/$$archive; \
	  (cd exercise$(EXERCISE) && zip -r ../submission/$$archive . --exclude $(EXCLUDE_PATTERNS)); \
	done
