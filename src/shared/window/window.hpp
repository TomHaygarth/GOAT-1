#pragma once

#include <string>
#include <memory>

#include "renderer/render_context.hpp"

namespace Window
{
    class IWindowFunctions
    {
    public:

        virtual ~IWindowFunctions();

        virtual void Input() = 0;
        virtual void Render(Renderer::IRenderContext * render_context) = 0;

        virtual bool WindowShouldClose() = 0;
    };

    class WindowInstance
    {
    public:
        WindowInstance(std::string const & window_name,
                       int width,
                       int height,
                       IWindowFunctions * window_funcs);

        int Run();

    private:
        std::string m_title;
        int m_width;
        int m_height;
        IWindowFunctions * m_window_funcs;
    };

    WindowInstance * create_window(std::string const & window_name,
                                   int width,
                                   int height,
                                   IWindowFunctions * window_funcs);

    void destroy_window(WindowInstance * window);
}
