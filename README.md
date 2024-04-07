# Terminal Telescope

> Linux program that displays images in terminal and refreshes screen each time the file is written to externally or if the terminal size is adjusted.

## Compile & Run

```sh
g++ ./main.cpp
chmod +x ./a.out
./a.out <image filename>
```

## Dependencies

- ImageMagick is used for the `convert` program to execute within the system to convert a given image to the required `ppm` image file type.

## Custom Image

> You can create your own `ppm` images using the `Bitmap` struct within `./main.cpp`.

```cpp
Bitmap test;
test.filename = "test.ppm";
test.width = 256;
test.height = 256;
test.pixels.resize((test.width * test.height) * 3);
for (int i = 0; i < test.height; i++) {
	for (int j = 0; j < test.width; j++) {
		test(i, j, 0) = i ^ j;
		test(i, j, 1) = i ^ j;
		test(i, j, 2) = i ^ j;
	}
}
test.write();
```
