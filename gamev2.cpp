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

const int MAPLARG = 23;
const int MAPALT = 13;
char mapa[MAPALT][MAPLARG] = { // Mapa do jogo
    "3333333333333333333333",
    "3121111111111111111213",
    "3141222222222222222213",
    "3121211211101112141213",
    "3121211212202212111213",
    "3222222200000002222223",
    "3111121212202212111213",
    "3111121211101112141213",
    "3122222222222222222213",
    "3121211121111121121213",
    "3141222222222222221213",
    "3121111111111111111213",
    "3333333333333333333333"};
const float SIZE = 50;      // Tamanho de cada célula do mapa

int posx = 9; // posicao do PacMan
int posy = 8;
int inten[2]={0,0}; //intenção de movimento
int pontos =0;
int pontosmax=0;

bool cima = false;  // direcao de movimento do PacMan
bool baixo = false;
bool esq = false;
bool dir = true;

bool vivo = true; //se o pacman  não foi morto pelos fantasmas
bool gatovivo[4] = {true, true, true, true}; // se os fantasmas n foram mortos pelo pacman
bool p2vivo = true;

// posições dos fantasmas / o ultimo eh o "inteligente"
int gatosx[] = {8, 12, 8, 12, 10};
int gatosy[] = {3, 3, 5, 5, 4};

// direcoes dos fantasmas
bool fcima[] = {true, false, false, false};
bool fbaixo[] = {false, true, false, false};
bool fesq[] = {false, false, true, false};
bool fdir[] = {false, false, false, true};

//pilula de força
bool pilulaAtiva = false; //verifica se a pilula esta ativa
sf::Clock tempoPilula; //relogio da pilula

float tempoFantasma = 0.2; // tempo normal do fantasma
struct Mov //infos do player2
{	
	bool input=false;
	bool vivo=true;
	bool di=false, es=false, up=false, dow=false;
	int movimentox=0, movimentoy=0;
	int posx=9,posy=4;
    float tempop2 = 0.2; // tempo normal do player 2
};
Mov p2;

// Função para verificar se há mais de 2 caminhos (o que ele veio + novas opções)
bool ehEncruzilhada(int i)
{ // verifica todos os 4 lados, se n for parede, add 1 unidade aos caminhos livres
    int caminhosLivres = 0;
    if (mapa[gatosy[i] - 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i] + 1][gatosx[i]] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] - 1] != '1')
        caminhosLivres++;
    if (mapa[gatosy[i]][gatosx[i] + 1] != '1')
        caminhosLivres++;

    return caminhosLivres > 2; // retorna true se tiver mais de 2 caminhos
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

    return caminhosLivres == 1; // se tiver apenas 1 caminho livre, significa q o unico caminho possivel eh voltar
}

// func que escolhe uma direcao, das disponiveis, para andar
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

        int escolha = opcoes[distrib(gen)]; // direcao escolhida

        // Reseta todas e ativa a escolhida
        fcima[i] = fbaixo[i] = fesq[i] = fdir[i] = false;
        // muda a direcao, dependendo da escolha
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

// func mudarDirecaoInteligente, pois nao estava sendo usada

int main() {
    for(int i=0;i<MAPALT;i++) //verifica a quantidade de pilulas
	    for(int j=0;j<MAPLARG;j++)
	    {
		if(mapa[i][j]=='2')
			pontosmax++;
	    }
    // cria a janela
    sf::RenderWindow window(sf::VideoMode({1100, 650}), "Pac-Man");

    // cria um quadrado de tamanho 50 (a parede)
    sf::RectangleShape quad({SIZE, SIZE});
    quad.setFillColor(sf::Color(0, 100, 200));
    quad.setOutlineThickness(-5);
    quad.setOutlineColor(sf::Color(50, 50, 50));
    
    //quadrado para as margens do jogo
    sf::RectangleShape quadmargem({SIZE, SIZE});
    quadmargem.setFillColor(sf::Color(0, 20, 60));
	
    //pilulas
     sf::CircleShape circ(5);
    circ.setFillColor(sf::Color(255, 255, 0));
    
    //pilula de força
    
     sf::CircleShape pilulaforca(10);
    pilulaforca.setFillColor(sf::Color(255, 165, 0));


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

    //textura do p2
    sf::Texture texturep2;
    if (!texturep2.loadFromFile("gatinho_rosa.png")) {
        std::cout << "Erro lendo imagem gatinho_rosa.png\n";
        return 0;
    }
    sf::Sprite spritep2{texturep2};
    
    // sprites do fantasma
    sf::Texture texturegato;
    if (!texturegato.loadFromFile("gatinho.png"))
    {
        std::cout << "Erro lendo imagem gatinho.png\n";
        return 0;
    }
    sf::Sprite sprite2{texturegato};

    sf::Texture textureaquario;
    if (!textureaquario.loadFromFile("aquario.png")) {
        std::cout << "Erro lendo imagem aquario.png\n";
        return 0;
    }
    sf::Sprite sprite3{textureaquario};

    sf::Texture texturegatoazul;
    if (!texturegatoazul.loadFromFile("gatinho_azul.png")) {
        std::cout << "Erro lendo imagem aquario.png\n";
        return 0;
    }
    sf::Sprite spritegatoazul{texturegatoazul};

    sf::Font fonte; //carrega fonte principal
    if (!fonte.openFromFile("Emulogic-zrEw.ttf")) {
        std::cout << "Erro lendo a fonte Emulogic-zrEw.ttf\n";
        return 0; 
    }
    sf::Font fonte2; //carrega fonte secundária
    if (!fonte2.openFromFile("PacfontGood-yYye.ttf")) {
        std::cout << "Erro lendo a fonte PacfontGood-yYye.ttf\n";
        return 0; 
    }
    
    sf::Text placar(fonte); //cria um placar 
    placar.setCharacterSize(40); 
    placar.setFillColor(sf::Color::White); 
    placar.setPosition({0.f, 0.f}); 

    sf::Text win(fonte2); //texto de vitoria
    win.setString("Vitoria!!! 1  9");
    win.setCharacterSize(100); 
    win.setFillColor(sf::Color::White); 
    win.setPosition({100.f, 225.f}); 

    sf::Text lose(fonte2); //texto de derrota
    lose.setString("Derrota 9000001");
    lose.setCharacterSize(100); 
    lose.setFillColor(sf::Color::Red); 
    lose.setPosition({100.f, 225.f}); 

    // cria um relogio para medir o tempo do PacMan
    sf::Clock clock;
    // cria um relogio para medir o tempo do player 2
    sf::Clock p2clock;
    // cria um relogio para medir o tempo dos fantasmas
    sf::Clock fclock;

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
		  if (keyPressed->scancode == sf::Keyboard::Scancode::W) { //detecção do segundo player
                      p2.input=true;
		      p2.up=true;
		      p2.dow=p2.di=p2.es=false;
                  }
		  else if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
		      p2.input=true;
		      p2.dow=true;
		      p2.up=p2.di=p2.es=false;
                  }
		  else if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
		      p2.input=true;
		      p2.es=true;
		      p2.up=p2.dow=p2.di=false;
                  }
		  else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
		      p2.input=true;
		      p2.di=true;
		      p2.up=p2.dow=p2.es=false;
                  }
            }
        }

	// Muda a posição do PacMan a cada 0.2 segundos
        if (clock.getElapsedTime() > sf::seconds(0.2)) { // tempo desde último restart > 0.2s?
            clock.restart();      // recomeça contagem do tempo
	    if (cima&&mapa[posy-1][posx]!='1')
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
	    else if (esq&&mapa[posy][posx-1]!='1')
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
	    if(posy>=MAPALT-1) //verifica caso do pac fora do mapa
		posy=1; //inverte posição
	    else if(posy<1) 
		posy=MAPALT-2;
	    else if(posx>=MAPLARG-2)//verifica se chegou ao /0 
		posx=1;
	    else if(posx<1) 
		posx=MAPLARG-3; //transporta para fora de /0
	}

	// Muda a posição do Player 2 de acordo com se a pilula esta ou n ativa
        if (p2clock.getElapsedTime() > sf::seconds(p2.tempop2)&&p2.input) 
        { 
            p2clock.restart(); 
	    if (p2.up&&mapa[p2.posy-1][p2.posx]!='1')
	    {
		    p2.posy--;
		    p2.movimentoy=-1; 
		    p2.movimentox=0; 
	    }
	    else if (p2.dow&&mapa[p2.posy+1][p2.posx]!='1')
	    {
		    p2.posy++;
		    p2.movimentoy=1; 
		    p2.movimentox=0;
	    }
	    else if (p2.es&&mapa[p2.posy][p2.posx-1]!='1')
	    {
		    p2.posx--;
		    p2.movimentoy=0; 
		    p2.movimentox=-1;    	    
	    }
	    else if (p2.di&&mapa[p2.posy][p2.posx+1]!='1') 
	    {
		    p2.posx++;
		    p2.movimentoy=0; 
		    p2.movimentox=1;
	    }
	    else if(mapa[p2.posy+p2.movimentoy][p2.posx+p2.movimentox]!='1')
	    {
		    p2.posy+=p2.movimentoy;
		    p2.posx+=p2.movimentox;
	    }
	    if(p2.posy>=MAPALT-1)
		p2.posy=1; 
	    else if(p2.posy<1) 
		p2.posy=MAPALT-2;
	    else if(p2.posx>=MAPLARG-2) 
		p2.posx=1;
	    else if(p2.posx<1) 
		p2.posx=MAPLARG-3;
	}
	if (pilulaAtiva)
	{
  	    tempoFantasma = 0.8; // fantasma fica mais lento quando a pilula ta ativa
  	    p2.tempop2 = 0.8; // fantasma controlavel tbm fica mais lento 
	}

	//ativa a pilula de força
    	if(mapa[posy][posx]=='4') 
	{
		mapa[posy][posx]='0';
        	pilulaAtiva = true;
        	tempoPilula.restart(); // começa a contar do zero o tempo da pilula
	}         
        //se o tempo da pilula acabou, volta ao normal
	if (pilulaAtiva && tempoPilula.getElapsedTime() > sf::seconds(7))
	{
        	pilulaAtiva = false;
        	tempoFantasma = 0.2;
        	p2.tempop2 = 0.2;
    	}

	// Muda a posição dos fantasmas de acordo com se a pilula esta ativa ou não
        if (fclock.getElapsedTime() > sf::seconds(tempoFantasma))
	{
            fclock.restart();
            for (int i = 0; i < 3; i++) // para cada fantasma
            {
                if (ehEncruzilhada(i) || semSaida(i)) // verifica se esta em encrusilhada ou sem saida
                {
                    mudarDirecao(i); // se tiver, muda de direcao
                }

                if (fcima[i]) // se for para cima
                {
                    if (mapa[gatosy[i] - 1][gatosx[i]] != '1') // e pra cima nao for parede
                        gatosy[i]--;                           // sobe
                    else
                    {
                        mudarDirecao(i); // caso contrario (seja parede), muda de direcao
                    }
                    if (gatosy[i] < 1)          // se o fantasma tiver atravessado a borda do mapa
                        gatosy[i] = MAPALT - 2; // teleporta para o lado oposto
                } // essa logica se segue para os prox 3 if
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
            if (mapa[gatosy[3] - 1][gatosx[3]] != '1') // se a direcao de cima nao for parede
            {
                distancia = sqrt(pow(gatosx[3] - posx, 2) + pow(gatosy[3] - 1 - posy, 2)); // calcula distancia
                if (distancia < menordistancia)                                            // se a distancia atual for menor do q a menor dist ate agr
                {
                    menordistancia = distancia; // distancia atual vira a menordistancia
                    direcaoescolhida = 0;       // direcao escolhida vira 0 (cima)
                }
            } // essa logica se repete nos 3 if
            if (mapa[gatosy[3] + 1][gatosx[3]] != '1')
            {
                distancia = sqrt(pow(gatosx[3] - posx, 2) + pow(gatosy[3] + 1 - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 1;
                }
            }
            if (mapa[gatosy[3]][gatosx[3] - 1] != '1')
            {
                distancia = sqrt(pow(gatosx[3] - 1 - posx, 2) + pow(gatosy[3] - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 2;
                }
            }
            if (mapa[gatosy[3]][gatosx[3] + 1] != '1')
            {
                distancia = sqrt(pow(gatosx[3] + 1 - posx, 2) + pow(gatosy[3] - posy, 2));
                if (distancia < menordistancia)
                {
                    menordistancia = distancia;
                    direcaoescolhida = 3;
                }
            }

            // anda na direcao escolhida
            // mesma logica dos outros fantasmas
            if (direcaoescolhida == 0)
            {
                if (gatosy[3] < 1)
                    gatosy[3] = MAPALT - 2;
                else
                    gatosy[3]--;
            }
            else if (direcaoescolhida == 1)
            {
                if (gatosy[3] > MAPALT - 2)
                    gatosy[3] = 1;
                else
                    gatosy[3]++;
            }
            else if (direcaoescolhida == 2)
            {

                if (gatosx[3] < 1)
                    gatosx[3] = MAPLARG - 3;
                else
                    gatosx[3]--;
            }
            else if (direcaoescolhida == 3)
            {
                if (gatosx[3] > MAPLARG - 3)
                    gatosx[3] = 1;
                else
                    gatosx[3]++;
            }
        }
	if(mapa[posy][posx]=='2') //sistema de pontos, conta e remove as pilulas
	{
		pontos++;
		mapa[posy][posx]='0';
	}

	//mata o fantasma 
	
        for(int i=0; i<4; i++)
        {
            if(gatovivo[i] && gatosx[i]==posx && gatosy[i]==posy)
            {
                if(pilulaAtiva)
                {
                    gatovivo[i]=false;
                }
                else 
                    vivo=false; //"mata" o pacman 
            }
        }
        // limpa a janela com a cor preta
        window.clear(sf::Color::Black);

        // desenhar tudo aqui...

        //desenha paredes
        for(int i=0;i<13;i++)
            for(int j=0;j<23;j++)
                if (mapa[i][j]=='1') {
                    quad.setPosition({j*SIZE, i*SIZE});
                    window.draw(quad);
                }
                
       //desenha as margens            
        for(int i=0;i<13;i++)
           for(int j=0;j<23;j++)
                if (mapa[i][j]=='3') {
                    quadmargem.setPosition({j*SIZE, i*SIZE});
                    window.draw(quadmargem);
                }
                
	//desenha pilulas
	for(int i=0;i<13;i++)
            for(int j=0;j<23;j++)
                if (mapa[i][j]=='2') {
                    circ.setPosition({j*SIZE + 25, i*SIZE + 25});
                    window.draw(circ);
                }

    //desenha a pilula de força
	for(int i=0;i<13;i++)
            for(int j=0;j<23;j++)
                if (mapa[i][j]=='4') {
                    pilulaforca.setPosition({j*SIZE + 20, i*SIZE + 20});
                    window.draw(pilulaforca);
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
	for(int i=0; i<4; i++)
  	{ 
        	if(gatovivo[i])
    		{   
        		if(!pilulaAtiva)
        		{
            			sprite2.setPosition({gatosx[i] * SIZE, gatosy[i] * SIZE});
            			window.draw(sprite2);
        		}
        		else
        		{
            			spritegatoazul.setPosition({gatosx[i] * SIZE, gatosy[i] * SIZE});
            			window.draw(spritegatoazul);  
        		}          
    		}   
  	}
	//desenha p2
	if(p2.input)
	{
		spritep2.setPosition({p2.posx*SIZE,p2.posy*SIZE});
       		window.draw(spritep2);
	}

	//desenha a margem topo 
        sprite3.setPosition({0,0});
        window.draw(sprite3);
	
	placar.setString("Pontos: " + to_string(pontos));
	window.draw(placar); //desenha o placar
        if(pontos==pontosmax&&vivo) //condição de vitória, check de "vivo" para evitar que o pacman ganhe post mortem
	{
		window.clear(sf::Color::Black);
		window.draw(win);
	}
	else if((p2.posx==posx&&p2.posy==posy&&p2.input)||!vivo) //condição de derrota do pacman/ vitoria do p2|fantasmas
	{	
       		 if(!pilulaAtiva)
       		 {
         		 vivo=false; //"mata" o pacman  
           		 window.clear(sf::Color::Black);
           		 window.draw(lose);
                 }
       		 else 
       		 {
            		 p2.input= false;
        	 }
	}
        // termina e desenha o frame corrente
        window.display();	
    }
    return 0;
}

