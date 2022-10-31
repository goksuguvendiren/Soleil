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

// the main rendering / app event happens here
std::vector<glm::vec4> metal::render(const soleil::scene &scene) {
    using NS::StringEncoding::UTF8StringEncoding;
    
    MTL::RenderPipelineDescriptor* pipeline_state_desc = build_shaders();
    
    NS::Error* error = nullptr;
    MTL::RenderPipelineState* pipeline_state = device->newRenderPipelineState(pipeline_state_desc, &error);
    if (!pipeline_state || error) {
        __builtin_printf("Failed to create pipeline state, error: %s", error->localizedDescription()->utf8String());

        std::cout << "Failed to create pipeline state, error: " << &error << '\n';
        abort();
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
    
    __block dispatch_semaphore_t block_sem = in_flight_semaphore;
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
    
    dispatch_semaphore_wait(in_flight_semaphore, DISPATCH_TIME_FOREVER);
    
    std::vector<glm::vec4> rgb(output_desc->width() * output_desc->height());

    texture->getBytes(rgb.data(), sizeof(glm::vec4) * output_desc->width(), MTL::Region::Make2D(0, 0, output_desc->width(), output_desc->height()), 0);
    
    pool->release();
    return rgb;
}

metal::metal(unsigned int w, unsigned int h, int sq) : width(w), height(h) {
    device = MTL::CreateSystemDefaultDevice();
    in_flight_semaphore = dispatch_semaphore_create(0);
    output_desc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormat::PixelFormatRGBA32Float, width, height, false);

    output_desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    texture = device->newTexture(output_desc);

    // initialize all pixels with 0
    std::vector<glm::vec4> pixels(output_desc->width() * output_desc->height(), glm::vec4(0));
    texture->replaceRegion(MTL::Region::Make2D(0, 0, output_desc->width(), output_desc->height()), 0, pixels.data(), sizeof(glm::vec4) * output_desc->width());
}

MTL::RenderPipelineDescriptor* metal::build_shaders() {
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

       fragment float4 fragmentShader(Vertex in [[stage_in]])
       {
           return float4(0, in.texCoord.x, in.texCoord.y, 1);
       }
    )";
    
    NS::Error* error = nullptr;
    library = device->newLibrary(NS::String::string(shader_source, UTF8StringEncoding), nullptr, &error);
    if (!library)
    {
        __builtin_printf("%s", error->localizedDescription()->utf8String());
        exit(1);
    }

    MTL::RenderPipelineDescriptor* pipeline_state_desc = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_state_desc->setSampleCount(1);
    pipeline_state_desc->setVertexFunction(library->newFunction(NS::String::string("vertexShader", UTF8StringEncoding)));
    pipeline_state_desc->setFragmentFunction(library->newFunction(NS::String::string("fragmentShader", UTF8StringEncoding)));
    pipeline_state_desc->colorAttachments()->object(0)->setPixelFormat(output_desc->pixelFormat());
    
    return pipeline_state_desc;
}
}
