BUILD_DIR = .build

.PHONY: all ship debug profile test purge clean mirror

MIRROR_FLAGS ?= -vf
MAKEFLAGS += --no-print-directory

# $(call build,<build dir>,<build type>,<target>,<extra cmake flags>[,<mirror flags>])
# Mirrors settings, configures the build dir (if it has never been configured, or a previous configure failed partway), then builds the target.
define build
	@$(MAKE) mirror MIRROR_FLAGS=$(or $(5),-fq)
	@if [ ! -f "$(1)/CMakeCache.txt" ] || [ ! -f "$(1)/Makefile" ]; then \
		cmake -S . -B $(1) -DCMAKE_BUILD_TYPE=$(2) $(4); \
	fi
	@cmake --build $(1) --config $(2) --target $(3) $(EXTRA_BUILD_FLAGS)
endef

all:
	$(call build,$(BUILD_DIR),Release,gdsims,-DFORCE_JSL_CLONE=OFF)

ship:
	$(call build,$(BUILD_DIR)-ship,Release,gdsims,-DFORCE_JSL_CLONE=ON,$(MIRROR_FLAGS))

debug:
	$(call build,$(BUILD_DIR)-debug,Debug,gdsims,-DFORCE_JSL_CLONE=OFF)

profile:
	$(call build,$(BUILD_DIR)-profile,RelWithDebInfo,gdsims,-DFORCE_JSL_CLONE=OFF)

test:
	$(call build,$(BUILD_DIR)-test,Debug,gdsims_tests,-DFORCE_JSL_CLONE=OFF -DGDSIMS_BUILD_TESTS=ON)
	@ctest --test-dir $(BUILD_DIR)-test -C Debug --output-on-failure

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
