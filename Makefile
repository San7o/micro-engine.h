# SPDX-License-Identifier: MIT
# Author:  Giovanni Santini
# Mail:    giovanni.santini@proton.me
# Github:  @San7o

all: build

.PHONY: build
build:
	cmake -Bbuild
	cmake --build build

.PHONY: test
test: build
	./build/demo

.PHONY: html
html:
	cd docs/micro-website && ./generate.sh

.PHONY: emscriptem
emscriptem:
	emcmake cmake -Bbuild
	emmake make app_example -C build

.PHONY: wasm
wasm:
	clang --target=wasm32 -nodefaultlibs -nostdlib -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined -Wl,--initial-memory=4325376 -o build/wasm_demo.wasm examples/wasm_demo.c examples/micro-engine-impl.c -I ./include
	cp utils/wasm_platform.js build
	cp utils/wasm_template.html build
	sed -i "s|{{{ SCRIPT }}}|<script src=\"wasm_platform.js\"></script>|g" build/wasm_template.html
	sed -i "s|{{{ MODULE_NAME }}}|wasm_demo|g" build/wasm_platform.js
