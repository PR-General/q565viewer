# q565viewer
A visual conversion tool that will help test q565 implementations.

![Screenshot](docs/screenshot.png)

This tool can load in a file of (most) image file formats and convert it to a q565 (RGB16) image.
Often there will be few discernable differences between image but there are signifcantly fewer colors in RGB16.

The gain comes in the size of bytes the image needs to be communicated. 

Q565 includes multiple compression methods and this code currently implementations

**Supported Compression Features:**
[x] - Color Difference (describe a pixel as a difference from the prior pixel)
[x] - Color run, uses a single byte to describe up to 62 contiguous pixels of one color
[x] - Color Table (index a unique color value) to be referenced by ID (0..63)
[ ] - Lumocity difference
[ ] - Color table difference

