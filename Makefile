all: lib.a server.o
	g++ build/objects/release/server.o -o server -lpack109final -lhashset -Lbuild/lib/release -std=c++11
	mkdir -p build/bin/release 
	
# libpack109final.a: 
# 	g++ src/serialize.cpp -c -Iinclude -std=c++11
# 	ar rs libpack109final.a serialize.o 
# 	mkdir -p build/lib/release 
# 	mkdir -p build/objects/release 
# 	mv *.o build/objects/release 
# 	mv libpack109final.a build/lib/release 

# libhashset.a: 
# 	g++ src/serialize.cpp -c -Iinclude -std=c++11
# 	ar rs libhashset.a serialize.o 
# 	mkdir -p build/lib/release 
# 	mkdir -p build/objects/release 
# 	mv *.o build/objects/release 
# 	mv libhashset.a build/lib/release 

lib.a: 
	g++ src/serialize.cpp -c -Iinclude -std=c++11
	ar rs libpack109final.a serialize.o
	ar rs libhashset.a serialize.o
	mkdir -p build/lib/release
	mkdir -p build/objects/release 
	mv *.o build/objects/release 
	mv libpack109final.a build/lib/release 
	mv libhashset.a build/lib/release

server.o: 
	g++ src/server.cpp -c -lpack109final -lhashset -Lbuild/lib/release -Iinclude -std=c++11
	mkdir -p build/objects/release 
	mv server.o build/objects/release 

clean: 
	rm -f *.a
	rm -f *.o 
	rm -rf build