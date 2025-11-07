.PHONY: rast

rust:
	cmake --build build --target Rasterization

run-rust:
	cmake --build build --target Rasterization
	./build/Rasterization

ray:
	cmake --build build --target Raytracing

run-ray:
	cmake --build build --target Raytracing
	./build/Raytracing

