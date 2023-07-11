all: validator.js

validator.js: validator.c ../m3d.h ../m3dconv/dump.h
	emcc -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS='["_m3d_validate","_malloc","_free"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' validator.c -o validator.js

clean:
	rm validator.js validator.wasm 2>/dev/null || true
