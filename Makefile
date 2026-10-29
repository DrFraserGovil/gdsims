BUILD_DIR = .build

.PHONY: all ship debug profile test purge clean mirror

MIRROR_FLAGS ?= -vf
MAKEFLAGS += --no-print-directory

# The build configurations live in CMakePresets.json (so IDEs see the same ones); these targets are shortcuts for them.
# Mirroring the settings with lsj is part of the CMake build.

# $(call build,<preset>,<build dir>)
# Configures the build dir (if it has never been configured, or a previous configure failed partway), then builds the preset's target.
define build
	@if [ ! -f "$(2)/CMakeCache.txt" ] || [ ! -f "$(2)/Makefile" ]; then \
		cmake --preset $(1); \
	fi
	@cmake --build --preset $(1) $(EXTRA_BUILD_FLAGS)
endef

all:
	$(call build,release,$(BUILD_DIR))

ship:
	$(call build,ship,$(BUILD_DIR)-ship)

debug:
	$(call build,debug,$(BUILD_DIR)-debug)

profile:
	$(call build,profile,$(BUILD_DIR)-profile)

test:
	$(call build,test,$(BUILD_DIR)-test)
	@ctest --preset test

# Manual mirror, without building
mirror:
	@command -v lsj>/dev/null 2>&1 && lsj src $(MIRROR_FLAGS) || echo "no mirror found, using cached values"

purge:
	@echo "Removing all build files"
	@rm -rf $(BUILD_DIR) $(BUILD_DIR)-*
	@rm -f gdsims gdsims_debug gdsims_profile tests/gdsims_tests

clean:
	@echo "Rebuilding from scratch"
	@$(MAKE) purge
	@$(MAKE) all
