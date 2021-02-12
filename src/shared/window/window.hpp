#pragma once

#include <string>
#include <memory>

namespace Window
{
    class IWindowFunctions
    {
    public:
        virtual ~IWindowFunctions() = 0;

        virtual void Input() = 0;
        virtual void Render() = 0;
    };

    class WindowInstance
    {
    public:
        WindowInstance(std::string const & window_name,
                       int width,
                       int height,
                       std::unique_ptr<IWindowFunctions> && window_funcs);

        int Run();

    private:
        std::string m_title;
        int m_width;
        int m_height;
        std::unique_ptr<IWindowFunctions> m_window_funcs;
    };

    WindowInstance * create_window(std::string const & window_name,
                                   int width,
                                   int height,
                                   std::unique_ptr<IWindowFunctions> && window_funcs);

    void destroy_window(WindowInstance * window);
}
