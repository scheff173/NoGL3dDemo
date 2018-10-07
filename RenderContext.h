/** @file
 * interface of class RenderContext
 */

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

// standard C++ header:
#include <cstdint>
#include <functional>
#include <vector>

// own header:
#include "linmath.h"
#include "Texture.h"
#include "util.h"

/// @todo clip space culling

/** provides a class for the 3d render context.
 *
 * This is actually the 3d rendering engine managing
 * - rendering states
 * - output buffers
 * - rasterizing of triangles into output buffers.
 */
class RenderContext {
  
  // types:
  public:

    /// rendering modes
    enum Mode {
      FrontSide, ///< render front face of triangles
      BackSide, ///< render back face of triangles
      DepthBuffer, ///< depth buffer (writing)
      DepthTest, ///< depth testing
      Smooth, ///< per-pixel color interpolation
      Blending, ///< alpha blending
      Texturing, ///< texturing
      Lighting, ///< lighting
      NModes ///< number of modes
    };

  private:

    /// depth mode
    enum DepthMode {
      NoDepth, ///< depth buffer off
      DepthWrite, ///< write depth values but don't test
      DepthCheckAndWrite ///< write and test depth values
    };

    /// vertex
    struct Vertex {
      Vec3f coord; ///< 3d coordinate
      Vec3f normal; ///< vertex normal
      Vec4f color; ///< vertex color
      Vec2f texCoord; ///< texture coordinates associated to vertex
    };

  // variables:
  private:
    /// width and height of frame buffers
    uint _width, _height;
    /// current clear color
    uint32 _rgbaClear;
    /// current clear depth value
    float _depthClear;
    /// the frame buffer
    struct FrameBuffer {
      std::vector<uint32> rgba; ///< frame buffer for colors
      std::vector<float> depth; ///< frame buffer for depth values
      /// constructor.
      FrameBuffer(uint size, uint32 rgba, float depth):
        rgba(size, rgba), depth(size, depth)
      { }
      /// destructor.
      ~FrameBuffer() = default;
      // disabled:
      FrameBuffer(const FrameBuffer&) = delete;
      FrameBuffer& operator=(const FrameBuffer&) = delete;
    } _fb;
    Mat4x4f _matScreen;
    Mat4x4f _matProj;
    Mat4x4f _matView;
    Mat4x4f _matCam;
    Mat4x4f _matModel;
    /// current normal used in drawVertex()
    Vec3f _normal;
    /// current color used in drawVertex()
    Vec4f _color;
    /// current texture coordinate used in drawVertex()
    Vec2f _texCoord;
    /// light vector (of directed light)
    Vec3f _light;
    /// ratio of ambient light (to directed light) in [0, 1]
    float _ambient;
    /// bit mask with combination of current modes
    uint _mode;
    /** index of currently bound texture
     *
     * The index may only address textures which have been loaded before.
     * Index 0 means none texture.
     * Actually, which refers to a 1x1 texture with black color and full
     * transparency.
     */
    uint _iTex;
    /// loaded textures
    std::vector<Texture> _tex;
    /// vertices of triangle to rasterize
    Vertex _vtcs[3];
    uint _iVtx;

    /// render callback
    std::function<void(RenderContext&)> _cbRender;

  // methods:
  public:
    /// @name Construction & Destruction
    //@{

    /** constructor.
     *
     * @param width width of frame buffers (in pixels)
     * @param height height of frame buffers (in pixels)
     */
    RenderContext(uint width, uint height);

    /// destructor.
    ~RenderContext() = default;

    // disabled:
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    //@}
  public:
    /// @name Public Access
    //@{

    /** returns width of viewport.
     *
     * @return width of viewport (in pixels)
     */
    uint getViewportWidth() const { return _width; }
    /** returns height of viewport.
     *
     * @return height of viewport (in pixels)
     */
    uint getViewportHeight() const { return _height; }

    /** @todo future extension?
     */
    void setViewport(uint width, uint height);

    /** returns current projection matrix.
     *
     * @return current projection matrix
     */
    Mat4x4f& getProjMat() { return _matProj; }
    /** returns current projection matrix.
     *
     * @return current projection matrix (read-only)
     */
    const Mat4x4f& getProjMat() const { return _matProj; }

    /** returns current view matrix.
     *
     * @return current view matrix (read-only)
     */
    const Mat4x4f& getViewMat() const { return _matView; }
    /** sets a new view matrix.
     *
     * @note
     * This will change the camera matrix as well.
     *
     * @param mat the new view matrix
     */
    void setViewMat(const Mat4x4f &mat);

    /** returns current camera matrix.
     *
     * @return current camera matrix (read-only)
     */
    const Mat4x4f& getCamMat() const { return _matCam; }
    /** sets a new camera matrix.
     *
     * @note
     * This will change the view matrix as well.
     *
     * @param mat the new camera matrix
     */
    void setCamMat(const Mat4x4f &mat);

    /** returns current model matrix.
     *
     * @return current model matrix
     */
    Mat4x4f& getModelMat() { return _matModel; }
    /** returns current model matrix.
     *
     * @return current model matrix (read-only)
     */
    const Mat4x4f& getModelMat() const { return _matModel; }

    /** returns whether a certain mode is enabled.
     *
     * @param mode the mode to be retrieved
     * @return true ... mode @mode is enabled\n
     *         false ... otherwise
     */
    bool isEnabled(Mode mode) const { return (_mode & (1 << mode)) != 0; }
    /** enables a certain mode.
     *
     * @param mode the mode to change
     * @param enable flag: true ... enable, false ... disable
     */
    void enable(Mode mode, bool enable = true);
    /** enables a certain mode.
     *
     * @param mode the mode to disable
     */
    void disable(Mode mode) { enable(mode, false); }

    /** returns the current ambient light brightness.
     *
     * @return current ambient light factor
     */
    float getAmbient() const { return _ambient; }
    /** sets a new ambient light brightness.
     *
     * @parent ambient new ambient light factor (clamped to [0, 1])
     */
    void setAmbient(float ambient)
    {
      _ambient = clamp(ambient, 0.0f, 1.0f);
    }

    /** returns current normal.
     *
     * @return current normal
     */
    const Vec3f& getNormal() const { return _normal; }
    /** sets a new normal.
     *
     * @note
     * The last set normal becomes effective in the next
     * drawVertex() call.
     *
     * @param normal the normal to set
     */
    void setNormal(const Vec3f &value) { _normal = value; }

    /** returns current color.
     *
     * @return current color
     */
    const Vec4f& getColor() const { return _color; }
    /** sets a new color.
     *
     * @note
     * The last set color becomes effective in the next
     * drawVertex() call.
     *
     * @param color the color to set\n
     *        Color components are applied in the following way
     *        - .x ... red
     *        - .y ... green
     *        - .z ... blue
     *        - .w ... alpha (opacity)
     */
    void setColor(const Vec4f &value) { _color = value; }

    /** returns current texture coordinate.
     *
     * @return current texture coordinate
     */
    const Vec2f& getTexCoord() const { return _texCoord; }
    /** sets a new texture coordinate.
     *
     * @note
     * The last set texture coordinate becomes effective in the next
     * drawVertex() call.
     *
     * @param texCoord the normal to set
     */
    void setTexCoord(const Vec2f &value) { _texCoord = value; }

    /** draws a vertex.
     *
     * @note
     * The provided coordinates are associated with last set
     * normal, color, and texture coordinates.
     *
     * @param coord the vertex coordinates
     *        The coordinates are relative to the current model coordinate
     *        system (set in model matrix).
     */
    void drawVertex(const Vec3f &coord);

    /** loads a texture from an image.
     *
     * @param width width of image (must be a power of 2)
     * @param height height of image (must be a power of 2)
     * @param img the color values of image
     *        (from top/left to bottom/right corner)\n
     *        The image @a img has to provide at least width * height RGBA
     *        values.
     *        Thereby, in each element R (red) has to be stored in the
     *        least significant byte, A (alpha) in the most significant.
     * @return 0 ... texture not loaded\n
     *         else ... index of loaded texture
     */
    uint loadTex(uint width, uint height, const uint32 img[]);
    /** returns current texture index.
     *
     * @return current texture index (0 ... no texture)
     */
    uint getTex() const { return _iTex; }
    /** sets current texture index.
     *
     * @param i texture index\n
     *        Only 0 or texture indices returned by loadTex() are
     *        accepted. Otherwise, current texture is set to 0.
     */
    uint setTex(uint i);

    void setClearColor(const Vec4f &color);
    void clear(bool rgba, bool depth);

    void setRenderCallback(std::function<void(RenderContext&)> &&cbRender)
    {
      _cbRender = cbRender;
    }

    void render() { _cbRender(*this); }

    /** returns the start address of RGBA frame buffer.
     *
     * @return start address of RGBA frame buffer\n
     *         Each element stores R (red) in the least significant byte,
     *         A (alpha) in the most significant.
     */
    const uint32* getRGBA() const { return _fb.rgba.data(); }

    //@}
  private:
    /// @name Internal Stuff
    //@{
    /** returns frame buffer index for a certain row.
     *
     * @param y index of row
     * @return index of first pixel in row @a y
     */
    uint getFBI(uint y) const { return y * _width; }

    /** returns frame buffer index for a certain pixel.
     *
     * @param x index of column (in row)
     * @param y index of row
     * @return index of pixel at (@a x, @a y)
     */
    uint getFBI(uint x, uint y) const { return y * _width + x; }

    /** rasterizes one triangle.
     *
     * @tparam DEPTH_MODE the depth mode
     * @tparam SMOOTH flag: true ... enable color interpolation
     * @tparam BLEND flag: true ... enable alpha blending
     * @tparam TEX flag: true ... enable texture sampling
     *
     * @param vtcs the vertices of triangle to rasterize\n
     *        This vertices are expected in screen space.
     */
    template <
      DepthMode DEPTH_MODE,
      bool SMOOTH,
      bool BLEND,
      bool TEX>
    void rasterize(const Vertex (&vtcs)[3]);

    //@}
};

#endif // RENDER_CONTEXT_H