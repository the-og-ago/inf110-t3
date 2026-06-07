#include <SFML/Window.hpp>

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
        }
    }
}
