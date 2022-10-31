//
// Created by goksu on 6/7/19.
//

#include "integrators/metal.hpp"

#include "Foundation/Foundation.hpp"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <simd/simd.h>

namespace soleil {

app_delegate::~app_delegate() {
    window->release();
    mtk_view->release();
    device->release();
    delete view_delegate;
}

#pragma mark - AppDelegate
#pragma region AppDelegate {

void app_delegate::applicationWillFinishLaunching(NS::Notification* notification) {
    NS::Menu* menu = createMenuBar();
    NS::Application* app = reinterpret_cast<NS::Application*>(notification->object());
    app->setMainMenu(menu);
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void app_delegate::applicationDidFinishLaunching(NS::Notification* notification) {
    CGRect frame = (CGRect){ {100.0, 100.0}, {512.0, 512.0} };

    window = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false );

    device = MTL::CreateSystemDefaultDevice();

    mtk_view = MTK::View::alloc()->init(frame, device);
    mtk_view->setColorPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
    mtk_view->setClearColor( MTL::ClearColor::Make( 1.0, 0.0, 0.0, 1.0 ) );

    view_delegate = new MTK_view_delegate(device);
    mtk_view->setDelegate(view_delegate);

    window->setContentView(mtk_view);
    window->setTitle(NS::String::string("01 - Primitive", NS::StringEncoding::UTF8StringEncoding));

    window->makeKeyAndOrderFront(nullptr);

    NS::Application* current_app = reinterpret_cast<NS::Application*>(notification->object());
    current_app->activateIgnoringOtherApps(true);
}

bool app_delegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application *sender) {
    return true;
}

NS::Menu* app_delegate::createMenuBar() {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu* main_menu = NS::Menu::alloc()->init();
    NS::MenuItem* app_menu_item = NS::MenuItem::alloc()->init();
    NS::Menu* app_menu = NS::Menu::alloc()->init(NS::String::string("Appname", UTF8StringEncoding));

    NS::String* app_name = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quit_item_name = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(app_name);
    SEL quit_cb = NS::MenuItem::registerActionCallback( "appQuit", [](void*, SEL, const NS::Object* sender){
        auto current_app = NS::Application::sharedApplication();
        current_app->terminate(sender);
    } );

    NS::MenuItem* app_quit_item = app_menu->addItem(quit_item_name, quit_cb, NS::String::string("q", UTF8StringEncoding));
    app_quit_item->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    app_menu_item->setSubmenu(app_menu);

    NS::MenuItem* window_menu_item = NS::MenuItem::alloc()->init();
    NS::Menu* window_menu = NS::Menu::alloc()->init(NS::String::string( "Window", UTF8StringEncoding));

    SEL close_window_cb = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*){
        auto current_app = NS::Application::sharedApplication();
        current_app->windows()->object<NS::Window>(0)->close();
    } );
    NS::MenuItem* close_window_item = window_menu->addItem(NS::String::string("Close Window", UTF8StringEncoding), close_window_cb, NS::String::string("w", UTF8StringEncoding));
    close_window_item->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    window_menu_item->setSubmenu(window_menu);

    main_menu->addItem(app_menu_item);
    main_menu->addItem(window_menu_item);

    app_menu_item->release();
    window_menu_item->release();
    app_menu->release();
    window_menu->release();

    return main_menu->autorelease();
}

#pragma endregion AppDelegate }

#pragma mark - ViewDelegate
#pragma region ViewDelegate {

MTK_view_delegate::MTK_view_delegate(MTL::Device* device) : MTK::ViewDelegate(), renderer(new mtk_renderer(device)) {}

MTK_view_delegate::~MTK_view_delegate()
{
    delete renderer;
}

void MTK_view_delegate::drawInMTKView(MTK::View* view)
{
    renderer->draw(view);
}

#pragma endregion ViewDelegate }


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
    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();
    
    app_delegate delegate;
    NS::Application* application = NS::Application::sharedApplication();
    application->setDelegate(&delegate);
    application->run();
    
    autorelease_pool->release();
    
    return {};
}

metal::metal(unsigned int w, unsigned int h, int sq) : width(w), height(h) {}

}
