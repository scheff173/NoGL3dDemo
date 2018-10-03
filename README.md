# No-GL 3D Renderer

(If you ask &ldquo;Why?&rdquo; you missed the point.)

This was inspired by a Stack Overflow question [software rasterization implementation speed up ideas](https://stackoverflow.com/a/52534685/7478597). In the answer I provided, I left out any color interpolation to gain the maximum speed. So, I became curious about how much certain options like

- color interpolation
- depth buffering / testing
- texture mapping
- lighting

might cost.

So, I prepared this project.

![Teaser (sequence of snapshots)](./snap-NoGL3d.gif)

It appears to be less useful nowadays where nearly every computer (incl. smartphones) seems to have a GPU.
On the other hand, such things (which I still learned and practiced while I was studying at University) are usually under the hood &ndash; at best, somebody can imagine how it may work.

So, I came to the idea that it might be funny to do this again as if there were no GPU &ndash; for my own joy and education and, may be, for somebody else too who is curious about this.

## Renderer

@todo transformation of coordinates into screen space

@todo transformation of normals into view space

![Transformations for coordinates to screen space](./Sketch-mat.png)

### Lighting

This is the most simple kind of lighting which is imaginable:

- only global light (with infinite distance of light source) supported
- lighting calculations per vertex normals only.

Thus, the result of lighting is applied to vertex colors.

The colors of rasterized vertices are simply interpolated.
This is called [Gouraud-Shading](https://en.wikipedia.org/wiki/Gouraud_shading) &ndash;
the most simple (and most ugly) form of lighting calculation I know but also the one with the least performance impact.

Thereby, the angle of normal to light vector is evaluated.

cos(&alpha;) = (normal &middot; light) / (|normal| &middot; |light|)

Assuming that `normal` as well as `light` should be normalized, this simplifies to

cos(&alpha;) = (normal &middot; light)

The nice fact about this is that it's not necessary to call the &ldquo;expensive&rdquo; `acos()` in this calculation.
For my purpose, the cos(&alpha;) value is fully sufficient. It is:

- 1 if `normal` and `light` are identical (i.e. sun shines vertical onto face)
- between 0 and 1 for angle of `normal` and `light` &lt; 90 deg.
- 0 if `normal` and `light` are orthogonal (i.e. sun shines along face)
- &lt; 0 for angle of `normal` and `light` &gt; 90 deg.

Angles &gt; 90 deg. are irrelvant &ndash; a face in shadow is simply dark.

Hence, the result of (normal &middot; light) is simply multiplied with the color value.

The fixed-pipe OpenGL engine supports additional lighting effects like:

- specular lighting
- point lights.

For now, I ignored this.
I got the impression that the absence of these things covers quite good the weakness of Gouraud shading.

## Rasterizer

When the rasterizer is called vertex coordinates are already transformed into screen space.

### Interpolation of Coordinates

This means

- x coordinate of a visible pixel must be in the range \[0, width)
- y coordinate of a visible pixel must be in the range \[0, height)
- z coordinate might be considered for depth buffering / depth test.

The `RenderContext::rasterize()` function has to fill horizontal screen lines. For this, I sort the 3 triangle vertices by their y components. In regular case, this allows to cut the triangle horizontally into two parts:

- the upper with peak above of horizontal base
- the lower with peak below of horizontal base.

The horizontal base is on one end the middle vertex, on the other end the horizontally project point on the line from top to bottom vertex. For this constructed vertex, the color and texture coordinates are interpolated according to the ratio (y<sub>M</sub> - y<sub>T</sub>) / (y<sub>B</sub> - y<sub>M</sub>).

![Sketch of interpolation in RenderContext::rasterize()](https://i.stack.imgur.com/j9sdu.png)

Border cases like

- y<sub>T</sub> = y<sub>M</sub>
- y<sub>M</sub> = y<sub>B</sub> or even
- y<sub>T</sub> = y<sub>M</sub> = y<sub>B</sub>

are simply covered by tests which may skip the upper, lower, or even both parts.

### Shading

As the lighting calculations are applied to the vertex colors, there is no distinction between

- color interpolation (interpolating pixel colors according to the distance of this pixel to the vertices and their associated colors)
- shading (modification of vertex colors according to lighting).

If smooth rasterization is enabled (`RenderContext::Smooth`) pixel colors are interpolated for each pixel (x, y).

If lighting is enabled additionally (`RenderContext::Lighting`) vertex colors are pre-modified according to angle of corresponding vertex normals and light vector. (This is done prior to rasterizing.)

Lighting enabled while smooth rasterization results in flat shading. (This makes the demo sphere similar looking like a golf ball.)

In modern OpenGL (with GLSL), lighting can be done per pixel. This allows there natural looking effects.
In my case, it would require expensive calculations in the most inner loops of `RenderContext::rasterize()`.
If I understood it right, this was the actual intention of the OP in [software rasterization implementation speed up ideas](https://stackoverflow.com/a/52534685/7478597) where he complained about the miserable frame rate. I believe a per-pixel lighting is simply too heavy for a software renderer (at least if a multi frame-per-second rate is intended).

### Texturing

OpenGL provides various texture filtering options whereby two situations are distinguished:

- a texel covers multiple pixels
- a pixel covers multiple texels.

Respectively, OpenGL supports

- magnification filters and
- minification filters.

For best performance, I didn't implement any texture filtering. Thus, in both situations the nearest texel is chosen.
This means the U-V coordinates (associated two vertices) are simply interpolated.

The interpolated U-V coordinates are used with my overloaded `Texture::operator[]`.
It subtracts the integer part from the floating point value yielding the decimals (effectively a value in range [0, 1]).
This value is multiplied with the width (u) or height (v), casted to `unsigned int` and provides thus the indices to access the texture image.

## Optimization Attempts

While developing and testing, it became very obvious to me that optimization pays off best in the most inner loops of the `RenderContext::rasterize()` (i.e. the two iterations over x).
(Unfortunately, I cannot remember the title of the book where I read this nor the author but I memorized it as a good rule of thumb: &ldquo;If you try to optimize then optimize the most inner loops and leave any other loop as is.&rdquo;)
Therefore, I intended to remove branches from this most inner loops as most as possible.
According to the different rendering modes, there are still certain options which might be enabled or disabled.
Therefore, I made all these options as template parameters of `rasterize()`.
Hence, when `rasterize()` is compiled all these conditions check constant values, and a modern compiler should simply remove the `if()` check where the body is compiled in or left out depending on condition.
So, I need a &ldquo;flavor of&rdquo; `RenderContext::rasterize()` for every possible combination of template arguments.
This is found in `RenderContext::drawVertex()` where I made an array `rasterizes[]` initialized with template function instances iterated through every combination of possible template argument values.
Afterwards, `RenderContext::drawVertex()` computes the table index `i` combining all relevant modes accordingly.
Thus, the conditions which appear inside of `RenderContext::rasterize()` are actually resolved outside.

<!-- @todo mention Bresenham? -->

<!-- @todo ## Some Measured Values -->

