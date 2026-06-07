#include <SFML/Graphics.hpp>

int main() {
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Minha janela");

    // cria um círculo de raio 50
    sf::CircleShape shape(50.f);

    // define a posição absoluta do círculo
    shape.setPosition({10.f, 50.f});

    // define a cor do círculo (verde)
    shape.setFillColor(sf::Color(100, 250, 50));

    // executa o programa enquanto a janela está aberta
    while (window.isOpen()) {

        // verifica todos os eventos que foram acionados na janela desde a última iteração do loop
        while (const std::optional event = window.pollEvent()) {
            // evento "fechar" acionado: fecha a janela
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        // desenha o círculo na janela
        window.draw(shape);

        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
