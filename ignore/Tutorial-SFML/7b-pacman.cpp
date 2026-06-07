#include <SFML/Graphics.hpp>
#include <iostream>

char mapa[9][17] = {    // Mapa do jogo
  "1111111111111111",
  "1000010000100001",
  "1011000110001101",
  "1011010110101101",
  "1000010000100001",
  "1011010110101101",
  "1011000000001101",
  "1000011111100001",
  "1111111111111111"
};

int posx = 7; // posicao do PacMan
int posy = 6;

// direcao de movimento
bool cima = false;
bool baixo = false;
bool esq = false;
bool dir = false;

// está olhando para a direita?
bool olhadireita = true;

int main() {
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Minha janela");

    // cria um quadrado de tamanho 50
    sf::RectangleShape quad({50.f, 50.f});
    quad.setFillColor(sf::Color(0, 100, 200));

    // sprites do PacMan
    sf::Texture texture;
    if (!texture.loadFromFile("pacman.png")) {
        std::cout << "Erro lendo imagem pacman.png\n";
        return 0;
    }
    sf::Sprite spritedir{texture};

    sf::Texture texture2;
    if (!texture2.loadFromFile("pacman-esq.png")) {
        std::cout << "Erro lendo imagem pacman.png\n";
        return 0;
    }
    sf::Sprite spriteesq(texture2);

    // cria um relogio para medir o tempo
    sf::Clock clock;

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
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                      esq = true;   // left key: PacMan passa a mover para esquerda
                      dir = cima = baixo = false;
                      olhadireita = false;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    dir = true;   // right key: PacMan passa a mover para direita
                    esq = cima = baixo = false;
                    olhadireita = true;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    cima = true;  // up key: PacMan passa a mover para cima
                    esq = dir = baixo = false;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    baixo = true; // down key:PacMan passa a mover para baixo
                    esq = dir = cima = false;
                }
            }
        }

        // Muda a posição do PacMan a cada 0.2 segundos
        if (clock.getElapsedTime() > sf::seconds(0.2)) { // tempo desde último restart > 0.2s?
            clock.restart();      // recomeça contagem do tempo
            if (cima) posy--;     // muda a posição de acordo com booleano ativo
            if (baixo) posy++;
            if (esq) posx--;
            if (dir) posx++;
        }

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        // desenha paredes
        for(int i=0;i<9;i++)
            for(int j=0;j<17;j++)
                if (mapa[i][j]=='1') {
                    quad.setPosition({j*50.f, i*50.f});
                    window.draw(quad);
                }

        // desenha PacMan
        if (olhadireita) {
            spritedir.setPosition({posx*50.f,posy*50.f});
            window.draw(spritedir);
        }
        else {
            spriteesq.setPosition({posx*50.f,posy*50.f});
            window.draw(spriteesq);
        }

        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
