#ifndef GPU_ENUMS_H
#define GPU_ENUMS_H

#include <cstdint>

namespace Poke
{
    // Resource and GPU
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

    // Texture and Image formats
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

    // Depth and stencil
    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqueal,
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

    // Blending
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

    // Vertex input
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

    // Resource states
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

    // Pipeline stages
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

    // Load and store operations
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
}

#endif