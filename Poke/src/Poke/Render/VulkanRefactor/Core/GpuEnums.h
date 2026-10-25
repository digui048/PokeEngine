#ifndef GPU_ENUMS_H
#define GPU_ENUMS_H

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Poke
{
    namespace ColorWriteEnabled
    {
        enum Enum {
            Red, Green, Blue, Alpha, All, Count
        };

        enum Mask {
            RedMask = 1 << 0, GreenMask = 1 << 1, BlueMask = 1 << 2, AlphaMask = 1 << 3, AllMask = RedMask | GreenMask | BlueMask | AlphaMask
        };
    }

    namespace CullMode
    {
        enum Enum {
            None, Front, Back, Count
        };

        enum Mask {
            NoneMask = 1 << 0, FrontMask = 1 << 1, BackMask = 1 << 2, CountMask = 1 << 3
        };
    }

    namespace FillMode
    {
        enum Enum {
            Wireframe, Solid, Point, Count
        };

        enum Mask {
            WireframeMask = 1 << 0, SolidMask = 1 << 1, PointMask = 1 << 2, CountMask = 1 << 3
        };
    }

    namespace FrontClockWise
    {
        enum Enum {
            True, False, Count
        };

        enum Mask {
            TrueMask = 1 << 0, FalseMask = 1 << 1, CountMask = 1 << 2
        };
    }

    namespace StencilOperation
    {
        enum Enum {
            Keep, Zero, Replace, IncrSat, DecrSat, Invert, Incr, Decr, Count
        };

        enum Mask {
            KeepMask = 1 << 0, ZeroMask = 1 << 1, ReplaceMask = 1 << 2, IncrSatMask = 1 << 3, DecrSatMask = 1 << 4, InvertMask = 1 << 5, IncrMask = 1 << 6, DecrMask = 1 << 7, CountMask = 1 << 8
        };
    }

    namespace TopologyType
    {
        enum Enum {
            Unknown, Point, Line, Triangle, Patch, Count
        };

        enum Mask {
            UnknownMask = 1 << 0, PointMask = 1 << 1, LineMask = 1 << 2, TriangleMask = 1 << 3, PatchMask = 1 << 4, CountMask = 1 << 5
        };
    }

    namespace ResourceUsageType
    {
        enum Enum {
            Immutable, Dynamic, Stream, Count
        };

        enum Mask {
            ImmutableMask = 1 << 0, DynamicMask = 1 << 1, StreamMask = 1 << 2, CountMask = 1 << 3
        };
    }

    namespace IndexType
    {
        enum Enum {
            Uint16, Uint32, Count
        };

        enum Mask {
            Uint16Mask = 1 << 0, Uint32Mask = 1 << 1, CountMask = 1 << 2
        };
    }

    namespace TextureType
    {
        enum Enum {
            Texture1D, Texture2D, Texture3D, Texture1DArray, Texture2DArray, TextureCubeArray, Count
        };

        enum Mask {
            Texture1DMask = 1 << 0, Texture2DMask = 1 << 1, Texture3DMask = 1 << 2, Texture1DArrayMask = 1 << 3, Texture2DArrayMask = 1 << 4, TextureCubeArrayMask = 1 << 5, CountMask = 1 << 6
        };
    }

    namespace VertexComponentFormat
    {
        enum Enum {
            Float, Float2, Float3, Float4, Mat4, Byte, Byte4N, UByte, UByte4N, Short2, Short2N, Short4, Short4N, Uint, Uint2, Uint3, Uint4, Count
        };
    }

    namespace VertexInputRate
    {
        enum Enum {
            PerVertex, PerInstance, Count
        };

        enum Mask {
            PerVertexMask = 1 << 0, PerInstanceMask = 1 << 1, CountMask = 1 << 2
        };
    }

    namespace LogicOperation 
    {
        enum Enum {
            Clear, Set, Copy, CopyInverted, Noop, Invert, And, Nand, Or, Nor, Xor, Equiv, AndReverse, AndInverted, OrReverse, OrInverted, Count
        };

        enum Mask {
            ClearMask = 1 << 0, SetMask = 1 << 1, CopyMask = 1 << 2, CopyInvertedMask = 1 << 3, NoopMask = 1 << 4, InvertMask = 1 << 5, AndMask = 1 << 6, NandMask = 1 << 7, OrMask = 1 << 8, NorMask = 1 << 9, XorMask = 1 << 10, EquivMask = 1 << 11, AndReverseMask = 1 << 12, AndInvertedMask = 1 << 13, OrReverseMask = 1 << 14, OrInvertedMask = 1 << 15, CountMask = 1 << 16
        };
    }

    namespace QueueType
    {
        enum Enum {
            Graphics, Compute, CopyTransfer, Count
        };

        enum Mask {
            GraphicsMask = 1 << 0, ComputeMask = 1 << 1, CopyTransferMask = 1 << 2, CountMask = 1 << 3
        };
    }

    namespace CommandType
    {
        enum Enum {
            BindPipeline, BindResourceTable, BindVertexBuffer, BindIndexBuffer, BindResourceSet, Draw, DrawIndexed, DrawInstanced, DrawIndexedInstanced, Dispatch, CopyResource, SetScissor, SetViewport, Clear, ClearDepth, ClearStencil, BeginPass, EndPass, Count
        };
    }

    enum DeviceExtensions {
        DeviceExtensions_DebugCallback = 1 << 0,
    };

    namespace TextureFlags {
        enum Enum {
            Default, RenderTarget, Compute, Count
        };

        enum Mask {
            DefaultMask = 1 << 0, RenderTargetMask = 1 << 1, ComputeMask = 1 << 2
        };
    }

    namespace PipelineStage
    {
        enum Enum {
            DrawIndirect, VertexInput, VertexShader, FragmentShader, RenderTarget, ComputeShader, Transfer
        };

        enum Mask {
            DrawIndirectMask = 1 << 0, VertexInputMask = 1 << 1, VertexShaderMask = 1 << 2, FragmentShaderMask = 1 << 3, RenderTargetMask = 1 << 4, ComputeShaderMask = 1 << 5, TransferMask = 1 << 6
        };
    }

    namespace RenderPassType
    {
        enum Enum {
            Geometry, Swapchain, Compute
        };
    }

    namespace ResourceDeletionType
    {
        enum Enum {
            Buffer, Texture, Pipeline, Sampler, DescriptorSetLayout, DescriptorSet, RenderPass, ShaderState, Count
        };
    }

    namespace PresentMode
    {
        enum Enum {
            Immediate, VSync, VSyncFast, VSyncRelaxed, Count
        };
    }

    namespace RenderPassOperation 
    {
        enum Enum {
            DontCare, Load, Clear, Count
        };
    }

    namespace TextureFormat
    {
        inline bool IsDepthStencil(VkFormat value) {
            return value == VK_FORMAT_D16_UNORM_S8_UINT || value == VK_FORMAT_D24_UNORM_S8_UINT || value == VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        inline bool IsDepthOnly(VkFormat value) {
            return value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_D32_SFLOAT;
        }
        inline bool IsStencilOnly(VkFormat value) {
            return value == VK_FORMAT_S8_UINT;
        }

        inline bool HasDepth(VkFormat value) {
            return (value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_S8_UINT ) || (value >= VK_FORMAT_D16_UNORM_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT);
        }
        inline bool HasStencil(VkFormat value) {
            return value >= VK_FORMAT_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        inline bool HasDepthOrStencil(VkFormat value) {
            return value >= VK_FORMAT_D16_UNORM && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
    }
}

#endif