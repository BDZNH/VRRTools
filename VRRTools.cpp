#ifdef _WIN32
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#endif // _WIN32
#include<SFML/Graphics.hpp>
#include<chrono>
long long GetNowUs()
{
    auto now = std::chrono::steady_clock::now();
    auto dutaion = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(dutaion).count();
}

int main()
{
    sf::String title(L"VRR测试");
    sf::RenderWindow window(sf::VideoMode(1280, 720), title);
    window.setFramerateLimit(60);

    sf::Font font;

    if (!font.loadFromFile("font/msyh.ttc"))
    {
        //load font failed
        return -1;
    }
    
    auto windowsize = window.getSize();
    sf::RectangleShape slideBlock(sf::Vector2f(static_cast<float>(windowsize.x / 60), static_cast<float>(windowsize.y / 2)));
    slideBlock.setPosition(0.0, static_cast<float>(windowsize.y / 2));

    sf::Text text(
        L"~ 按 Alt + Enter 切换全屏\r\n"
        "~ 按 Ctrl + H 切换垂直同步\r\n"
        "~ 按 Ctrl + B 切换VRR\r\n"
        "~ VRR 关闭时，按 Ctrl + ↑/↓ 调整目标帧率\r\n"
        "~ VRR 开启时, 按 Ctrl + ↑/↓ 调整VRR上限，按 Alt + ↑/↓ 调整帧率下限", font);
    text.setPosition(20, 0);

    bool isFullScreen = false;
    bool isVrrMode = false;
    bool vrrIncrase = true;
    bool isVerticalSyncEnabled = false;
    window.setVerticalSyncEnabled(isVerticalSyncEnabled);

    unsigned int frameratelimit = 60;
    unsigned int vrrMinFrameRate = 48;
    unsigned int vrrMaxFrameRate = 120;

    sf::Text frametext(L"目标帧率: 60", font);
    frametext.setPosition(text.getPosition().x, 200);
    sf::Text realFrameText(L"实际帧率: ", font);
    realFrameText.setPosition(text.getPosition().x, 240);
    sf::Text verticalSyncText(L"垂直同步: 关", font);
    verticalSyncText.setPosition(text.getPosition().x, 280);
    sf::Text vrrText(L"VRR: 关", font);
    vrrText.setPosition(text.getPosition().x, 320);

    long long beginTime = GetNowUs();
    size_t frameCount = 0;
    bool updateVrrText;
    while (window.isOpen()) {
        updateVrrText = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                auto windowsize = window.getSize();
                slideBlock.setSize(sf::Vector2f(static_cast<float>(windowsize.x / frameratelimit), static_cast<float>(windowsize.y / 2)));
                slideBlock.setPosition(0.0, static_cast<float>(windowsize.y / 2));
            }
            else if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(sf::View(visibleArea));
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code)
                {
                case sf::Keyboard::Q:
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Enter:
                    if (event.key.alt)
                    {
                        if (isFullScreen)
                        {
                            isFullScreen = false;
                            window.create(sf::VideoMode(1280, 720), title, sf::Style::Default);
                            window.setFramerateLimit(frameratelimit);
                            auto windowsize = window.getSize();
                            slideBlock.setSize(sf::Vector2f(static_cast<float>(windowsize.x / frameratelimit), static_cast<float>(windowsize.y / 2)));
                            slideBlock.setPosition(0.0, static_cast<float>(windowsize.y / 2));
                        }
                        else
                        {
                            isFullScreen = true;
                            window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
                            window.setFramerateLimit(frameratelimit);
                            auto windowsize = window.getSize();
                            slideBlock.setSize(sf::Vector2f(static_cast<float>(windowsize.x / frameratelimit), static_cast<float>(windowsize.y / 2)));
                            slideBlock.setPosition(0.0, static_cast<float>(windowsize.y / 2));
                        }
                    }
                    break;
                case sf::Keyboard::Down:
                    if (isVrrMode)
                    {
                        if (event.key.control)
                        {
                            if (vrrMaxFrameRate - vrrMinFrameRate > 2)
                            {
                                vrrMaxFrameRate--;
                                updateVrrText = true;
                            }
                        }
                        else if (event.key.alt)
                        {
                            if (vrrMinFrameRate > 1)
                            {
                                vrrMinFrameRate--;
                                updateVrrText = true;
                            }
                        }
                    }
                    else
                    {
                        if (event.key.control)
                        {
                            if (frameratelimit > 0)
                            {
                                frameratelimit--;
                                window.setFramerateLimit(frameratelimit);
                                wchar_t tmpstr[512] = { 0 };
                                swprintf(tmpstr, L"目标帧率: %u", frameratelimit);
                                auto position = frametext.getPosition();
                                frametext = sf::Text(sf::String(tmpstr), font);
                                frametext.setPosition(position);
                            }
                        }
                    }
                    break;
                case sf::Keyboard::Up:
                    if (isVrrMode)
                    {
                        if (event.key.control)
                        {
                            vrrMaxFrameRate++;
                            updateVrrText = true;
                        }
                        else if (event.key.alt)
                        {
                            if (vrrMaxFrameRate - vrrMinFrameRate > 2)
                            {
                                vrrMinFrameRate++;
                                updateVrrText = true;
                            }
                        }
                    }
                    else
                    {
                        if (event.key.control)
                        {
                            if (frameratelimit < 166)
                            {
                                frameratelimit++;
                                window.setFramerateLimit(frameratelimit);
                                wchar_t tmpstr[512] = { 0 };
                                auto position = frametext.getPosition();
                                frametext = sf::Text(sf::String(tmpstr), font);
                                frametext.setPosition(position);
                            }
                        }
                    }
                    break;
                case sf::Keyboard::H:
                    if (event.key.control)
                    {
                        isVerticalSyncEnabled = !isVerticalSyncEnabled;
                        window.setVerticalSyncEnabled(isVerticalSyncEnabled);
                        if (isVerticalSyncEnabled)
                        {
                            auto postition = verticalSyncText.getPosition();
                            verticalSyncText = sf::Text(L"垂直同步: 开", font);
                            verticalSyncText.setPosition(postition);
                        }
                        else
                        {
                            auto postition = verticalSyncText.getPosition();
                            verticalSyncText = sf::Text(L"垂直同步: 关", font);
                            verticalSyncText.setPosition(postition);
                        }
                    }
                    break;
                case sf::Keyboard::B:
                    isVrrMode = !isVrrMode;
                    updateVrrText = true;
                    break;
                default:
                    break;
                }
            }
        }

        if (updateVrrText)
        {
            if (isVrrMode)
            {
                wchar_t tmpstr[512] = { 0 };
                swprintf(tmpstr, L"VRR: Min=%u Max=%u", vrrMinFrameRate, vrrMaxFrameRate);
                auto position = vrrText.getPosition();
                vrrText = sf::Text(tmpstr, font);
                vrrText.setPosition(position);
            }
            else
            {
                auto position = vrrText.getPosition();
                vrrText = sf::Text(L"VRR: 关", font);
                vrrText.setPosition(position);
            }
        }

        auto position = slideBlock.getPosition();
        if (position.x >= window.getSize().x)
        {
            position.x = 0;
        }
        else
        {
            position.x += slideBlock.getSize().x;
        }
        slideBlock.setPosition(position);

        window.clear();
        window.draw(text);
        window.draw(frametext);
        window.draw(realFrameText);
        window.draw(verticalSyncText);
        window.draw(vrrText);
        window.draw(slideBlock);
        window.display();
        frameCount++;
        if (frameCount>0 && frameCount % 60 == 0) {
            auto nowtime = GetNowUs();
            auto duration = (nowtime - beginTime) / 1000000.0;
            beginTime = nowtime;
            wchar_t tmpstr[512] = { 0 };
            swprintf(tmpstr, L"实际帧率: %.2f", (60 / duration));
            auto position = realFrameText.getPosition();
            realFrameText = sf::Text(tmpstr, font);
            realFrameText.setPosition(position);
        }
        if (frameCount > 0 && frameCount % 10 == 0 && isVrrMode)
        {
            if (vrrIncrase)
            {
                frameratelimit++;
            }
            else if(frameratelimit > 0)
            {
                frameratelimit--;
            }
            if (frameratelimit >= vrrMaxFrameRate)
            {
                frameratelimit = vrrMaxFrameRate;
                vrrIncrase = !vrrIncrase;
            }
            if (frameratelimit <= vrrMinFrameRate)
            {
                frameratelimit = vrrMinFrameRate;
                vrrIncrase = !vrrIncrase;
            }
            window.setFramerateLimit(frameratelimit);
            wchar_t tmpstr[512] = { 0 };
            swprintf(tmpstr, L"目标帧率: %u", frameratelimit);
            auto position = frametext.getPosition();
            frametext = sf::Text(sf::String(tmpstr), font);
            frametext.setPosition(position);
        }
    }
}
