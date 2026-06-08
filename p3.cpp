#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

using namespace std;

// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:        André Gustavo   15/06/23
// Movimentos Pac-Man:    André Gustavo   15/06/23
// Movimento contínuo:    André Gustavo   16/06/23
// Colisão com paredes:   não implementada
// Intenção de movimento: não implementado
// Atualizado para 3.1.0: André Gustavo   03/06/26

char mapa[11][21] = { // Mapa do jogo
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
    "12111111111111111121"};

const float SIZE = 50; // Tamanho de cada célula do mapa
// posições dos fantasmas
int gatosx[] = {8, 12, 8, 12};
int gatosy[] = {3, 3, 5, 5};

bool fcima[] = {true, false, false, false};
bool fbaixo[] = {false, true, false, false};
bool fesq[] = {false, false, true, false};
bool fdir[] = {false, false, false, true};

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
    sf::RenderWindow window(sf::VideoMode({1000, 550}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));

    sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));

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
        if (fclock.getElapsedTime() > sf::seconds(0.1))
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
                    if (gatosy[i] < 0)
                        gatosy[i] = 10;
                }
                else if (fbaixo[i])
                {
                    if (mapa[gatosy[i] + 1][gatosx[i]] != '1')
                        gatosy[i]++;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosy[i] > 10)
                        gatosy[i] = 0;
                }
                else if (fesq[i])
                {
                    if (mapa[gatosy[i]][gatosx[i] - 1] != '1')
                        gatosx[i]--;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosx[i] < 0)
                        gatosx[i] = 19;
                }
                else if (fdir[i])
                {
                    if (mapa[gatosy[i]][gatosx[i] + 1] != '1')
                        gatosx[i]++;
                    else
                    {
                        mudarDirecao(i);
                    }
                    if (gatosx[i] > 19)
                        gatosx[i] = 0;
                }
            }
        }

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        // desenha paredes
        for (int i = 0; i < 11; i++)
            for (int j = 0; j < 21; j++)
                if (mapa[i][j] == '1')
                {
                    quad.setPosition({j * SIZE, i * SIZE});
                    window.draw(quad);
                }

        for (int i = 0; i < 11; i++)
            for (int j = 0; j < 21; j++)
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

        // termina e desenha o frame corrente
        window.display();
    }

    return 0;
}
