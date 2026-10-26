BUILD_DIR = .build

.PHONY: all ship purge clean mirror

MIRROR_FLAGS ?= -vf
MAKEFLAGS += --no-print-directory

all:
	@$(MAKE) mirror MIRROR_FLAGS=-fq
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		mkdir -p $(BUILD_DIR); \
		cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DFORCE_JSL_CLONE=OFF; \
	fi
	@cmake --build $(BUILD_DIR)  --config Release --target gdsims $(EXTRA_BUILD_FLAGS) 
	 
mirror:
	@command -v lsj>/dev/null 2>&1 && lsj src/Settings/*.h $(MIRROR_FLAGS) || echo "no mirror found, using cached values" 
ship:
	@$(MAKE) mirror
	@mkdir -p $(BUILD_DIR)-ship
	@cmake -S . -B $(BUILD_DIR)-ship -DFORCE_JSL_CLONE=ON -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(BUILD_DIR)-ship --config Release --target gdsims

purge:
	@echo "Removing all build files"
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BUILD_DIR)-ship
	@rm -f gdsims

clean:
	@echo "Rebuilding from scratch"
	@$(MAKE) purge
	@$(MAKE) all 
