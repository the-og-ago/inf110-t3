#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <cmath>

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
// posições dos fantasmas / o ultimo eh o "inteligente"
int gatosx[] = {8, 12, 8, 12, 10};
int gatosy[] = {3, 3, 5, 5, 4};

// direcoes dos fantasmas
bool fcima[] = {true, false, false, false, false};
bool fbaixo[] = {false, true, false, false, false};
bool fesq[] = {false, false, true, false, false};
bool fdir[] = {false, false, false, true, false};

// target (no jogo sera o pacman, aqui eh o fantasma 2)
int targetx = gatosx[1];
int targety = gatosy[1];

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
    std::vector<int> opcoes;
    // Adiciona as opções válidas, excluindo o caminho de volta (a direção oposta)
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1' && (!fbaixo[i] || semSaida(i)))
        opcoes.push_back(0); // Cima
    else if (mapa[gatosy[i] + 1][gatosx[i]] != '1' && (!fcima[i] || semSaida(i)))
        opcoes.push_back(1); // Baixo
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1' && (!fdir[i] || semSaida(i)))
        opcoes.push_back(2); // Esq
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1' && (!fesq[i] || semSaida(i)))
        opcoes.push_back(3); // Dir

    if (!opcoes.empty())
    {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dist(0, opcoes.size() - 1);
        int escolha = opcoes[dist(gen)];

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

int main()
{
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1100, 650}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));

    sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));

    // quadrado para as margens do jogo
    sf::RectangleShape quadmargem({SIZE, SIZE});
    quadmargem.setFillColor(sf::Color(0, 20, 60));

    // sprites do fantasma
    sf::Texture texturegato;
    if (!texturegato.loadFromFile("gatinho.png"))
    {
        std::cout << "Erro lendo imagem gatinho.png\n";
        return 0;
    }
    sf::Sprite sprite2{texturegato};

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
                distancia = sqrt(pow(gatosx[4] - gatosx[1], 2) + pow(gatosy[4] - 1 - gatosy[1], 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 0;
                }
            }
            if (mapa[gatosy[4] + 1][gatosx[4]] != '1')
            {
                distancia = sqrt(pow(gatosx[4] - gatosx[1], 2) + pow(gatosy[4] + 1 - gatosy[1], 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 1;
                }
            }
            if (mapa[gatosy[4]][gatosx[4] - 1] != '1')
            {
                distancia = sqrt(pow(gatosx[4] - 1 - gatosx[1], 2) + pow(gatosy[4] - gatosy[1], 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 2;
                }
            }
            if (mapa[gatosy[4]][gatosx[4] + 1] != '1')
            {
                distancia = sqrt(pow(gatosx[4] + 1 - gatosx[1], 2) + pow(gatosy[4] - gatosy[1], 2));
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

        // desenha fantasmas(gatinhos)
        for (int i = 0; i < 4; i++)
        {
            sprite2.setPosition({gatosx[i] * SIZE, gatosy[i] * SIZE});
            window.draw(sprite2);
        }

        // estou deixando o fantasma que persegue sendo um quadrado de parede, para ser mais visivel nos testes
        quad.setPosition({gatosx[4] * SIZE, gatosy[4] * SIZE});
        window.draw(quad);

        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
