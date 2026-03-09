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

.PHONY: wasm
wasm:
	emcmake cmake -Bbuild
	emmake make app_example -C build
