native:
	g++ -I. main.c region_detect.c mobj_parse.c util/bits.c file/file_posix.c -o region_detect

web:
	emcc -I. region_detect_web.c region_detect.c mobj_parse.c util/bits.c file/file_wasm.c -s WASM=1 -O3 -o region_detect_web.js
