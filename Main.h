#pragma once
#include "FrameBuffer.h"

int color = 0;

void Setup(FrameBuffer* fb, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Pixel(fb, x, y) = RGB888(255, 0, 0);
		}
	}
}

void Update(FrameBuffer* fb, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Pixel(fb, x, y) = RGB888(color % 255, color % 255, color % 255);
		}
	}
	color += 1;
}

void OnDestroy(FrameBuffer* fb, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Pixel(fb, x, y) = RGB888(0, 255, 0);
		}
	}
}
