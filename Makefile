host:
	./blst/build.sh CC=clang CFLAGS=-O3
	mv ./libblst.a  ./native/libblst.a
	clang ./native/*.c ./native/libblst.a -o main
wasm:
	./blst/build.sh CC=clang CFLAGS=-O3 --target=wasm32 --no-standard-libraries
	clang *.c libblst.a -o bls.wasm --target=wasm32 --no-standard-libraries -mbulk-memory -Wl,--no-entry -Oz -Wl,--lto-O3 -fno-exceptions
	python remove.py