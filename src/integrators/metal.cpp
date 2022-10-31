//
// Created by goksu on 6/7/19.
//

#include "integrators/metal.hpp"

#include "Foundation/Foundation.hpp"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <simd/simd.h>

namespace soleil {

typedef struct
{
    vector_float4 position;
    vector_float2 texCoord;
} VertexData;

#pragma mark - Renderer
#pragma region Renderer {

mtk_renderer::mtk_renderer(MTL::Device* dev)
: device(dev->retain())
{
    command_queue = device->newCommandQueue();
    
    build_shaders();
    build_buffers();
}

mtk_renderer::~mtk_renderer()
{
    device->release();
    command_queue->release();
    pipeline_state->release();
    vertex_pos_buffer->release();
    vertex_color_buffer->release();
}

void mtk_renderer::draw(MTK::View* view)
{
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();
    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* rce = command_buffer->renderCommandEncoder(rpd);
    
    rce->setRenderPipelineState(pipeline_state);
    rce->setVertexBuffer(vertex_pos_buffer, 0, 0);
    rce->setVertexBuffer(vertex_color_buffer, 0, 1);
    rce->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    rce->endEncoding();
    command_buffer->presentDrawable( view->currentDrawable() );
    command_buffer->commit();

    pool->release();
}

void mtk_renderer::build_shaders() {
    using NS::StringEncoding::UTF8StringEncoding;
    
    const char* shader_source = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct v2f
        {
            float4 position [[position]];
            half3 color;
        };

        v2f vertex vertexMain(uint vertexId [[vertex_id]],
                              device const float3* positions [[buffer(0)]],
                              device const float3* colors [[buffer(1)]])
        {
            v2f o;
            o.position = float4(positions[vertexId], 1.0);
            o.color = half3(colors[vertexId]);
            return o;
        }

        half4 fragment fragmentMain(v2f in [[stage_in]])
        {
            return half4(in.color, 1.0);
        }
    )";

    NS::Error* error = nullptr;
    MTL::Library* library = device->newLibrary(NS::String::string(shader_source, UTF8StringEncoding), nullptr, &error);
    if (!library)
    {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        exit(1);
    }
    
    MTL::Function* vertex_func = library->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function* fragment_func = library->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

    MTL::RenderPipelineDescriptor* pipeline_desc = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_desc->setVertexFunction(vertex_func);
    pipeline_desc->setFragmentFunction(fragment_func);
    pipeline_desc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    pipeline_state = device->newRenderPipelineState(pipeline_desc, &error);
    if (!pipeline_state)
    {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        exit(1);
    }

    vertex_func->release();
    fragment_func->release();
    pipeline_desc->release();
    library->release();
}

void mtk_renderer::build_buffers()
{
    const size_t num_vertices = 3;

    simd::float3 positions[num_vertices] =
    {
        { -0.8f,  0.8f, 0.0f },
        {  0.0f, -0.8f, 0.0f },
        { +0.8f,  0.8f, 0.0f }
    };

    simd::float3 colors[num_vertices] =
    {
        {  1.0, 0.3f, 0.2f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 }
    };

    const size_t pos_data_size = num_vertices * sizeof( simd::float3 );
    const size_t col_data_size = num_vertices * sizeof( simd::float3 );

    MTL::Buffer* vert_pos_buffer = device->newBuffer(pos_data_size, MTL::ResourceStorageModeManaged);
    MTL::Buffer* vert_col_buffer = device->newBuffer(col_data_size, MTL::ResourceStorageModeManaged);

    vertex_pos_buffer   = vert_pos_buffer;
    vertex_color_buffer = vert_col_buffer;

    memcpy(vertex_pos_buffer->contents(), positions, pos_data_size);
    memcpy(vertex_color_buffer->contents(), colors, col_data_size);

    vertex_pos_buffer->didModifyRange(NS::Range::Make(0, vertex_pos_buffer->length()));
    vertex_pos_buffer->didModifyRange(NS::Range::Make(0, vertex_pos_buffer->length()));
}

#pragma endregion Renderer }

// the main rendering / app event happens here
std::vector<glm::vec3> metal::render(const soleil::scene &scene) {
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    dispatch_semaphore_t in_flight_semaphone = dispatch_semaphore_create(0);
    
    MTL::TextureDescriptor* output_desc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormat::PixelFormatRGBA8Uint, width, height, false);
    output_desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    
    MTL::Texture* texture = device->newTexture(output_desc);
    {
        std::vector<uint8_t> pixels(output_desc->width() * output_desc->height() * 4, 1);
        texture->replaceRegion(MTL::Region::Make2D(0, 0, output_desc->width(), output_desc->height()), 0, pixels.data(), output_desc->width() * 4);
    }
    
    using NS::StringEncoding::UTF8StringEncoding;
    
    const char* shader_source = R"(
       #include <metal_stdlib>
       #include <simd/simd.h>
       using namespace metal;


       typedef struct
       {
           float4 position [[position]];
           float2 texCoord;
       } Vertex;


       vertex Vertex vertexShader(constant Vertex* in [[buffer(0)]], uint vid [[vertex_id]])
       {
           return in[vid];
       }

       fragment uint4 fragmentShader(Vertex in [[stage_in]])
       {
           return uint4(0, in.texCoord.x * 255, in.texCoord.y * 255, 1);
       }
    )";
    
    NS::Error* error = nullptr;
    MTL::Library* library = device->newLibrary(NS::String::string(shader_source, UTF8StringEncoding), nullptr, &error);
    if (!library)
    {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        exit(1);
    }

//    MTL::Library* default_library = device->newDefaultLibrary();
    MTL::RenderPipelineDescriptor* pipeline_state_desc = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_state_desc->setSampleCount(1);
    pipeline_state_desc->setVertexFunction(library->newFunction(NS::String::string("vertexShader", UTF8StringEncoding)));
    pipeline_state_desc->setFragmentFunction(library->newFunction(NS::String::string("fragmentShader", UTF8StringEncoding)));
    pipeline_state_desc->colorAttachments()->object(0)->setPixelFormat(output_desc->pixelFormat());
    
    error = nullptr;
    MTL::RenderPipelineState* pipeline_state = device->newRenderPipelineState(pipeline_state_desc, &error);
    if (!pipeline_state || error) {
        std::cout << "Failed to create pipeline state, error: " << error << '\n';
    }
    
    MTL::CommandQueue* command_queue = device->newCommandQueue();
    // loadAssets
    VertexData vertexData[3] = {
        {{-1,  -1, 0, 1}, {0, 0}},
        {{ 1,  -1, 0, 1}, {1, 0}},
        {{ 0,   1, 0, 1}, {0, 1}}
    };

    MTL::Buffer* vertex_buffer = device->newBuffer(vertexData, sizeof(vertexData), MTL::ResourceCPUCacheModeDefaultCache);
    
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    
    MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();
    command_buffer->setLabel(NS::String::string("my_command", UTF8StringEncoding));
    
    __block dispatch_semaphore_t block_sem = in_flight_semaphone;
    command_buffer->addCompletedHandler(^(MTL::CommandBuffer* buffer)
    {
        dispatch_semaphore_signal(block_sem);
    });
    
    MTL::RenderPassDescriptor* render_pass_desc = MTL::RenderPassDescriptor::renderPassDescriptor();
    render_pass_desc->colorAttachments()->object(0)->setTexture(texture);
    render_pass_desc->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(1, 2, 3, 4));
    render_pass_desc->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
    render_pass_desc->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
    
    MTL::RenderCommandEncoder* rce = command_buffer->renderCommandEncoder(render_pass_desc);
    if (!rce) {
        std::cout << "No encoder\n";
        abort();
    }

    rce->pushDebugGroup(NS::String::string("DrawBox", UTF8StringEncoding));
    rce->setRenderPipelineState(pipeline_state);
    rce->setVertexBuffer(vertex_buffer, 0, 0);
    rce->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), sizeof(vertexData) / sizeof(VertexData));
    
    rce->popDebugGroup();
    rce->endEncoding();
    command_buffer->commit();
    
    dispatch_semaphore_wait(in_flight_semaphone, DISPATCH_TIME_FOREVER);
    
    std::vector<glm::u8vec4> result(output_desc->width() * output_desc->height());
    std::vector<glm::vec3> rgb(output_desc->width() * output_desc->height());
//    uint8_t pixels[output_desc->width() * output_desc->height() * 4];
//    memset(pixels, 2, sizeof(pixels));
//    texture->getBytes(pixels, sizeof(uint8_t) * 4 * output_desc->width(), MTL::Region::Make2D(0, 0, output_desc->width(), output_desc->height()), 0);
    
    texture->getBytes(result.data(), sizeof(uint8_t) * 4 * output_desc->width(), MTL::Region::Make2D(0, 0, output_desc->width(), output_desc->height()), 0);
    
//    uint8_t* p = pixels;
//    int i = 0;
//    for (int y = 0; y < output_desc->height(); ++y)
//    {
//        for (int x = 0; x < output_desc->width(); ++x)
//        {
//            std::cout << "( ";
////            rgb[i++]
//            for (int k = 0; k < 4; ++k)
//            {
//                std::cout << int(*(p++)) << ", ";
//            }
//            std::cout << " ) - ";
//            std::cout << int(result[i].x) << ", " << int(result[i].y) << ", " << int(result[i].z) << ", " << int(result[i].w) << ")\n";
//            ++i;
//        }
//    }
    
    std::transform(result.begin(), result.end(), rgb.begin(), [](const auto& pix){
        return glm::vec3{float(pix.x), float(pix.y), float(pix.z)};
    });

    pool->release();
    return rgb;
}

metal::metal(unsigned int w, unsigned int h, int sq) : width(w), height(h) {}

}
