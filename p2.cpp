//Thiago Luis de Arruda Rodrigues

//to-do-list
//*movimento contínuo (baseado em tempo) do pacman
//	*sistema de troca de sprites baseado na direção de movimento
//	*o Pacman deve possuir um estado de movimento (em que dire¸c˜ao est´a se movimentando) e uma inten¸c˜ao de movimento (para qual dire¸c˜ao o jogador quer que ele se movimente);
//		*ao pressionar uma tecla direcional, o jogador muda a inten¸c˜ao de movimento
//		*se as condi¸c˜oes atuais permitem alterar o estado de movimento para a inten¸c˜ao definida pelo jogador, ou seja, se n˜ao h´a parede adjacente na dire¸c˜ao escolhida, ele altera seu estado;
//		*en˜ao, ele continua se movimentando na dire¸c˜ao em que estava, ou seja, de acordo com seu estado; mas o sistema armazena a ´ultima inten¸c˜ao do jogador at´e que ela possa ser concretizada (ou que o jogador altere essa inten¸c˜ao, pressionando outra tecla).
//sistema de coleção de pilulas
//	sitema de placar de pontos (baseados em pilulas)
//	condicional de vitoria (baseado na quantidade de pilulas)
//*sistema de teletransporte nas bordas

#include <SFML/Graphics.hpp>
#include <iostream>

// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:        André Gustavo   15/06/23
// Movimentos Pac-Man:    André Gustavo   15/06/23
// Movimento contínuo:    André Gustavo   16/06/23
// Colisão com paredes:   não implementada
// Intenção de movimento: não implementado
// Atualizado para 3.1.0: André Gustavo   03/06/26
const int MAPLARG =21;
const int MAPALT =11;
char mapa[MAPALT][MAPLARG] = {     // Mapa do jogo
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
int inten[2]={0,0}; //intenção de movimento

int gato1x = 8,  gato1y = 3; //posições dos fantasmas
int gato2x = 12, gato2y = 3;
int gato3x = 8,  gato3y = 5;
int gato4x = 12, gato4y = 5;

bool cima = false;  // direcao de movimento do PacMan
bool baixo = false;
bool esq = false;
bool dir = true;

int main() {
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1000, 550}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));
	
    //pilulas
     sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));

    // sprites do PacMan
    sf::Texture texture;
    if (!texture.loadFromFile("peixe_dir.png")) {
        std::cout << "Erro lendo imagem peixe_dir.png\n";
        return 0;
    }
    sf::Sprite sprite{texture};

    sf::Texture texturesq;
    if (!texturesq.loadFromFile("peixe_esq.png")) {
        std::cout << "Erro lendo imagem peixe_esq.png\n";
        return 0;
    }
    sf::Sprite spritesq{texturesq};

    sf::Texture texturecim;
    if (!texturecim.loadFromFile("peixe_cima.png")) {
       std::cout << "Erro lendo imagem peixe_cima.png\n";
       return 0;
    }
    sf::Sprite spritecim{texturecim};

    sf::Texture texturebai;
    if (!texturebai.loadFromFile("peixe_baixo.png")) {
        std::cout << "Erro lendo imagem peixe_baixo.png\n";
        return 0;
    }
    sf::Sprite spritebai{texturebai};

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
	    if (cima&&mapa[posy-1][posx]!='1'|| posy==0) //evita a leitura de y=-1
	    {
		    posy--;
		    inten[1]=-1; //salva a ultima instrução válida
		    inten[0]=0; //zera a instrução no outro eixo	
	    }
	    else if (baixo&&mapa[posy+1][posx]!='1')
	    {
		    posy++;
		    inten[1]=+1;
		    inten[0]=0;
	    }
	    else if (esq&&mapa[posy][posx-1]!='1'|| posx==0)
	    {
		    posx--;
		    inten[0]=-1;
		    inten[1]=0;	    	    
	    }
	    else if (dir&&mapa[posy][posx+1]!='1') 
	    {
		    posx++;
		    inten[0]=+1;
		    inten[1]=0;
	    }
	    else if(mapa[posy+inten[1]][posx+inten[0]]!='1')
	    {
		    posy+=inten[1];
		    posx+=inten[0];
	    }
	    if(posy>=MAPALT) //verifica caso do pac fora do mapa
		posy=0; //inverte posição
	    else if(posy<0) 
		posy=MAPALT-1;
	    else if(posx>=MAPLARG-1)//verifica se chegou ao /0 
		posx=0;
	    else if(posx<0) 
		posx=MAPLARG-2; //transporta para fora de /0
	}

        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        //desenha paredes
        for(int i=0;i<11;i++)
            for(int j=0;j<21;j++)
                if (mapa[i][j]=='1') {
                    quad.setPosition({j*SIZE, i*SIZE});
                    window.draw(quad);
                }
	//desenha pilulas
	for(int i=0;i<11;i++)
            for(int j=0;j<21;j++)
                if (mapa[i][j]=='2') {
                    circ.setPosition({j*SIZE + 25, i*SIZE + 25});
                    window.draw(circ);
                }


        // desenha PacMan
	if(inten[0]==-1)
	{
		spritesq.setPosition({posx*SIZE,posy*SIZE});
       		window.draw(spritesq);
	}
	else if(inten[1]==-1)
	{
		spritecim.setPosition({posx*SIZE,posy*SIZE});
      		window.draw(spritecim);
	}
	else if(inten[1]==1)
	{
		spritebai.setPosition({posx*SIZE,posy*SIZE});
	       	window.draw(spritebai);
	}
	else
	{
		sprite.setPosition({posx*SIZE,posy*SIZE});
       		window.draw(sprite);
	}


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
