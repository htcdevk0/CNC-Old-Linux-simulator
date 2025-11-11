#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

sf::ContextSettings contextSettings;
void setupContextSettings()
{
    contextSettings.antiAliasingLevel = 0;
    contextSettings.depthBits = 24;
    contextSettings.stencilBits = 8;
}

void applyRetroWindowRenderer(tgui::ChildWindow::Ptr window)
{
    auto renderer = window->getRenderer();
    renderer->setBackgroundColor(sf::Color(192, 192, 192));
    renderer->setTitleBarColor(sf::Color(64, 64, 64));
    renderer->setTitleColor(sf::Color::White);
    renderer->setBorders(2);
    renderer->setBorderColor(sf::Color::Black);
    renderer->setDistanceToSide(2);
}

int runCncLinuxInternal()
{
    setupContextSettings();

    sf::RenderWindow window(sf::VideoMode({ 1024, 720 }), "CNC Linux - Desktop", sf::State::Fullscreen, contextSettings);
    window.setFramerateLimit(60);

    tgui::Gui gui(window);

    sf::Image curArrow;
    if (!curArrow.loadFromFile("assets/cursors/arrow.png"))
        return -1;

    unsigned int newWidth = 24;
    unsigned int newHeight = 24;
    sf::Vector2u srcSize = curArrow.getSize();
    std::vector<std::uint8_t> resizedPixels(newWidth * newHeight * 4);
    const std::uint8_t* srcPixels = curArrow.getPixelsPtr();

    for (unsigned int y = 0; y < newHeight; ++y)
    {
        for (unsigned int x = 0; x < newWidth; ++x)
        {
            unsigned int srcX = x * srcSize.x / newWidth;
            unsigned int srcY = y * srcSize.y / newHeight;
            unsigned int srcIndex = (srcY * srcSize.x + srcX) * 4;
            unsigned int dstIndex = (y * newWidth + x) * 4;

            resizedPixels[dstIndex + 0] = srcPixels[srcIndex + 0];
            resizedPixels[dstIndex + 1] = srcPixels[srcIndex + 1];
            resizedPixels[dstIndex + 2] = srcPixels[srcIndex + 2];
            resizedPixels[dstIndex + 3] = srcPixels[srcIndex + 3];
        }
    }

    auto cursor = sf::Cursor::createFromPixels(resizedPixels.data(), { newWidth, newHeight }, { 0, 0 });
    if (!cursor)
        return -1;

    window.setMouseCursor(*cursor);
    sf::Color bgColor(200, 200, 200);

    auto desktop = tgui::Panel::create({ "100%", "100%" });
    desktop->getRenderer()->setBackgroundColor(bgColor);
    gui.add(desktop, "desktop");

    const int taskbarHeight = 36;
    auto taskbar = tgui::Panel::create({ "100%", taskbarHeight });
    taskbar->setPosition(0, "100% - 36");
    taskbar->getRenderer()->setBackgroundColor(sf::Color(100, 100, 100));
    taskbar->getRenderer()->setBorders(1);
    taskbar->getRenderer()->setBorderColor(sf::Color::Black);
    gui.add(taskbar, "taskbar");

    auto startBtn = tgui::Button::create("Start");
    startBtn->setSize(100, 28);
    startBtn->setPosition(6, 4);
    startBtn->getRenderer()->setBackgroundColor(sf::Color(192, 192, 192));
    startBtn->getRenderer()->setBorderColor(sf::Color::Black);
    startBtn->getRenderer()->setBorders(1);
    startBtn->getRenderer()->setTextColor(sf::Color::Black);
    taskbar->add(startBtn, "startBtn");

    auto clockLabel = tgui::Label::create("00:00:00");
    clockLabel->setTextSize(14);
    clockLabel->getRenderer()->setTextColor(sf::Color::White);
    clockLabel->setPosition("100% - 110", 6);
    taskbar->add(clockLabel, "clock");

    const int startMenuHeight = 200;
    auto startMenu = tgui::Panel::create({ 180, startMenuHeight });
    startMenu->setPosition(6, tgui::String("100% - 36 - ") + tgui::String::fromNumber(startMenuHeight));
    startMenu->getRenderer()->setBackgroundColor(sf::Color(210, 210, 210));
    startMenu->getRenderer()->setBorders(2);
    startMenu->getRenderer()->setBorderColor(sf::Color::Black);
    startMenu->setVisible(false);
    gui.add(startMenu, "startMenu");

    auto startTitle = tgui::Label::create("CNC Linux");
    startTitle->setPosition(8, 6);
    startTitle->setTextSize(14);
    startTitle->getRenderer()->setTextColor(sf::Color::Black);
    startMenu->add(startTitle);

    auto menuTextViewer = tgui::Button::create("Text Viewer");
    menuTextViewer->setSize("100%", 36);
    menuTextViewer->setPosition(0, 34);
    menuTextViewer->getRenderer()->setBackgroundColor(sf::Color(200, 200, 200));
    menuTextViewer->getRenderer()->setBorderColor(sf::Color::Black);
    menuTextViewer->getRenderer()->setBorders(1);
    menuTextViewer->getRenderer()->setTextColor(sf::Color::Black);
    startMenu->add(menuTextViewer);

    auto menuTerminal = tgui::Button::create("Terminal");
    menuTerminal->setSize("100%", 36);
    menuTerminal->setPosition(0, 34 + 36 + 4);
    menuTerminal->getRenderer()->setBackgroundColor(sf::Color(200, 200, 200));
    menuTerminal->getRenderer()->setBorderColor(sf::Color::Black);
    menuTerminal->getRenderer()->setBorders(1);
    menuTerminal->getRenderer()->setTextColor(sf::Color::Black);
    startMenu->add(menuTerminal);

    auto menuAbout = tgui::Button::create("About");
    menuAbout->setSize("100%", 36);
    menuAbout->setPosition(0, 34 + 36 + 4 + 36 + 4);
    menuAbout->getRenderer()->setBackgroundColor(sf::Color(200, 200, 200));
    menuAbout->getRenderer()->setBorderColor(sf::Color::Black);
    menuAbout->getRenderer()->setBorders(1);
    menuAbout->getRenderer()->setTextColor(sf::Color::Black);
    startMenu->add(menuAbout);

    auto menuExit = tgui::Button::create("Shutdown");
    menuExit->setSize("100%", 36);
    menuExit->setPosition(0, 34 + 36 + 4 + 36 + 4 + 36 + 4);
    menuExit->getRenderer()->setBackgroundColor(sf::Color(200, 200, 200));
    menuExit->getRenderer()->setBorderColor(sf::Color::Black);
    menuExit->getRenderer()->setBorders(1);
    menuExit->getRenderer()->setTextColor(sf::Color::Black);
    startMenu->add(menuExit);

    auto iconBtn = tgui::Button::create("Text Viewer");
    iconBtn->setSize(140, 28);
    iconBtn->setPosition(40, 80);
    iconBtn->getRenderer()->setBackgroundColor(sf::Color(220, 220, 220));
    iconBtn->getRenderer()->setBorderColor(sf::Color::Black);
    iconBtn->getRenderer()->setBorders(1);
    iconBtn->getRenderer()->setTextColor(sf::Color::Black);
    gui.add(iconBtn, "iconTextViewer");

    auto iconTerminal = tgui::Button::create("Terminal");
    iconTerminal->setSize(140, 28);
    iconTerminal->setPosition(40, 112);
    iconTerminal->getRenderer()->setBackgroundColor(sf::Color(220, 220, 220));
    iconTerminal->getRenderer()->setBorderColor(sf::Color::Black);
    iconTerminal->getRenderer()->setBorders(1);
    iconTerminal->getRenderer()->setTextColor(sf::Color::Black);
    gui.add(iconTerminal, "iconTerminal");

    auto tvWindow = tgui::ChildWindow::create();
    tvWindow->setSize("60%", "55%");
    tvWindow->setPosition("20%", "18%");
    tvWindow->setTitle("Text Viewer - Notes.txt");
    tvWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    tvWindow->setVisible(false);
    applyRetroWindowRenderer(tvWindow);
    gui.add(tvWindow, "tvWindow");

    auto tvContent = tgui::TextArea::create();
    tvContent->setSize("96%", "96%");
    tvContent->setPosition("2%", "2%");
    tvContent->setReadOnly(true);
    tvContent->setTextSize(14);
    tvContent->getRenderer()->setBackgroundColor(sf::Color(250, 250, 250));
    tvContent->getRenderer()->setBorderColor(sf::Color::Black);
    tvWindow->add(tvContent, "content");

    const std::string sampleText =
        "CNC Linux - Text Viewer\n\n"
        "This is a demonstration of the simulated desktop environment.\n\n"
        "All windows, buttons, and controls are rendered entirely in memory.\n\n"
        "Explore the Start Menu and Terminal to see more examples.";

    tvContent->setText(sampleText);

    auto aboutWindow = tgui::ChildWindow::create();
    aboutWindow->setSize(340, 160);
    aboutWindow->setPosition("calc(50% - 170)", "calc(50% - 80)");
    aboutWindow->setTitle("About CNC Linux");
    aboutWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    aboutWindow->setVisible(false);
    applyRetroWindowRenderer(aboutWindow);
    gui.add(aboutWindow, "aboutWindow");

    auto aboutLabel = tgui::Label::create("CNC Linux Desktop Simulation\nVersion 1.0\nAll components are virtual and stored in memory only.");
    aboutLabel->setPosition(8, 8);
    aboutLabel->setTextSize(13);
    aboutLabel->getRenderer()->setTextColor(sf::Color::Black);
    aboutLabel->setAutoSize(true);
    aboutWindow->add(aboutLabel);

    auto aboutClose = tgui::Button::create("OK");
    aboutClose->setSize(80, 28);
    aboutClose->setPosition("calc(50% - 40)", "100% - 40");
    aboutClose->getRenderer()->setBackgroundColor(sf::Color(192, 192, 192));
    aboutClose->getRenderer()->setBorderColor(sf::Color::Black);
    aboutClose->getRenderer()->setBorders(1);
    aboutWindow->add(aboutClose);

    auto termWindow = tgui::ChildWindow::create();
    termWindow->setSize(600, 400);
    termWindow->setPosition("25%", "25%");
    termWindow->setTitle("Terminal");
    termWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Close);
    termWindow->setVisible(false);
    applyRetroWindowRenderer(termWindow);
    termWindow->getRenderer()->setBackgroundColor(sf::Color::Black);
    gui.add(termWindow, "termWindow");

    auto termOutput = tgui::TextArea::create();
    termOutput->setSize("100%", "100% - 30");
    termOutput->setPosition(0, 0);
    termOutput->setReadOnly(true);
    termOutput->setTextSize(14);
    termOutput->getRenderer()->setBackgroundColor(sf::Color::Black);
    termOutput->getRenderer()->setTextColor(sf::Color(220, 220, 220));
    termOutput->getRenderer()->setBorderColor(sf::Color::Black);
    termOutput->setText("> Terminal ready.\n> Type 'help' for a list of commands.\n\n");
    termWindow->add(termOutput, "termOutput");

    auto termInput = tgui::EditBox::create();
    termInput->setSize("100%", 30);
    termInput->setPosition(0, "100% - 30");
    termInput->setTextSize(14);
    auto renderer = termInput->getRenderer();
    renderer->setBackgroundColor(sf::Color::Black);
    renderer->setTextColor(sf::Color(220, 220, 220));
    renderer->setBorderColor(sf::Color(50, 50, 50));
    renderer->setBorders(1);
    renderer->setBackgroundColorFocused(renderer->getBackgroundColor());
    renderer->setBorderColorFocused(renderer->getBorderColor());
    renderer->setBackgroundColorHover(renderer->getBackgroundColor());
    renderer->setBorderColorHover(renderer->getBorderColor());

    termWindow->add(termInput, "termInput");

    startBtn->onClick([&]() { startMenu->setVisible(!startMenu->isVisible()); });

    desktop->onMousePress([&](tgui::Vector2f pos) {
        if (startMenu->isVisible())
        {
            tgui::FloatRect menuRect(startMenu->getAbsolutePosition().x, startMenu->getAbsolutePosition().y, startMenu->getSize().x, startMenu->getSize().y);
            tgui::FloatRect btnRect(startBtn->getAbsolutePosition().x, startBtn->getAbsolutePosition().y, startBtn->getSize().x, startBtn->getSize().y);
            if (menuRect.contains(pos) || btnRect.contains(pos))
                return;
            startMenu->setVisible(false);
        }
        });

    menuTextViewer->onClick([&]() { startMenu->setVisible(false); tvWindow->setVisible(true); tvWindow->moveToFront(); });
    menuAbout->onClick([&]() { startMenu->setVisible(false); aboutWindow->setVisible(true); aboutWindow->moveToFront(); });
    menuTerminal->onClick([&]() { startMenu->setVisible(false); termWindow->setVisible(true); termWindow->moveToFront(); });
    menuExit->onClick([&]() { window.close(); });

    iconBtn->onClick([&]() { tvWindow->setVisible(true); tvWindow->moveToFront(); });
    iconTerminal->onClick([&]() { termWindow->setVisible(true); termWindow->moveToFront(); });

    tvWindow->onClose([&]() { tvWindow->setVisible(false); });
    aboutClose->onClick([&]() { aboutWindow->setVisible(false); });
    aboutWindow->onClose([&]() { aboutWindow->setVisible(false); });
    termWindow->onClose([&]() { termWindow->setVisible(false); });

    termInput->onReturnKeyPress([&]() {
        std::string cmd = termInput->getText().toStdString();
        termInput->setText("");
        tgui::TextArea::Ptr output = termWindow->get<tgui::TextArea>("termOutput");
        output->addText("> " + cmd + "\n");

        if (cmd == "help") {
            output->addText("Commands:\n  help  - Show available commands\n  ls    - List files\n  cat   - Show file content\n  clear - Clear screen\n  exit  - Close terminal\n\n");
        }
        else if (cmd == "ls") {
            output->addText("Documents: README.txt   welcome.txt\n\n");
        }
        else if (cmd == "cat welcome.txt") {
            output->addText("Welcome to the CNC Linux Terminal!\nThis system runs entirely in memory.\n\n");
        }
        else if (cmd == "cat README.txt") {
            output->addText("CNC Linux Terminal v1.0\nType 'help' for a list of supported commands.\n\n");
        }
        else if (cmd.rfind("cat", 0) == 0) {
            output->addText("File not found.\n\n");
        }
        else if (cmd == "clear") {
            output->setText("> Terminal ready.\n\n");
        }
        else if (cmd == "exit") {
            termWindow->setVisible(false);
        }
        else if (!cmd.empty()) {
            output->addText("Unknown command: '" + cmd + "'\n\n");
        }

        output->setCaretPosition(output->getText().length());
        });

    auto bringParentToFront = [&](tgui::Widget::Ptr widget) {
        widget->onMouseEnter([=] {
            if (widget->getParent()) widget->getParent()->moveToFront();
            });
        };

    bringParentToFront(tvContent);
    bringParentToFront(aboutClose);
    bringParentToFront(termOutput);
    bringParentToFront(termInput);

    sf::Clock clock;
    sf::Time clockAcc = sf::Time::Zero;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>()) window.close();
            else if (const auto* keyEvt = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvt->scancode == sf::Keyboard::Scancode::Escape)
                {
                    if (aboutWindow->isVisible()) aboutWindow->setVisible(false);
                    else if (termWindow->isVisible()) termWindow->setVisible(false);
                    else if (tvWindow->isVisible()) tvWindow->setVisible(false);
                    else window.close();
                }
            }
        }

        sf::Time dt = clock.restart();
        clockAcc += dt;
        if (clockAcc.asSeconds() >= 1.0f)
        {
            auto now = std::chrono::system_clock::now();
            std::time_t tnow = std::chrono::system_clock::to_time_t(now);
            std::tm tm_now;
#if defined(_WIN32)
            localtime_s(&tm_now, &tnow);
#else
            localtime_r(&tnow, &tm_now);
#endif
            std::ostringstream ss;
            ss << std::setfill('0') << std::setw(2) << tm_now.tm_hour << ":"
                << std::setfill('0') << std::setw(2) << tm_now.tm_min << ":"
                << std::setfill('0') << std::setw(2) << tm_now.tm_sec;
            clockLabel->setText(ss.str());
            clockAcc = sf::Time::Zero;
        }

        window.clear(bgColor);
        gui.draw();
        window.display();
    }

    return 0;
}
