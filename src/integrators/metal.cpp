//
// Created by goksu on 6/7/19.
//

#include "integrators/metal.hpp"

#include "Foundation/Foundation.hpp"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace soleil {

app_delegate::~app_delegate() {
    window->release();
    mtk_view->release();
    device->release();
    delete view_delegate;
}

#pragma mark - AppDelegate
#pragma region AppDelegate {

void app_delegate::applicationWillFinishLaunching(NS::Notification *pNotification) {
    NS::Menu* pMenu = createMenuBar();
    NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
    pApp->setMainMenu( pMenu );
    pApp->setActivationPolicy( NS::ActivationPolicy::ActivationPolicyRegular );
}

void app_delegate::applicationDidFinishLaunching(NS::Notification *pNotification) {
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
    window->setTitle( NS::String::string( "00 - Window", NS::StringEncoding::UTF8StringEncoding ) );

    window->makeKeyAndOrderFront( nullptr );

    NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
    pApp->activateIgnoringOtherApps( true );

}

bool app_delegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application *pSender) {
    return true;
}

NS::Menu* app_delegate::createMenuBar() {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu* main_menu = NS::Menu::alloc()->init();
    NS::MenuItem* app_menu_item = NS::MenuItem::alloc()->init();
    NS::Menu* app_menu = NS::Menu::alloc()->init( NS::String::string( "Appname", UTF8StringEncoding ) );

    NS::String* app_name = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quit_item_name = NS::String::string( "Quit ", UTF8StringEncoding )->stringByAppendingString( app_name );
    SEL quit_cb = NS::MenuItem::registerActionCallback( "appQuit", [](void*, SEL, const NS::Object* sender){
        auto current_app = NS::Application::sharedApplication();
        current_app->terminate( sender );
    } );

    NS::MenuItem* app_quit_item = app_menu->addItem( quit_item_name, quit_cb, NS::String::string("q", UTF8StringEncoding));
    app_quit_item->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
    app_menu_item->setSubmenu(app_menu);

    NS::MenuItem* window_menu_item = NS::MenuItem::alloc()->init();
    NS::Menu* window_menu = NS::Menu::alloc()->init(NS::String::string( "Window", UTF8StringEncoding));

    SEL close_window_cb = NS::MenuItem::registerActionCallback( "windowClose", [](void*, SEL, const NS::Object*){
        auto current_app = NS::Application::sharedApplication();
        current_app->windows()->object<NS::Window>(0)->close();
    } );
    NS::MenuItem* close_window_item = window_menu->addItem( NS::String::string("Close Window", UTF8StringEncoding), close_window_cb, NS::String::string( "w", UTF8StringEncoding));
    close_window_item->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );

    window_menu_item->setSubmenu( window_menu );

    main_menu->addItem( app_menu_item );
    main_menu->addItem( window_menu_item );

    app_menu_item->release();
    window_menu_item->release();
    app_menu->release();
    window_menu->release();

    return main_menu->autorelease();
}

#pragma endregion AppDelegate }

#pragma mark - ViewDelegate
#pragma region ViewDelegate {

MTK_view_delegate::MTK_view_delegate(MTL::Device* pDevice) : MTK::ViewDelegate(), renderer(new mtk_renderer(pDevice))
{
}

MTK_view_delegate::~MTK_view_delegate()
{
    delete renderer;
}

void MTK_view_delegate::drawInMTKView( MTK::View* pView )
{
    renderer->draw( pView );
}

#pragma endregion ViewDelegate }


#pragma mark - Renderer
#pragma region Renderer {

mtk_renderer::mtk_renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
}

mtk_renderer::~mtk_renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
}

void mtk_renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    pEnc->endEncoding();
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
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
