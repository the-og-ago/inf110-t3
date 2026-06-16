#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <random>
using namespace std;

// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:        André Gustavo   15/06/23
// Movimentos Pac-Man:    André Gustavo   15/06/23
// Movimento contínuo:    André Gustavo   16/06/23
// Colisão com paredes:   não implementada
// Intenção de movimento: não implementado
// Atualizado para 3.1.0: André Gustavo   03/06/26

// O sistema de perseguicao esta um pouco "defeituoso", as vezes o fantasma fica preso em um loop, indo pra frente e pra tras, acho q isso tem a ver com o mapa ter esquinas e caminhos sem saida, e a forma q o fantasma calcula a melhor direcao

const int MAPLARG = 23;
const int MAPALT = 13;
char mapa[MAPALT][MAPLARG] = { // Mapa do jogo
    "333333333333333333333",
    "3121111111111111111213",
    "3121222222222222222213",
    "3121211211101112121213",
    "3121211212202212111213",
    "3222222200000002222223",
    "3111121212202212111213",
    "3111121211101112121213",
    "3122222222222222222213",
    "3121211121111121121213",
    "3121222222222222221213",
    "3121111111111111111213",
    "3333333333333333333333"};
const float SIZE = 50; // Tamanho de cada célula do mapa

int posx = 9; // posicao do PacMan
int posy = 8;
int inten[2] = {0, 0}; // intenção de movimento
int pontos = 0;
int pontosmax = 0;

bool cima = false; // direcao de movimento do PacMan
bool baixo = false;
bool esq = false;
bool dir = true;

// posições dos fantasmas / o ultimo eh o "inteligente"
int gatosx[] = {8, 12, 8, 12, 10};
int gatosy[] = {3, 3, 5, 5, 4};

// direcoes dos fantasmas
bool fcima[] = {true, false, false, false, false};
bool fbaixo[] = {false, true, false, false, false};
bool fesq[] = {false, false, true, false, false};
bool fdir[] = {false, false, false, true, false};
// Função para verificar se há mais de 2 caminhos (o que ele veio + novas opções)
bool ehEncruzilhada(int i)
{
    int caminhosLivres = 0;
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i] + 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1')
        caminhosLivres++;

    return caminhosLivres > 2;
}

// Função para verificar se eh caminho sem saida
bool semSaida(int i)
{
    int caminhosLivres = 0;
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i] + 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1')
        caminhosLivres++;

    return caminhosLivres == 1; // se tiver apenas 1 caminho livre
}

void mudarDirecao(int i)
{
    int num = 0;
    int opcoes[4] = {0};
    // Adiciona as opções válidas, excluindo o caminho de volta (a direção oposta)
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1' && (!fbaixo[i] || semSaida(i)))
    {
        opcoes[num] = 0; // Cima
        num++;
    }
    else if (mapa[gatosy[i] + 1][gatosx[i]] != '1' && (!fcima[i] || semSaida(i)))
    {
        opcoes[num] = 1; // Baixo
        num++;
    }
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1' && (!fdir[i] || semSaida(i)))
    {
        opcoes[num] = 2;
        ; // Esq
        num++;
    }
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1' && (!fesq[i] || semSaida(i)))
    {
        opcoes[num] = 3;
        ; // Dir
        num++;
    }

    if (num != 0)
    {
        num--; // removendo um valor do num, para igualar com o vetor

        // randomizando
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> distrib(0, num);

        int escolha = opcoes[distrib(gen)];

        // Reseta todas e ativa a escolhida
        fcima[i] = fbaixo[i] = fesq[i] = fdir[i] = false;
        if (escolha == 0)
            fcima[i] = true;
        else if (escolha == 1)
            fbaixo[i] = true;
        else if (escolha == 2)
            fesq[i] = true;
        else if (escolha == 3)
            fdir[i] = true;
    }
}

void mudarDirecaoInteligente(int i)
{
    double dist = 9999;
    int escolha = 0;
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1' && (!fbaixo[i] || semSaida(i))) // cima
    {
        double d = sqrt(pow(gatosx[4] - posx, 2) - pow(gatosy[4] - 1 - posy, 2));
        if (d < dist)
        {
            dist = d;
            escolha = 0;
        }
    }
    else if (mapa[gatosy[i] + 1][gatosx[i]] != '1' && (!fcima[i] || semSaida(i))) // baixo
    {
        double d = sqrt(pow(gatosx[4] - posx, 2) - pow(gatosy[4] + 1 - posy, 2));
        if (d < dist)
        {
            dist = d;
            escolha = 0;
        }
    }
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1' && (!fdir[i] || semSaida(i))) // esq
    {
        float d = sqrt(pow(gatosx[4] - 1 - posx, 2) - pow(gatosy[4] - posy, 2));
        if (d < dist)
        {
            dist = d;
            escolha = 0;
        }
    }
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1' && (!fesq[i] || semSaida(i))) // dir
    {
        float d = sqrt(pow(gatosx[4] + 1 - posx, 2) - pow(gatosy[4] - posy, 2));
        if (d < dist)
        {
            dist = d;
            escolha = 0;
        }
    }

    // Reseta todas e ativa a escolhida (nesse caso eh a q tiver a menor distancia direta com o alvo)
    fcima[i] = fbaixo[i] = fesq[i] = fdir[i] = false;
    if (escolha == 0)
        fcima[i] = true;
    else if (escolha == 1)
        fbaixo[i] = true;
    else if (escolha == 2)
        fesq[i] = true;
    else if (escolha == 3)
        fdir[i] = true;
}
int main()
{
    for (int i = 0; i < MAPALT; i++) // verifica a quantidade de pilulas
        for (int j = 0; j < MAPLARG; j++)
        {
            if (mapa[i][j] == '2')
                pontosmax++;
        }
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1100, 650}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));

    // quadrado para as margens do jogo
    sf::RectangleShape quadmargem({SIZE, SIZE});
    quadmargem.setFillColor(sf::Color(0, 20, 60));

    // pilulas
    sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));

    // sprites do PacMan
    sf::Texture texture;
    if (!texture.loadFromFile("peixe_dir.png"))
    {
        std::cout << "Erro lendo imagem peixe_dir.png\n";
        return 0;
    }
    sf::Sprite sprite{texture};

    sf::Texture texturesq;
    if (!texturesq.loadFromFile("peixe_esq.png"))
    {
        std::cout << "Erro lendo imagem peixe_esq.png\n";
        return 0;
    }
    sf::Sprite spritesq{texturesq};

    sf::Texture texturecim;
    if (!texturecim.loadFromFile("peixe_cima.png"))
    {
        std::cout << "Erro lendo imagem peixe_cima.png\n";
        return 0;
    }
    sf::Sprite spritecim{texturecim};

    sf::Texture texturebai;
    if (!texturebai.loadFromFile("peixe_baixo.png"))
    {
        std::cout << "Erro lendo imagem peixe_baixo.png\n";
        return 0;
    }
    sf::Sprite spritebai{texturebai};

    // sprites do fantasma
    sf::Texture texturegato;
    if (!texturegato.loadFromFile("gatinho.png"))
    {
        std::cout << "Erro lendo imagem gatinho.png\n";
        return 0;
    }
    sf::Sprite sprite2{texturegato};

    sf::Texture textureaquario;
    if (!textureaquario.loadFromFile("aquario.png"))
    {
        std::cout << "Erro lendo imagem aquario.png\n";
        return 0;
    }
    sf::Sprite sprite3{textureaquario};

    sf::Font fonte; // carrega fonte
    if (!fonte.openFromFile("Emulogic-zrEw.ttf"))
    {
        std::cout << "Erro lendo a fonte Emulogic-zrEw.ttf\n";
        return 0;
    }
    sf::Text placar(fonte); // cria um placar
    placar.setCharacterSize(40);
    placar.setFillColor(sf::Color::White);
    placar.setPosition({0.f, 0.f});

    sf::Text win(fonte); // texto de vitoria
    win.setString("Vitoria!!!");
    win.setCharacterSize(100);
    win.setFillColor(sf::Color::White);
    win.setPosition({10.f, 225.f});

    // cria um relogio para medir o tempo do PacMan
    sf::Clock clock;
    // cria um relogio para medir o tempo dos fantasmas
    sf::Clock fclock;
    // executa o programa enquanto a janela está aberta
    while (window.isOpen())
    {
        // verifica todos os eventos que foram acionados na janela desde a última iteração do loop
        while (const std::optional event = window.pollEvent())
        {
            // evento "fechar" acionado: fecha a janela
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                {
                    esq = true; // left key: PacMan tem intenção de se mover para esquerda
                    dir = cima = baixo = false;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                {
                    dir = true; // right key: PacMan tem intenção de se mover para direita
                    esq = cima = baixo = false;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                {
                    cima = true; // up key: PacMan tem intenção de se mover para cima
                    esq = dir = baixo = false;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                {
                    baixo = true; // down key: PacMan tem intenção de se mover para baixo
                    esq = dir = cima = false;
                }
            }
        }

        // Muda a posição do PacMan a cada 0.2 segundos
        if (clock.getElapsedTime() > sf::seconds(0.2))
        {                    // tempo desde último restart > 0.2s?
            clock.restart(); // recomeça contagem do tempo
            if (cima && mapa[posy - 1][posx] != '1')
            {
                posy--;
                inten[1] = -1; // salva a ultima instrução válida
                inten[0] = 0;  // zera a instrução no outro eixo
            }
            else if (baixo && mapa[posy + 1][posx] != '1')
            {
                posy++;
                inten[1] = +1;
                inten[0] = 0;
            }
            else if (esq && mapa[posy][posx - 1] != '1')
            {
                posx--;
                inten[0] = -1;
                inten[1] = 0;
            }
            else if (dir && mapa[posy][posx + 1] != '1')
            {
                posx++;
                inten[0] = +1;
                inten[1] = 0;
            }
            else if (mapa[posy + inten[1]][posx + inten[0]] != '1')
            {
                posy += inten[1];
                posx += inten[0];
            }
            if (posy >= MAPALT - 1) // verifica caso do pac fora do mapa
                posy = 1;           // inverte posição
            else if (posy < 1)
                posy = MAPALT - 2;
            else if (posx >= MAPLARG - 2) // verifica se chegou ao /0
                posx = 1;
            else if (posx < 1)
                posx = MAPLARG - 3; // transporta para fora de /0
        }
        // Muda a posição dos fantasmas a cada 0.2 segundos
        if (fclock.getElapsedTime() > sf::seconds(0.2))
        {
            fclock.restart();
            for (int i = 0; i < 4; i++) // para cada fantasma
            {
                if (ehEncruzilhada(i) || semSaida(i))
                {
                    mudarDirecao(i);
                }

                if (fcima[i])
                {
                    if (mapa[gatosy[i] - 1][gatosx[i]] != '1')
                        gatosy[i]--;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosy[i] < 1)
                        gatosy[i] = MAPALT - 2;
                }
                else if (fbaixo[i])
                {
                    if (mapa[gatosy[i] + 1][gatosx[i]] != '1')
                        gatosy[i]++;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosy[i] > MAPALT - 2)
                        gatosy[i] = 1;
                }
                else if (fesq[i])
                {
                    if (mapa[gatosy[i]][gatosx[i] - 1] != '1')
                        gatosx[i]--;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosx[i] < 1)
                        gatosx[i] = MAPLARG - 3;
                }
                else if (fdir[i])
                {
                    if (mapa[gatosy[i]][gatosx[i] + 1] != '1')
                        gatosx[i]++;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosx[i] > MAPLARG - 3)
                        gatosx[i] = 1;
                }
            }

            // movimentacao do fantasma "inteligente"
            // para cada direcao livre, ele escolhe a que esta mais perto do alvo, em uma linha reta
            double menordistancia = 999;
            double distancia;
            int direcaoescolhida = -1;
            if (mapa[gatosy[4] - 1][gatosx[4]] != '1')
            {
                distancia = sqrt(pow(gatosx[4] - posx, 2) + pow(gatosy[4] - 1 - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 0;
                }
            }
            if (mapa[gatosy[4] + 1][gatosx[4]] != '1')
            {
                distancia = sqrt(pow(gatosx[4] - posx, 2) + pow(gatosy[4] + 1 - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 1;
                }
            }
            if (mapa[gatosy[4]][gatosx[4] - 1] != '1')
            {
                distancia = sqrt(pow(gatosx[4] - 1 - posx, 2) + pow(gatosy[4] - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 2;
                }
            }
            if (mapa[gatosy[4]][gatosx[4] + 1] != '1')
            {
                distancia = sqrt(pow(gatosx[4] + 1 - posx, 2) + pow(gatosy[4] - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 3;
                }
            }

            // anda na direcao escolhida
            if (direcaoescolhida == 0)
            {
                if (gatosy[4] < 1)
                    gatosy[4] = MAPALT - 2;
                else
                    gatosy[4]--;
            }
            else if (direcaoescolhida == 1)
            {
                if (gatosy[4] > MAPALT - 2)
                    gatosy[4] = 1;
                else
                    gatosy[4]++;
            }
            else if (direcaoescolhida == 2)
            {

                if (gatosx[4] < 1)
                    gatosx[4] = MAPLARG - 3;
                else
                    gatosx[4]--;
            }
            else if (direcaoescolhida == 3)
            {
                if (gatosx[4] > MAPLARG - 3)
                    gatosx[4] = 1;
                else
                    gatosx[4]++;
            }
        }
        if (mapa[posy][posx] == '2') // sistema de pontos, conta e remove as pilulas
        {
            pontos++;
            mapa[posy][posx] = '0';
        }

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        // desenha paredes
        for (int i = 0; i < 13; i++)
            for (int j = 0; j < 23; j++)
                if (mapa[i][j] == '1')
                {
                    quad.setPosition({j * SIZE, i * SIZE});
                    window.draw(quad);
                }

        // desenha as margens
        for (int i = 0; i < 13; i++)
            for (int j = 0; j < 23; j++)
                if (mapa[i][j] == '3')
                {
                    quadmargem.setPosition({j * SIZE, i * SIZE});
                    window.draw(quadmargem);
                }

        // desenha pilulas
        for (int i = 0; i < 13; i++)
            for (int j = 0; j < 23; j++)
                if (mapa[i][j] == '2')
                {
                    circ.setPosition({j * SIZE + 25, i * SIZE + 25});
                    window.draw(circ);
                }

        // desenha PacMan
        if (inten[0] == -1)
        {
            spritesq.setPosition({posx * SIZE, posy * SIZE});
            window.draw(spritesq);
        }
        else if (inten[1] == -1)
        {
            spritecim.setPosition({posx * SIZE, posy * SIZE});
            window.draw(spritecim);
        }
        else if (inten[1] == 1)
        {
            spritebai.setPosition({posx * SIZE, posy * SIZE});
            window.draw(spritebai);
        }
        else
        {
            sprite.setPosition({posx * SIZE, posy * SIZE});
            window.draw(sprite);
        }
        // desenha fantasmas(gatinhos)
        for (int i = 0; i < 5; i++)
        {
            sprite2.setPosition({gatosx[i] * SIZE, gatosy[i] * SIZE});
            window.draw(sprite2);
        }

        // desenha a margem topo
        sprite3.setPosition({0, 0});
        window.draw(sprite3);

        placar.setString("Pontos: " + to_string(pontos));
        window.draw(placar);     // desenha o placar
        if (pontos == pontosmax) // condição de vitória
        {
            window.clear(sf::Color::Black);
            window.draw(win);
        }
        // termina e desenha o frame corrente
        window.display();
    }
    return 0;
}
