#include <SFML/Graphics.hpp>
#include <iostream>

// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:        André Gustavo   15/06/23
// Movimentos Pac-Man:    André Gustavo   15/06/23
// Movimento contínuo:    não implementado
// Colisão com paredes:   não implementada
// Intenção de movimento: não implementado
// Atualizado para 3.1.0: André Gustavo   03/06/26

char mapa[11][21] = {     // Mapa do jogo
  "11111111111111111111",
  "10000100000000100001",
  "10110101111110101101",
  "10100000000000000101",
  "10101101100110110101",
  "10000001000010000001",
  "10101101111110110101",
  "10100000000000000101",
  "10110101111110101101",
  "10000100000000100001",
  "11111111111111111111"
};

const float SIZE = 50;      // Tamanho de cada célula do mapa

int posx = 9; // posicao do PacMan
int posy = 7;

int main() {
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1000, 550}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));

    // sprites do PacMan
    sf::Texture texture;
    if (!texture.loadFromFile("pacman.png")) {
        std::cout << "Erro lendo imagem pacman.png\n";
        return 0;
    }
    sf::Sprite sprite{texture};

    // executa o programa enquanto a janela está aberta
    while (window.isOpen()) {

        // verifica todos os eventos que foram acionados na janela desde a última iteração do loop
        while (const std::optional event = window.pollEvent()) {
            // evento "fechar" acionado: fecha a janela
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                  if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                      window.close();
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                      posx--;   // left key: move o PacMan para esquerda
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                      posx++;   // right key: move o PacMan para direita
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                      posy--;   // up key: move o PacMan para cima
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                      posy++;   // down key: move o PacMan para baixo
            }
        }

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        // desenha paredes
        for(int i=0;i<11;i++)
            for(int j=0;j<21;j++)
                if (mapa[i][j]=='1') {
                    quad.setPosition({j*SIZE, i*SIZE});
                    window.draw(quad);
                }

        // desenha PacMan
        sprite.setPosition({posx*SIZE,posy*SIZE});
        window.draw(sprite);

        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
