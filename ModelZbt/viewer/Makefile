all: viewer.js

viewer.js: ../m3dview/viewer.c ../m3dview/glfw.c ../m3d.h viewer.c
	emcc -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS='["_m3d_viewer","_malloc","_free"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s USE_GLFW=3 -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=0 -I../m3dview -I.. -I/usr/include/GL -I/usr/include/GLFW ../m3dview/glfw.c ../m3dview/viewer.c viewer.c -o viewer.js

clean:
	rm viewer.js viewer.wasm 2>/dev/null || true
