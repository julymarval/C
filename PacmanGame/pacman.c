//	Universidad Central de Venezuela
// 	Sistemas Operativos: Laboratorio 5 PACMAN
//  Rafael Vasquez CI: 20.116.813
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<ncurses.h>
#include<unistd.h>

// Altura y Anchuras maximas permitidas por el juego
int Alto = 0;
int Ancho = 0;

// Definicion de la matriz de caracteres principal.
char Campo[200][200];

// Vectores de movimiento
int movX[5] = {0,0,1,0,-1};
int movY[5] = {0,-1,0,1,0};

//Puntaje local y maximo
int puntos = 0;
int maxpuntos = 0;

//Status del juego; 1 = jugando, 0 = terminado
int inplay = 1;

// posicion y orientacion de PACMAN : Inicial: x = 14, y = 23
int posX = 0;
int posY = 0;
char orientation = 'd';

// posicion inicial de Blinky
int posActualB[2] = {0,1};

// posicion inicial de Pinky | Original: {1,26}
int posActualP[2] = {0,2};

// posicion inicial de Inky
int posActualI[2] = {0,3};

// posicion inicial de Clyde
int posActualC[2] = {0,4};

// Variables usadas en la manipulacion de hilos de procesamiento
pthread_t pman, ghost1, ghost2, ghost3, ghost4;
pthread_mutex_t lock;

//Funcion que se encarga de leer e inicializar las posiciones de pacman, puntos maximos y fantasmas.
void initPos(){
	int i,j;
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			switch(Campo[i][j]){
				case '.': 	maxpuntos++;
							break;
				case 'P':	posX = j;
							posY = i;
							Campo[i][j]=' ';
							break;
				case '1':	posActualB[0] = i;
							posActualB[1] = j;
							Campo[i][j]='.';
							maxpuntos++;
							break;
				case '2':	posActualP[0] = i;
							posActualP[1] = j;
							Campo[i][j]='.';
							maxpuntos++;
							break;
				case '3':	posActualI[0] = i;
							posActualI[1] = j;
							Campo[i][j]='.';
							maxpuntos++;
							break;
				case '4':	posActualC[0] = i;
							posActualC[1] = j;
							Campo[i][j]='.';
							maxpuntos++;
			}
		}
	}

}

//Verificamos si en la celdilla superior a la actual hay o no una pared.
int vefArriba(int i, int j){
	if(Campo[i-1][j] == '*' || (i-1==posActualB[0] && j==posActualB[1]) || (i-1==posActualP[0] && j==posActualP[1]) || (i-1==posActualI[0] && j==posActualI[1]) || (i-1==posActualC[0] && j==posActualC[1])) return 0;
	return 1;
}
//Verificamos si en la celdilla derecha a la actual hay o no una pared.
int vefDerecha(int i, int j){
	if(Campo[i][j+1] == '*' || (i==posActualB[0] && j+1==posActualB[1]) || (i==posActualP[0] && j+1==posActualP[1]) || (i==posActualI[0] && j+1==posActualI[1]) || (i==posActualC[0] && j+1==posActualC[1])) return 0;
	return 1;
}
//Verificamos si en la celdilla inferior a la actual hay o no una pared.
int vefAbajo(int i, int j){
	if(Campo[i+1][j] == '*' || (i+1==posActualB[0] && j==posActualB[1]) || (i+1==posActualP[0] && j==posActualP[1]) || (i+1==posActualI[0] && j==posActualI[1]) || (i+1==posActualC[0] && j==posActualC[1])) return 0;
	return 1;
}
//Verificamos si en la celdilla izquierda a la actual hay o no una pared.
int vefIzquierda(int i, int j){
	if(Campo[i][j-1] == '*' || (i==posActualB[0] && j-1==posActualB[1]) || (i==posActualP[0] && j-1==posActualP[1]) || (i==posActualI[0] && j-1==posActualI[1]) || (i==posActualC[0] && j-1==posActualC[1])) return 0;
	return 1;
}

//Inicializa el campo con puros 0
void InitMapa(){
	int i,j;
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			Campo[i][j] = '0';
		}
	}
}


//Mide la distancia entre 2 casillas
int DistanceFromTo(int y, int x, int i, int j){
	return abs(y - i) + abs(x - j);
}

// Verificamos la direccion indicada a movernos y si es que nos podemos mover a ella.
int MoveDirection(int ch, int y, int x){
	switch(ch){
	case 'w': 	if(vefArriba(y,x))	return 1; 	// Retorna Arriba
				else return 0;
				break;
				
	case 'd': 	if(vefDerecha(y,x)) return 2;	// Retorna derecha
				else return 0;
				break;
	case 's': 	if(vefAbajo(y,x)) return 3;		// Retorna abajo
				else return 0;
				break;
	}
	if(vefIzquierda(y,x)) return 4;				// Retorna Izquierda
	return 0;
}

// 
int DirtToInt(int dir){
	switch(dir){
	case 'w': 	return 1;
	case 'd':	return 2;
	case 's':	return 3;
	case 'a': 	return 4;
	}
return 0;
}

//Verifica si la coordenada "y" esta dentro del area del campo de juego
int inPlayVertical(int y){
	if( y < 0 ) return 0;
	else if( y >= Alto) return Alto - 1;
	return y;
}

//Verifica si la coordenada "x" esta dentro del area del campo de juego
int inPlayHorizontal(int x){
	if( x < 0 ) return 0;
	else if( x >= Ancho) return Ancho - 1;
	return x;
}
//Codigo que se encarga de calcular la coordenada vertical de la celdilla objetivo de Inky
int InkyAIvertical(){

	int target[2] = {0,0};
	
	switch(orientation){															
		case 'w': 	target[0] = inPlayVertical(posY - 2);
					target[1] = inPlayHorizontal(posX);		
					break;									
		case 's':	target[0] = inPlayVertical(posY + 2);		
					target[1] = inPlayHorizontal(posX);			
					break;										
		case 'a':	target[0] = inPlayVertical(posY);				
					target[1] = inPlayHorizontal(posX - 2);	
					break;										
		case 'd':	target[0] = inPlayVertical(posY);			
					target[1] = inPlayHorizontal(posX + 2);		
					break;											
	}
	
	target[0] = inPlayVertical( 2*target[0]  - posActualB[0]);	
	target[1] = inPlayHorizontal( 2*target[1]  - posActualB[1]);	

	return target[0];
}

//Codigo que se encarga de calcular la coordenada horizontal de la celdilla objetivo de Inky
int InkyAIhorizontal(){

	int target[2] = {0,0};
	
	switch(orientation){															
		case 'w': 	target[0] = inPlayVertical(posY - 2);
					target[1] = inPlayHorizontal(posX);		
					break;									
		case 's':	target[0] = inPlayVertical(posY + 2);		
					target[1] = inPlayHorizontal(posX);			
					break;										
		case 'a':	target[0] = inPlayVertical(posY);				
					target[1] = inPlayHorizontal(posX - 2);	
					break;										
		case 'd':	target[0] = inPlayVertical(posY);			
					target[1] = inPlayHorizontal(posX + 2);		
					break;											
	}
	
	target[0] = inPlayVertical( 2*target[0]  - posActualB[0]);	
	target[1] = inPlayHorizontal( 2*target[1]  - posActualB[1]);
	
	
	
	return target[1];
}

// Estoy en una encrucijada?
int isCrossroad(int i, int j){
	int paredes = 0;
	if(Campo[i-1][j] == '*' || (i-1==posActualB[0] && j==posActualB[1]) || (i-1==posActualP[0] && j==posActualP[1]) || (i-1==posActualI[0] && j==posActualI[1]) || (i-1==posActualC[0] && j==posActualC[1])) paredes++; //Hay pared o fantasma arriba? Si hay, se cuenta.
	if(Campo[i+1][j] == '*' || (i+1==posActualB[0] && j==posActualB[1]) || (i+1==posActualP[0] && j==posActualP[1]) || (i+1==posActualI[0] && j==posActualI[1]) || (i+1==posActualC[0] && j==posActualC[1])) paredes++; //Hay pared abajo? Si hay, se cuenta.
	if(Campo[i][j-1] == '*' || (i==posActualB[0] && j-1==posActualB[1]) || (i==posActualP[0] && j-1==posActualP[1]) || (i==posActualI[0] && j-1==posActualI[1]) || (i==posActualC[0] && j-1==posActualC[1])) paredes++; //Hay pared izquierda? Si hay, se cuenta.
	if(Campo[i][j+1] == '*' || (i==posActualB[0] && j+1==posActualB[1]) || (i==posActualP[0] && j+1==posActualP[1]) || (i==posActualI[0] && j+1==posActualI[1]) || (i==posActualC[0] && j+1==posActualC[1])) paredes++; //Hay pared derecha? Si hay, se cuenta.
	
	if(paredes < 2) return 1;  			// Estoy en encrucijada
	else if (paredes == 2) return 2;	// Estoy en un corredor
	return 3;							// Estoy en un DeadEnd
}

//Cual sera la proxima direccion a tomar por el fantasma?
int NextMoveDirection(int dir, int i, int j, int goalY , int goalX ){
	// variables para que la IA calcule que direccion tomar en intersecciones.
	int distance = 999999;
	int nmove = 0;
	
	switch(dir){
	// voy hacia arriba
				//Primero preguntamos si a donde nos vamos a mover hay un fantasma, si lo hay nos regresamos.
	case 'w':	 	if(isCrossroad( i , j ) == 1){	// Si estoy en una encrucijada
													// Verficamos que ruta con respecto a nuestra celda objetivo
													// es mas corta y viable y la tomamos.
						if(vefDerecha(i,j) && DistanceFromTo( goalY, goalX, i , j+1 ) < distance){
							distance = DistanceFromTo( goalY, goalX, i , j+1 );
							nmove = 'd';
						}
						if(vefArriba(i,j) && DistanceFromTo( goalY, goalX, i-1 , j ) < distance){
							distance = DistanceFromTo( goalY, goalX, i-1 , j );
							nmove = 'w';
						}
						if(vefIzquierda(i,j) && DistanceFromTo( goalY, goalX, i , j-1 ) < distance){
							distance = DistanceFromTo( goalY, goalX, i , j-1 );
							nmove = 'a';
						}
					
						distance = 999999;
						return nmove;	
						
					}else{ 	
						if(isCrossroad( i , j ) >= 2){	// Si estoy en un corredor
							if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
							if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
							if(vefIzquierda(i,j)) return 'a';
						// Si estoy en un DeadEnd me regreso.
						}else{
							if(vefAbajo(i,j)) return 's';
							if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
							if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
							if(vefIzquierda(i,j)) return 'a';
						}
					}
				break;
	// Voy hacia la derecha
	case 'd': 		if(isCrossroad( i , j ) == 1){		
						if(vefDerecha(i,j) && DistanceFromTo( goalY, goalX, i , j+1 ) < distance){
							distance = DistanceFromTo( goalY, goalX, i , j+1 );
							nmove = 'd';
						}
						if(vefArriba(i,j) && DistanceFromTo( goalY, goalX, i-1 , j ) < distance){
							distance = DistanceFromTo( goalY, goalX, i-1 , j );
							nmove = 'w';
						}
						if(vefAbajo(i,j) && DistanceFromTo( goalY, goalX, i+1 , j ) < distance){
							distance = DistanceFromTo( goalY, goalX, i+1 , j );
							nmove = 's';
						}
					
						distance = 999999;
						return nmove;
					
					}else{
						if(isCrossroad( i , j ) == 2){	// Si estoy en un corredor
							
							if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
							if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
							if(vefAbajo(i,j)) return 's';
						// Si estoy en un DeadEnd me regreso.
						}else{
							if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
							if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
							if(vefIzquierda(i,j)) return 'a';
							if(vefAbajo(i,j)) return 's';
						}	
					}
				break;
	// Voy hacia abajo.
		case 's': 	if(isCrossroad( i , j ) == 1){	
						if(vefDerecha(i,j) && DistanceFromTo( goalY, goalX, i , j+1 ) < distance){
							distance = DistanceFromTo( goalY, goalX, i , j+1 );
							nmove = 'd';
						}
						if(vefAbajo(i,j) && DistanceFromTo( goalY, goalX, i+1 , j ) < distance){
							distance = DistanceFromTo( goalY, goalX, i+1 , j );
							nmove = 's';
						}
						if(vefIzquierda(i,j) && DistanceFromTo( goalY, goalX, i , j-1 ) < distance){
							distance = DistanceFromTo( goalY, goalX, i , j-1 );
							nmove = 'a';
						}
					
						distance = 999999;
						return nmove;
						
					}else{
						if(isCrossroad( i , j ) == 2){	// Si estoy en un corredor
							if(vefAbajo(i,j)) return 's';
							if(vefDerecha(i,j)) return 'd';	
							if(vefIzquierda(i,j)) return 'a';
							
						// Si estoy en un DeadEnd me regreso.
						}else{
							if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
							if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
							if(vefIzquierda(i,j)) return 'a';
							if(vefAbajo(i,j)) return 's';
						}
					}
	}
	// Voy hacia la Izquierda
		if(isCrossroad( i , j ) == 1){	
			if(vefAbajo(i,j) && DistanceFromTo( goalY, goalX, i+1 , j ) < distance){
				distance = DistanceFromTo( goalY, goalX, i+1 , j );
				nmove = 's';
			}
			if(vefIzquierda(i,j) && DistanceFromTo( goalY, goalX, i , j-1 ) < distance){
				distance = DistanceFromTo( goalY, goalX, i , j-1 );
				nmove = 'a';
			}
			if(vefArriba(i,j) && DistanceFromTo( goalY, goalX, i-1 , j ) < distance){
				distance = DistanceFromTo( goalY, goalX, i-1 , j );
				nmove = 'w';
			}
		
			distance = 999999;
			return nmove;
			
		}else{
			if(isCrossroad( i , j ) == 2){
				if(vefIzquierda(i,j)) return 'a';
				if(vefArriba(i,j))	return 'w';
				if(vefAbajo(i,j)) return 's';
			}else{
				if(vefArriba(i,j))	return 'w';	// Seguimos derecho o a los lados. 
				if(vefDerecha(i,j)) return 'd';	// Para todos los casos.
				if(vefIzquierda(i,j)) return 'a';
				if(vefAbajo(i,j)) return 's';
			}
		}
	
	return 0;
}



void PrintMapa(){ // Solo para Debug
	int i,j;
	//char *charbuf;
	mvprintw(0,0, "Puntaje: %d", puntos);
	mvprintw(0,20, "Puntaje Maximo: %d", maxpuntos);
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			if(Campo[i][j] == '*'){
				attron(COLOR_PAIR(3));
				mvprintw(i+2,j, "%c" , Campo[i][j]);
				attroff(COLOR_PAIR(3));
			}else mvprintw(i+2,j, "%c" , Campo[i][j]);
		}
		//printw("\n");
	}
	
}

void PrintMapaNCurses(){ // Solo para Debug
	int i,j;
	//char *charbuf;
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			mvprintw(i,j, "%c" , Campo[i][j]);
		}
		//printw("\n");
	}
}

//Imprime toda la interface del juego, es decir el puntaje y el mapa 
void PrintInterface(){ 
	int i,j;
	//char *charbuf;
	mvprintw(0,0, "Puntaje: %d", puntos);
	mvprintw(0,20, "Puntaje Maximo: %d", maxpuntos);
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			if(Campo[i][j] == '*'){
				//IMPRIMIMOS PAREDES
				attron(COLOR_PAIR(3));
				mvprintw(i+2,j, "%c" , Campo[i][j]);
				attroff(COLOR_PAIR(3));
			}
			else if(i == posY && j == posX){
					//IMPRIMIMOS A PACMAN
					attron(COLOR_PAIR(1));
					mvprintw(posY+2,posX,"<");										
					attroff(COLOR_PAIR(1));
				}else if(i == posActualB[0]  && j == posActualB[1]){
						  //IMPRIMIMOS A BLINKY
						  attron(COLOR_PAIR(2));
						  mvprintw(posActualB[0]+2,posActualB[1],"B");
						  attroff(COLOR_PAIR(2));
					  }else if(i == posActualP[0]  && j == posActualP[1]){
					  			//IMPRIMIMOS A PINKY
					  			attron(COLOR_PAIR(4));
					  			mvprintw(posActualP[0]+2,posActualP[1],"P");
					  			attroff(COLOR_PAIR(4));
					 	    }else if(i == posActualI[0]  && j == posActualI[1]){
					 	    		  //IMPRIMIMOS A INKY
					 	    		  attron(COLOR_PAIR(5));
					 	    		  mvprintw(posActualI[0]+2,posActualI[1],"I");
					 	    		  attroff(COLOR_PAIR(5));
					 	    	  }
					 	    	  else if(i == posActualC[0]  && j == posActualC[1]){
					 	    	  	   		//IMPRIMIMOS A CLYDE
					 	    		   		attron(COLOR_PAIR(6));
					 	    		   		mvprintw(posActualC[0]+2,posActualC[1],"C");
					 	    		   		attroff(COLOR_PAIR(6));
					 	    	  	   }else mvprintw(i+2,j, "%c" , Campo[i][j]); // IMPRIMIMOS .,+ y espacios
		}
		//printw("\n");
	}
}

//Imprime Interface de Jugador ganador!
void PrintWinner(){ 
	int i,j;
	//char *charbuf;
	mvprintw(0,0, "Puntaje: %d", puntos);
	mvprintw(0,20, "Puntaje Maximo: %d", maxpuntos);
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			if(Campo[i][j] == '*'){
				attron(COLOR_PAIR(3));
				mvprintw(i+2,j, "%c" , Campo[i][j]);
				attroff(COLOR_PAIR(3));
			}else mvprintw(i+2,j, "%c" , Campo[i][j]);
		}
		//printw("\n");
	}
	attron(COLOR_PAIR(7));
	mvprintw(1,10,"YOU WIN!!");			 	    		 
	attroff(COLOR_PAIR(7));
}


//Imprime Mensaje de Game over en el tope de la pantalla al finalizar el mapa (gane o pierda)
void PrintLoser(){ 
	int i,j;
	//char *charbuf;
	mvprintw(0,0, "Puntaje: %d", puntos);
	mvprintw(0,20, "Puntaje Maximo: %d", maxpuntos);
	for(i = 0 ; i < Alto ; i++){
		for(j= 0 ; j < Ancho ; j++){
			if(Campo[i][j] == '*'){
				attron(COLOR_PAIR(3));
				mvprintw(i+2,j, "%c" , Campo[i][j]);
				attroff(COLOR_PAIR(3));
			}else mvprintw(i+2,j, "%c" , Campo[i][j]);
		}
		//printw("\n");
	}
	attron(COLOR_PAIR(7));
	mvprintw(1,10,"GAME OVER!!");			 	    		 
	attroff(COLOR_PAIR(7));
	
}

// Realiza una copia del mapa del archivo a una matriz local
void CopiarMapa(FILE *input){
	int charbuf;
	int i,j;
	for(i = 0 ; i < Alto ; i++){
		// Hay que tomar en cuenta el caracter '\n'
		for(j= 0 ; j <= Ancho ; j++){
			charbuf  =  fgetc(input);
			if (charbuf != 10)
				Campo[i][j] = charbuf;
			else
				continue;
		}
	}
}

// Codigo de control y dibujo de Pacman en pantalla
void* Pacman(void *arg){
	int posType;
	int chkey = 0;
	while(inplay){
	
		chkey = getch();
		
		if(chkey == 'w' || chkey == 's' || chkey == 'd' || chkey == 'a'|| chkey == 0){
			
			Campo[posY][posX]= ' ';
				//Movemos la posicion de pacman para el proximo refresh
			posX = posX + movX[MoveDirection( chkey, posY, posX )];
			posY = posY + movY[MoveDirection( chkey, posY, posX )];
			posType = Campo[posY][posX];
			switch(posType){
				case '.': 	puntos++;
							break;
				case '+':   puntos = puntos + 50;
							break;
			}
			orientation = chkey; // Actualizamos la orientacion de pacman
		}
				
		pthread_mutex_lock(&lock);		//INICIO DE MUTEX ACA/
		PrintInterface();									//
		//mvprintw(posY+2,posX,"<");						//
		refresh();											//
															//
		pthread_mutex_unlock(&lock);	///FIN DE MUTEX ACA///
		
		
		
		//mvprintw(0,15,"%c", chkey);
		//refresh();
		
		/// Si se llego a 441 puntos (todos los posibles) o lo come un fantasma. Se termina el juego.
		if (puntos >= maxpuntos) inplay = 0;
		else if((posActualB[0] == posY) && (posActualB[1] == posX)) inplay = 0;
			 else if((posActualP[0] == posY) && (posActualP[1] == posX)) inplay = 0;
			 	  else if((posActualI[0] == posY) && (posActualI[1] == posX)) inplay = 0;
			 	  		else if((posActualC[0] == posY) && (posActualC[1] == posX)) inplay = 0;
		
		usleep(3000);
		
		
	}
	return NULL;
}

// Codigo de AI y dibujo de Fantasma "B" en pantalla
void* Blinky(void *arg){
	//Blinky comienza en las esquina superior izquierda moviendose a la derecha.
	
	int pastMove = 'd';
	int nextMove = 0;
	
	// Indica la celdilla objetivo de Blinky.
	int target[2] = {0,0};
	
	while(inplay){
		
		pthread_mutex_lock(&lock);		//INICIO MUTEX//////////////////////////////////////////////////	
		
		//Movemos la posicion de blinky para el proximo refresh
		nextMove = NextMoveDirection( pastMove , posActualB[0], posActualB[1], target[0], target[1]);
		pastMove = nextMove;
		posActualB[1] = posActualB[1] + movX[DirtToInt(nextMove)];
		posActualB[0] = posActualB[0] + movY[DirtToInt(nextMove)];
			
		/// Si el fantasma llega a ocupar la misma celdilla de pacman. Se termina el juego.
		if((posActualB[0] == target[0]) && (posActualB[1] == target[1])) inplay = 0;
																																									//
		//Actualizamos la posicion de la celdilla objetivo de Blinky (PACMAN)			
		target[0]= posY;																
		target[1]= posX;																
		
					
		//clear();																		
		PrintInterface();																
		//mvprintw(posActualC[0]+2,posActualC[1],"M");									
		refresh();	
																						
		pthread_mutex_unlock(&lock);	//FIN MUTEX//////////////////////////////////////////////////////	
		
		usleep(300000);
	}
	return NULL;
}

// Codigo de AI y dibujo de Fantasma "2" en pantalla
void* Pinky(void *arg){

	int pastMove = 'a';
	int nextMove = 0;
	
	// Indica la celdilla objetivo de Pinky.
	int target[2] = {0,0};
	
	while(inplay){
		
		pthread_mutex_lock(&lock);		//INICIO MUTEX////////////////////////////////////////////////									//
		
		//Movemos la posicion de Pinky para el proximo refresh
		nextMove = NextMoveDirection( pastMove , posActualP[0], posActualP[1], target[0], target[1]);
		pastMove = nextMove;
		posActualP[1] = posActualP[1] + movX[DirtToInt(nextMove)];
		posActualP[0] = posActualP[0] + movY[DirtToInt(nextMove)];
																									
		//clear();																					
		PrintInterface();																			
		//mvprintw(posActualC[0]+2,posActualC[1],"M");												
		refresh();																					
																									
		//Actualizamos la posicion de la celdilla objetivo de Pinky (4 Celdas delante de pacman)	
			switch(orientation){																	
				case 'w': 	target[0] = inPlayVertical(posY - 4);									
							target[1] = inPlayHorizontal(posX);										
							break;																	
				case 's':	target[0] = inPlayVertical(posY + 4);									
							target[1] = inPlayHorizontal(posX);										
							break;																	
				case 'a':	target[0] = inPlayVertical(posY);										
							target[1] = inPlayHorizontal(posX - 4);									
							break;																	
				case 'd':	target[0] = inPlayVertical(posY);										
							target[1] = inPlayHorizontal(posX + 4);									
							break;																	
			}																						
																									
		pthread_mutex_unlock(&lock);	//FIN MUTEX///////////////////////////////////////////////////
		
		
			
		/// Si el fantasma llega a ocupar la misma celdilla de pacman. Se termina el juego.
		if((posActualP[0] == posY) && (posActualP[1] == posX)) inplay = 0;
		
		usleep(300000);
	}

	return NULL;
}

// Codigo de AI y dibujo de Fantasma "3" en pantalla
void* Inky(void *arg){
	
	int pastMove = 'w';
	int nextMove = 0;
	
	// Indica la celdilla objetivo de Inky.
	int target[2] = {0,0};
	
	while(inplay){
		
		pthread_mutex_lock(&lock);		//INICIO MUTEX////////////////////////////////////													//
		
		//Movemos la posicion de Inky para el proximo refresh
		nextMove = NextMoveDirection( pastMove , posActualI[0], posActualI[1], target[0], target[1]);
		pastMove = nextMove;
		posActualI[1] = posActualI[1] + movX[DirtToInt(nextMove)];
		posActualI[0] = posActualI[0] + movY[DirtToInt(nextMove)];
																						
		//clear();																		
		PrintInterface();																
		//mvprintw(posActualC[0]+2,posActualC[1],"M");									
		refresh();																		
																						
		//Actualizamos la posicion de la celdilla objetivo de Inky						
		//El posee una inteligencia unica, algo complicada.								
		target[0]= InkyAIvertical();													
		target[1]= InkyAIhorizontal();													
																						
		pthread_mutex_unlock(&lock);	//FIN MUTEX///////////////////////////////////////	
		
		/// Si el fantasma llega a ocupar la misma celdilla de pacman. Se termina el juego.
		if((posActualI[0] == posY) && (posActualI[1] == posX)) inplay = 0;
		
		usleep(300000);
	}
	
	
	return NULL;
}

// Codigo de AI y dibujo de Fantasma "4" en pantalla
void* Clyde(void *arg){

	int pastMove = 'a';
	int nextMove = 0;
	
	// Indica la celdilla objetivo de Clyde.
	int target[2] = {0,0};
	
	while(inplay){
		
		
		pthread_mutex_lock(&lock);		//INICIO MUTEX////////////////////////////////////
		
		//Movemos la posicion de Clyde para el proximo refresh
		nextMove = NextMoveDirection( pastMove , posActualC[0], posActualC[1], target[0], target[1]);
		pastMove = nextMove;
		posActualC[1] = posActualC[1] + movX[DirtToInt(nextMove)];
		posActualC[0] = posActualC[0] + movY[DirtToInt(nextMove)];
		
		//clear();																		
		PrintInterface();																
		//mvprintw(posActualC[0]+2,posActualC[1],"M");									
		refresh();																		
		//Actualizamos la posicion de la celdilla objetivo de Clyde 					
		//El intentara atrapar a pacman si esta a mas de 8 casillas de el 				
		//sino ira a la esquina inferior izquierda del mapa. (Fingir Ignorancia)		
		if(DistanceFromTo( posY, posX, posActualC[0] , posActualC[1] ) >= 8){			
			target[0]= posY;															
			target[1]= posX;															
		}else{																			
			target[0]= Alto-1;															
			target[1]= 0;																
		}																				
		pthread_mutex_unlock(&lock);	//FIN MUTEX///////////////////////////////////////	
			
		/// Si el fantasma llega a ocupar la misma celdilla de pacman. Se termina el juego.
		if((posActualC[0] == posY) && (posActualC[1] == posX)) inplay = 0;
		
		usleep(300000);
	}
	
	return NULL;
}

int main (int argc, char* argv[]){
 
	// Tomamos los datos pasados por parametros de ejecucion y los almacenamos.
	Alto = atoi(argv[1]);
	Ancho = atoi(argv[2]);
	puntos = 0;
	
	FILE *mapa;
	mapa = fopen( argv[3] ,"r");
	CopiarMapa(mapa);
	fclose(mapa); 
	
	//Inicializamos las posiciones de las entidades
	initPos();
	
	// Inicializamos el mutex "lock"
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Fallo de creacion mutex!\n");
        return 1;
    }
	
	// Prueba de impresion en pantalla con Ncurses
	initscr();
	if(has_colors() == FALSE){	
		endwin();
		printf("Esta terminal no soporta colores\n");
		exit(1);
	}
	
	start_color();

	// Definimos los colores a usar en el juego
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); 	//PACMAN
	init_pair(2, COLOR_WHITE, COLOR_RED);		//BLINKY
	init_pair(3, COLOR_BLACK, COLOR_BLUE);		//PAREDES
	init_pair(4, COLOR_WHITE, COLOR_MAGENTA);	//PINKY
	init_pair(5, COLOR_WHITE, COLOR_CYAN);		//INKY
	init_pair(6, COLOR_WHITE, COLOR_GREEN);		//CLYDE
	init_pair(7, COLOR_RED, COLOR_WHITE);		//Mensaje de GAME OVER!
	
	// Inicializamos las opciones de la pantalla del juego y probamos imprimir 1 vez
  	noecho();
  	cbreak();
  	curs_set(FALSE);
  	clear(); 
  	PrintInterface();
  	refresh();
  	
  	// INICIO DEL HILO DE BLINKY
  	pthread_create(&(ghost1), NULL, &Blinky , NULL);
  	//Blinky();
  	
  	// INICIO DEL HILO DE PINKY
  	pthread_create(&(ghost2), NULL, &Pinky , NULL);
  	//Pinky();
  	
  	// INICIO DEL HILO DE INKY
  	pthread_create(&(ghost3), NULL, &Inky , NULL);
  	//Inky();
  	
  	// INICIO DEL HILO DE CLYDE
  	pthread_create(&(ghost4), NULL, &Clyde , NULL);
  	//CLYDE();
  	
  	// INICIO DEL HILO DE PACMAN
  	pthread_create(&(pman), NULL, &Pacman , NULL);
  	//Pacman();
  	
  	// Finalizan los hilos del juego y se destruye el mutex
  	pthread_join(pman, NULL);
    pthread_join(ghost1, NULL);
    pthread_join(ghost2, NULL);
  	pthread_join(ghost3, NULL);
    pthread_join(ghost4, NULL);
    pthread_mutex_destroy(&lock);
  	
  	// Se determina el jugador gano o perdio y se muestra un mensaje acorde.
  	if(puntos >= maxpuntos){
  		PrintWinner();	refresh();
  		sleep(3);
  		clear();	PrintMapa();	refresh();
  		usleep(500000);
  		PrintLoser();	refresh();
  		usleep(500000);	
  		clear();	PrintMapa();	refresh();
  		usleep(500000);
  		PrintLoser();	refresh();
  		sleep(4);
  	}else{
  		PrintLoser();	refresh();
  		usleep(500000);	
  		clear();	PrintMapa();	refresh();
  		usleep(500000);
  		PrintLoser();	refresh();
  		usleep(500000);	
  		clear();	PrintMapa();	refresh();
  		usleep(500000);
  		PrintLoser();	refresh();
  		usleep(500000);	
  		clear();	PrintMapa();	refresh();
 		usleep(500000);
  		PrintLoser();	refresh();
  		sleep(3);
  	}

	// Se regresa el puntero y se cierra la ventana de juego para volver a consola.
	curs_set(TRUE);
	endwin();
	return 0;
}
