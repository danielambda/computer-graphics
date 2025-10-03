.PHONY: rast

rust:
	cmake --build build --target Rasterization

run-rust:
	./build/Rasterization
