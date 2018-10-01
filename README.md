# No-GL 3D Renderer

(If you ask &ldquo;Why?&rdquo; you missed the point.)

This was inspired by a Stack Overflow question [software rasterization implementation speed up ideas](https://stackoverflow.com/a/52534685/7478597). In the answer I provided, I left out any color interpolation to gain the maximum speed. So, I became curious about how much certain options like

- color interpolation
- depth buffering / testing
- texture mapping
- lighting

might cost.

So, I prepared this project.

It appears to be less useful nowadays where nearly every computer (incl. smartphones) seems to have a GPU.
On the other hand, such things (which I still learned and practiced while I was studying at University) are usually under the hood &ndash; at best, somebody can imagine how it may work.

So, I came to the idea that it might be funny to do this again as if there were no GPU &ndash; for my own joy and education and, may be, for somebody else too who is curious about this.

## Renderer

transformation of coordinates into screen space

transformation of normals into view space

## Rasterizer

<!-- take description and image from stackoverflow -->

interpolation of coordinates

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

Hence, the result of (normal &cdot; light) is simply multiplied with the color value.

### Shading

As the lighting calculations are applied to the vertex colors, there is no distinction between

- color interpolation (interpolating pixel colors according to the distance of this pixel to the vertices and their associated colors)
- shading (modification of vertex colors according to lighting).

If smooth rasterization is enabled (`RenderContext::Smooth`) pixel colors are interpolated for each pixel (x, y).
If lighting is enabled additionally (`RenderContext::Lighting`) vertex colors are pre-modified according to angle of corresponding vertex normals and light vector.

(Lighting enabled while smooth rasterization disabled simply has no effect.)

### Texture Filtering

OpenGL provides various texture filtering options whereby two situations are distinguished:

- a texel covers multiple pixels
- a pixel covers multiple texels.

Respectively, OpenGL supports

- magnification filters and
- minification filters.

For best performance, I didn't implement any filtering. Thus, in both situations the nearest texel is chosen.
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
