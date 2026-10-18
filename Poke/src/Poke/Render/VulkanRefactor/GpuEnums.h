#ifndef GPU_ENUMS_H
#define GPU_ENUMS_H

#include <cstdint>

namespace Poke
{
    enum class QueueType
    {
        Graphics,
        Compute,
        Transfer
    };

    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Compute
    };

    enum class ShaderStageFlags : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        All = Vertex | Fragment | Compute
    };

    enum class TextureType
    {
        Texture1D,
        Texture2D,
        Texture3D,
        Texture1DArray,
        Texture2DArray,
        Cube,
        CubeArray
    };

    enum class ResourceUsage
    {
        Immutable,
        Dynamic,
        Stream
    };

    enum class Format
    {
        Undefined,

        // Color
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,

        RG8_UNORM,
        RG8_SNORM,
        RG8_UINT,
        RG8_SINT,

        RGBA8_UNORM,
        RGBA8_SRGB,
        RGBA8_UINT,
        RGBA8_SINT,

        BGRA8_UNORM,
        BGRA8_SRGB,

        R16_FLOAT,
        RG16_FLOAT,
        RGBA16_FLOAT,

        R32_FLOAT,
        RG32_FLOAT,
        RGB32_FLOAT,
        RGBA32_FLOAT,

        // Depth
        D16_UNORM,
        D32_FLOAT,

        // Depth and stencil
        D24_UNORM_S8_UINT,
        D32_FLOAT_S8_UINT
    };

    // Buffer types and usages
    enum class BufferUsage : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
        Indirect = 1 << 4,
        TransferSource = 1 << 5,
        TransferDest = 1 << 6,
    };

    // Texture usage
    enum class TextureUsage : uint32_t
    {
        None = 0,
        Sampled = 1 << 0,
        Storage = 1 << 1,
        RenderTarget = 1 << 2,
        DepthStencil = 1 << 3,
        TransferSource = 1 << 4,
        TransferDest = 1 << 5,
    };

    // Pipeline
    enum class PrimitiveTopology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        PatchList
    };

    enum class PolygonMode
    {
        Fill,
        Line,
        Point
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class FrontFace
    {
        CounterClockwise,
        Clockwise
    };

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class StencilOp
    {
        Keep,
        Zero,
        Replace,
        IncrementClamp,
        DecrementClamp,
        Invert,
        IncrementWrap,
        DecrementWrap
    };

    enum class BlendFactor
    {
        Zero,
        One,

        SourceColor,
        OneMinusSourceColor,
        DestinationColor,
        OneMinusDestinationColor,

        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha,

        ConstantColor,
        OneMinusConstantColor,

        ConstantAlpha,
        OneMinusConstantAlpha,

        SourceAlphaSaturate
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class VertexInputRate
    {
        PerVertex,
        PerInstance
    };

    enum class VertexFormat
    {
        Float,
        Float2,
        Float3,
        Float4,

        Int,
        Int2,
        Int3,
        Int4,

        UInt,
        UInt2,
        UInt3,
        UInt4,
    };

    enum class ResourceState
    {
        Undefined,

        CopySource,
        CopyDestination,

        VertexBuffer,
        IndexBuffer,

        ConstantBuffer,

        ShaderRead,
        ShaderWrite,

        RenderTarget,

        DepthWrite,
        DepthRead,

        Present
    };

    enum class PipelineStage
    {
        TopOfPipe,

        DrawIndirect,

        VertexInput,
        VertexShader,

        FragmentShader,

        ComputeShader,

        ColorAttachment,
        DepthStencil,

        Transfer,

        BottomOfPipe
    };

    enum class LoadOp
    {
        Load,
        Clear,
        DontCare
    };
    enum class StoreOp
    {
        Store,
        DontCare
    };

    enum class Filter
    {
        Nearest,
        Linear
    };

    enum class MipmapMode
    {
        Nearest,
        Linear
    };

    enum class AdressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class DescriptorType
    {
        Sampler,
        CombinedImageSampler,
        SampledImage,
        StorageImage,
        UniformBuffer,
        StorageBuffer
    };

    enum class PresentMode
    {
        Immediate,
        VSync,
        VSyncRelaxed
    };

    enum class RenderPassType
    {
        Geometry,
        Swapchain,
        Compute
    };

    enum class ColorWriteMask : uint32_t
    {
        None = 0,
        Red = 1 << 0,
        Green = 1 << 1,
        Blue = 1 << 2,
        Alpha = 1 << 3,
        All = Red | Green | Blue | Alpha
    };

    template <typename T>
    inline T GpuFlagsOr(T a, T b)
    {
        return static_cast<T>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    template <typename T>
    inline T GpuFlagsAnd(T a, T b)
    {
        return static_cast<T>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline ShaderStageFlags operator|(ShaderStageFlags a, ShaderStageFlags b) { return GpuFlagsOr(a, b); }
    inline ShaderStageFlags operator&(ShaderStageFlags a, ShaderStageFlags b) { return GpuFlagsAnd(a, b); }
    inline ShaderStageFlags operator|=(ShaderStageFlags a, ShaderStageFlags b) { a = a | b; return a; }

    inline BufferUsage operator|(BufferUsage a, BufferUsage b) { return GpuFlagsOr(a, b); }
    inline BufferUsage operator&(BufferUsage a, BufferUsage b) { return GpuFlagsAnd(a, b); }
    inline BufferUsage operator|=(BufferUsage a, BufferUsage b) { a = a | b; return a; }

    inline TextureUsage operator|(TextureUsage a, TextureUsage b) { return GpuFlagsOr(a, b); }
    inline TextureUsage operator&(TextureUsage a, TextureUsage b) { return GpuFlagsAnd(a, b); }
    inline TextureUsage operator|=(TextureUsage a, TextureUsage b) { a = a | b; return a; }

    inline ColorWriteMask operator|(ColorWriteMask a, ColorWriteMask b) { return GpuFlagsOr(a, b); }
    inline ColorWriteMask operator&(ColorWriteMask a, ColorWriteMask b) { return GpuFlagsAnd(a, b); }
    inline ColorWriteMask operator|=(ColorWriteMask a, ColorWriteMask b) { a = a | b; return a; }
}

#endif