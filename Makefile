.PHONY: rast

rust:
	cmake --build build --target Rasterization

run-rust:
	cmake --build build --target Rasterization
	./build/Rasterization
