all: fmt build/Makefile build/Release/mmap.node lib/index.js

run: all
	node .

fmt:
	clang-format --style=WebKit -i src/*

build/Makefile:
	yarn

build/Release/mmap.node: $(wildcard src/*.cc)
	node-gyp build

lib/index.js: src/index.ts
	tsc
