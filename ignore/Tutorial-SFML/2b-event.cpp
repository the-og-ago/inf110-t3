#include <SFML/Window.hpp>
#include <iostream>

int main()
{
    // cria a janela
    sf::Window window(sf::VideoMode({800, 600}), "My window");

    // executa o programa enquanto a janela está aberta
    while (window.isOpen())
    {
        // verifica todos os eventos que foram acionados na janela desde a última iteração do loop
        while (const std::optional event = window.pollEvent())
        {
            // evento "fechar" acionado: fecha a janela
            if (event->is<sf::Event::Closed>())
                window.close();

            // evento tecla pressionada
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // tecla ESC
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else {
                  std::cout << "scancode: " << static_cast<int>(keyPressed->scancode) << std::endl;
                  std::cout << "code: " << static_cast<int>(keyPressed->code) << std::endl;
                  std::cout << "control: " << keyPressed->control << std::endl;
                  std::cout << "alt: " << keyPressed->alt << std::endl;
                  std::cout << "shift: " << keyPressed->shift << std::endl;
                  std::cout << "system: " << keyPressed->system << std::endl;
                  std::cout << "description: " << sf::Keyboard::getDescription(keyPressed->scancode).toAnsiString() << std::endl;
                  std::cout << "localize: " << static_cast<int>(sf::Keyboard::localize(keyPressed->scancode)) << std::endl;
                  std::cout << "delocalize: " << static_cast<int>(sf::Keyboard::delocalize(keyPressed->code)) << std::endl;
                }
            }
        }

        // Restante do loop principal
    }
}
