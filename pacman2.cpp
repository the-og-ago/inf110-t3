#include <SFML/Graphics.hpp>
#include <iostream>

// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:        André Gustavo   15/06/23
// Movimentos Pac-Man:    André Gustavo   15/06/23
// Movimento contínuo:    André Gustavo   16/06/23
// Colisão com paredes:   não implementada
// Intenção de movimento: não implementado
// Atualizado para 3.1.0: André Gustavo   03/06/26

char mapa[11][21] = {     // Mapa do jogo
  "12111111111111111121",
  "12122222222222222221",
  "12121121110111212121",
  "12121121220221211121",
  "22222220000000222222",
  "11112121220221211121",
  "11112121110111212121",
  "12222222222222222221",
  "12121112111112112121",
  "12122222222222222121",
  "12111111111111111121"
};

const float SIZE = 50;      // Tamanho de cada célula do mapa

int posx = 9; // posicao do PacMan
int posy = 7;

//posições dos fantasmas
int gato1x = 8,  gato1y = 3;
int gato2x = 12, gato2y = 3;
int gato3x = 8,  gato3y = 5;
int gato4x = 12, gato4y = 5;

bool cima = false;  // direcao de movimento do PacMan
bool baixo = false;
bool esq = false;
bool dir = false;

int main() {
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1000, 550}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));
    
     sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));

    // sprites do PacMan
    sf::Texture texturepeixe;
    if (!texturepeixe.loadFromFile("peixe_dir.png")) {
        std::cout << "Erro lendo imagem pacman.png\n";
        return 0;
    }
    sf::Sprite sprite{texturepeixe};
    
    //sprites do fantasma
      sf::Texture texturegato;
    if (!texturegato.loadFromFile("gatinho.png")) {
        std::cout << "Erro lendo imagem gatinho.png\n";
        return 0;
    }
    sf::Sprite sprite2{texturegato};

    // cria um relogio para medir o tempo do PacMan
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
                      esq = true;   // left key: PacMan tem intenção de se mover para esquerda
                      dir = cima = baixo = false;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                      dir = true;   // right key: PacMan tem intenção de se mover para direita
                      esq = cima = baixo = false;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                      cima = true;   // up key: PacMan tem intenção de se mover para cima
                      esq = dir = baixo = false;
                  }
                  else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                      baixo = true;   // down key: PacMan tem intenção de se mover para baixo
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
        for(int i=0;i<11;i++)
            for(int j=0;j<21;j++)
                if (mapa[i][j]=='1') {
                    quad.setPosition({j*SIZE, i*SIZE});
                    window.draw(quad);
                }

        for(int i=0;i<11;i++)
            for(int j=0;j<21;j++)
                if (mapa[i][j]=='2') {
                    circ.setPosition({j*SIZE + 25, i*SIZE + 25});
                    window.draw(circ);
                }
        // desenha PacMan
        sprite.setPosition({posx*SIZE,posy*SIZE});
        window.draw(sprite);
        
        //desenha fantasmas(gatinhos)
        sprite2.setPosition({gato1x*SIZE,gato1y*SIZE});
        window.draw(sprite2);

        sprite2.setPosition({gato2x*SIZE, gato2y*SIZE});
        window.draw(sprite2);

        sprite2.setPosition({gato3x*SIZE, gato3y*SIZE});
        window.draw(sprite2);

        sprite2.setPosition({gato4x*SIZE, gato4y*SIZE});
        window.draw(sprite2);
        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
