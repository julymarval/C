#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>

#define LONG_MAX_ARG 256
#define MAX_ARGS 20
#define LONG_MAX_PATH 256
#define MAX_PATHS 256
#define MAX_STRING 256
#define MAX_DIR MAX_STRING
#define MAX_HOSTNAME MAX_STRING


#define READ 0
#define WRITE 1

int cantidad_comando=0;
char ESPACIOBLANCO[]= " ";

struct orden_t 
{
	char **argv;	
	int argc;
	char *nombre;	
};
struct orden_t *historial;
char **dirs;
void mostrarIndicador ( ) {
/* Construir el indicador con el nombre de la máquina,
* el directorio actual y cualquier otra información deseada.
*/
char indicador[]= "...";

char *directorio_actual = (char *)malloc(sizeof(char)*MAX_DIR);
getcwd (directorio_actual,MAX_DIR); 
char *hostname = (char *)malloc(sizeof(char)*MAX_HOSTNAME);
gethostname(hostname,MAX_HOSTNAME);
getcwd(directorio_actual,MAX_DIR);


printf("[@%s:~%s]$ ",hostname,directorio_actual);	
//printf("%s\n", indicador);
}

void leerOrden(char *bufer) {
/* Este código utiliza cualquier función E/S, como aquellas de
* la biblioteca stdio para leer la línea de comando y
* almacenarla en el buffer. Esta implementación está muy
* simplificada, pero hace su trabajo.
*/

	fgets(bufer, MAX_STRING, stdin);

		
}

/*Esta es la función que analiza la línea de órdenes,
* devolviendo el resultado en el argumento struct orden_t *orden .*/

/* Determina el nombre de la orden y construye la lista de
* parámetros.* Esta función construirá argv[] y fijara el valor de
* argc.
* argc es el número de "tokens" o palabras en la línea de
* ordenes.
* argv[] es un array de cadenas de caracteres (apuntadores a
* char *). El último elemento en argv[] debe ser NULL, según
* registremos la línea de ordenes desde la derecha, el primer
* token va en argv[0], el segundo en * argv[l], y así.
* sucesivamente Cuando añadimos un token a argv[],
* incrementamos argc.

*/

int analizarOrden(char *lOrdenes, struct orden_t *orden) {
		
	int argc;
	char *lcPtr;
	lcPtr=(char *)malloc(256*sizeof(char *));
/* Inicialización */
	lcPtr= lOrdenes; /* lOrdenes es la línea de ordenes */
	argc = 0 ;
	orden->argv=(char **)malloc(sizeof(char *));
	orden->argv[argc] = (char *) malloc(LONG_MAX_ARG);
	
	
	/* Rellenar argv[ ] */
	if(lcPtr[0]=='\n'){
			return 0;
		}
		
	//Guardar en el historial
	if (strstr (lcPtr, "|")!=NULL)
	{
		
		strcpy(orden->argv[argc], lcPtr);
		orden->argv[++argc] = (char *) malloc(LONG_MAX_ARG);
		orden->argv[argc] = NULL;
		orden->argc = argc-1;
		orden->nombre = (char *) malloc(256*sizeof(char *));
		strcpy(orden->nombre, orden->argv[0]);
		return 1;
	}
	
	//Separar las ordenes 
	while ( (orden->argv[argc] = strsep(&lcPtr, ESPACIOBLANCO) )!= NULL)	{		 	
		
		orden->argv[++argc] = (char *) malloc(LONG_MAX_ARG);
	}
	
	orden->argv[argc] = NULL;
	size_t longitud = strlen(orden->argv[argc-1]);
   	*(orden->argv[argc-1] + longitud-1 ) = '\0';	

	//Testing
	//printf("ESTO ES UNA PRUEBA %s %s",lcPtr,orden->argv[1]);
	//printf("HUEHUE %s \n ",orden->argv[1]);
	//int i=0;
	//for (i=0;i<=orden->argc;i++)	printf("%s",orden->argv[i]);

	/* Fijar el nombre de la orden y argc */
	orden->argc = argc-1;
	orden->nombre = (char *) malloc(256*sizeof(char *));
	strcpy(orden->nombre, orden->argv[0]);
	
	
	
	if((strcmp(orden->argv[0],"r")==0)&&orden->argc>0)
	{
		strcat(orden->nombre," ");
        strcat(orden->nombre,orden->argv[1]);
	}
	
	return 1;

}

int analizarPath(char *dirs[]) {
/* Esta función lee la variable PATH de este entorno,
* entonces construye un array, dirs[], con los
* directories en PATH
*/
	char *varEntPath;
	char *elPath;
	int  i;
	for(i=0; i<MAX_ARGS; i++)
	{ dirs[i]=(char *)malloc(MAX_STRING*sizeof(char)); dirs[i] = NULL;
     }
	varEntPath = (char *) getenv( "PATH" );
	
	elPath = (char *) malloc(strlen( varEntPath) );
	strcpy ( elPath , varEntPath );
	printf("%s \n",varEntPath);

/* Ciclo para analizar elPath. Buscar un ' : '
* delimitando cada nombre de path.
*/
i=0;
while ( (dirs[i] = strsep(&elPath, ":") )!= NULL)	{		 	
		
		i++;
	}

i=0;


return 1;
}
char *buscarPath(char **argv, char **dir) {
/* Esta función busca si el argumento argv[0] (el nombre de
* archivo) está en los directorios en el argumento dir .
* Crea una nueva cadena de caracteres, coloca el nombre
* completo en ellos y devuelva la cadena de caracteres.
*/
	char *resultado;
	char *pName ;
;
	pName=(char *)malloc(LONG_MAX_PATH*sizeof(char *));
// Comprueba si el nombre del archivo ya es un camino absoluto
	if(*argv[0] == '/') {
		
			
			resultado = (char *) malloc(strlen( pName));
			strcpy(resultado,argv[0]);
			
			return resultado;
		
	}
// Buscar en los directorios del PATH.
 if(*argv[0]=='.'){
	
	 resultado = (char*)malloc(sizeof(char)*(strlen(argv[0])+1));
	 strcpy(resultado,argv[0]);
	 char* aux2;
     aux2=(char*)malloc(sizeof(char)*(strlen(argv[0])+1));
		int h;
		int l;
		l=0;
		  for (h=0;h<=strlen(resultado)+1;h++){
				if(h!=0)
				{
				aux2[l]=resultado[h];
				l++;
				}
				
			}
		char *aux1;
		aux1=(char*)malloc(sizeof(char)*256);
		getcwd(aux1,256);
		strcat(aux1,aux2);
    return  aux1;
  }
// Utilizar access() para ver si un archivo está en un dir.
	
	int i;
	for(i =0; i < MAX_PATHS && dir[i]!=NULL; i++) {
		strcpy(pName,dir[i]);
                strcat(pName,"/");
		strcat(pName,argv[0]);
		
		if(access(pName,F_OK)==0)
		{
		
			resultado = (char *) malloc(strlen( pName));
			strcpy(resultado,pName);
			return resultado;
		}
		
	}

// Nombre de archivo no encontrado en ningún sitio
	fprintf(stderr, "%s: comando no encontrado\n", argv[0]);

return NULL;
 
}
void catch(int sig)
{
    int i=0;
    fflush(stdout);
	printf(" \n");
    while(i<10)
    {
		if(strcmp(historial[i].nombre,"NULL" )!=0)
		{
			printf("r %d %s \n",i,historial[i].nombre);
			
			  fflush(stdout);
		}else{
			printf("r %d\n",i);
			  fflush(stdout);	
		}
		i++;
	}
	
	//mostrarIndicador ( );
	
}
 
int EjecutarComando(char*ruta,struct orden_t *aux){
	//printf("Ruta: %s, Comando: %s $,Parametro: %s?\n",ruta, aux->argv[0],aux->argv[1]);
    int u=execv(ruta,aux->argv);
    free(ruta);
	
    return u;
      
}
int changdiryexit(struct orden_t *mambo);
int pipeline(char *entrada)
{	
	char *lOrdenes=(char *)malloc(256*sizeof(char *));
	strcpy(lOrdenes,entrada);
	char *pName ;
	pName=(char *)malloc(256*sizeof(char *));
	int i=0;
	pName =lOrdenes;
	
	//printf("%s pipeline \n",pName );
	
	
	if (strstr (pName, "|")!=NULL)
	{
			//printf("%s entreeeeeee \n",pName );
			        
		char **comandos;
		char *comado;
		comado=(char *)malloc(sizeof(char )*100);
		char *comado2;
		comado2=(char *)malloc(sizeof(char )*100);
		
		comandos=(char **)malloc(2*sizeof(char *));
		i=0;
		comandos[i]=(char *)malloc(sizeof(char )*100);
		//char **aux;
		
		while ( (comandos[i] = strsep(&pName, "|") )!= NULL)	{		 	
		//printf("%s separando comando\n",comandos[i] );
		
		 comandos[++i]=(char *)malloc(sizeof(char )*100);
		 
		}
		strcpy(comado,comandos[0]);
		strcpy(comado2,comandos[1]);
		
		size_t longitud = strlen(comado2);
		*(comado2+ longitud-1 ) = '\0';
		comandos[i]=NULL;
		strcat(comado2,"r");
		strcat(comado,"r");
	
		/*printf("%s separando comando1 con\n",comado );
		printf("%s separando comando2 con \n",comado2 );*/
		
				struct orden_t orden_1;
				struct orden_t orden_2;
						if( analizarOrden(comado,(struct orden_t *)  &orden_1)){
						//	printf("%s argumentos\n",orden_1.nombre);
						if( analizarOrden(comado2,(struct orden_t *)  &orden_2)){
						//	printf("%s argumentos\n",orden_2.nombre);
						if(changdiryexit((struct orden_t *)  &orden_1)){
						if(changdiryexit((struct orden_t *)  &orden_2)){
							char *bufer2;
							
							bufer2=(char *)malloc(MAX_ARGS*sizeof(char *));	
							
							if((bufer2=buscarPath(orden_1.argv, dirs))==NULL)
							{
								return 0;
							}
							char *bufer3;
							bufer3=(char *)malloc(MAX_ARGS*sizeof(char *));	
							if((bufer3=buscarPath(orden_2.argv, dirs))==NULL)
							{
								return 0;
							}
							int status =0;
							pid_t wpid,pid;
							pid=fork();
							if(pid==0){
									
								 int fd[2];
								 pipe(fd);		
								if(fork()){ 
									
									 /* codigo del hijo */
									 close(fd[READ]);
									 dup2(fd[WRITE],1); /* redireccion de la salida al pipe */
									 close(fd[WRITE]);
									 i= EjecutarComando(bufer2,(struct orden_t *)  &orden_1);
								}else{
									
									wpid = wait(&status);
									//wait(&pid);
									close(fd[WRITE]); /* codigo del padre */
									dup2(fd[READ],0);
									close(fd[READ]);
									//printf("%s %s soy hijo \n",orden_1.nombre,orden_1.argv[1]);
									i= EjecutarComando(bufer3,(struct orden_t *)  &orden_2);	 
								 } 
								 free(bufer3);
								 free(bufer2);
								 fflush(stdout);
								 fflush(stdout);
								 fflush(stdout);
							
						 }	
						}
						}
						
					}		
				}
				return 0;
				//ls -la|grep pi1.c
				//grep pi1.c|ls -la			 										
	}
	return 1;
}

int changdiryexit(struct orden_t *mam){
	int o;
	struct orden_t *mambo;
	mambo=mam;
	o=strlen(mambo->nombre);
	if(strcmp(mambo->nombre,"time")==0||(mambo->nombre[o-1]=='e'&&mambo->nombre[o-2]=='m'&&mambo->nombre[o-3]=='i'&&mambo->nombre[o-4]=='t'&&mambo->nombre[o-5]=='/')){
		
		struct orden_t *aux;
		aux=(struct orden_t*)malloc(sizeof(struct orden_t));
		
		FILE *tiempo=fopen("Tiempo.txt","a");
		if(tiempo==NULL)
		{ 
			printf("No se pudo abrir el archivo..\n");
		}else{
			if( mambo->argv[1]==NULL){
				fprintf(tiempo,"real 0.00\nuser 0.00\nsys 0.00\n\n");
				fclose(tiempo);
				return 0;
			}
		
		fprintf(tiempo,"%s\n", mambo->argv[1]);
		}

		
		if( mambo->argv[1]==NULL){
							return 0;
			}
		
		aux->nombre=(char*)malloc(sizeof(char)*(strlen(mambo->nombre)+1));
		strcpy(aux->nombre,mambo->nombre);

		aux->argc=mambo->argc +3;
		  int i=0,j=0;
		aux->argv=(char**)malloc(sizeof(char*)*(aux->argc+3));  
		while(i< aux->argc && j<mambo->argc){
		  if(i==1){
		     aux->argv[i]=(char*)malloc(sizeof(char)*(3));
		      strcpy(aux->argv[i],"-p");
		      
		      i++;
		       
		  }
		  if(i==2){
		  	  aux->argv[i]=(char*)malloc(sizeof(char)*(20));
		      strcpy(aux->argv[i],"--output=Tiempo.txt");
		      
		      i++;
		  	}
		  if(i==3){
		  	  aux->argv[i]=(char*)malloc(sizeof(char)*(3));
		      strcpy(aux->argv[i],"-a");
		      
		      i++;
		  	}
		   aux->argv[i]=(char*)malloc(sizeof(char)*(strlen(mambo->argv[j])+1));
		   strcpy(aux->argv[i],mambo->argv[j]); 
		    	
		   i++; 
		   j++; 

		}
		
		aux->argv[aux->argc]=NULL;

		char ** Arr=(char**)malloc(sizeof(char*)*1);
		Arr[0]=(char*)malloc(sizeof(char)*(strlen(aux->argv[4])+1));
		strcpy(Arr[0],aux->argv[4]);

		//printf(" %s salgo time\n",aux->argv[4]);
		if(buscarPath(Arr,dirs)!=NULL){
		char* extra =(char*)malloc(sizeof(char)*(strlen(buscarPath(Arr,dirs))+1));
			
		  strcpy(extra,buscarPath(Arr,dirs));
		  
		  
		  
			//printf("salgo time\n");
		aux->argv[4]=(char*)malloc(sizeof(char)*(strlen(extra)+1));
		strcpy(aux->argv[4], extra);
		
		  char* ultimo=buscarPath(aux->argv,dirs);
		 int pid1=fork();
		  if(pid1==0){
		  int a11;
		  a11=EjecutarComando(ultimo,aux);
			  if(a11==-1){
				printf("no existe el comando %s para poder ejecutarse en el time\n",aux->nombre);
				fflush(stdout);
				exit(0);
			  
			  }
		  }else{

		      wait(&pid1);
		  	
		  }

		
	      } else{
	      		printf("no existe el comando %s para poder ejecutarse en el time\n",Arr[0]);
	      		fflush(stdout);
	      }
	      if(tiempo!=NULL){
				fprintf(tiempo,"\n");
				fclose(tiempo);
			}
			free(Arr);
			free(aux);
		  return 0;

}  else if(strcmp(mambo->nombre,"exit")==0){
	      free(historial);
	      pthread_exit(0);
  	      exit(0);
  	} else if(strcmp(mambo->nombre,"cd")==0){
		if(mambo->argv[1]==NULL){
			if(chdir("/")==-1)
			printf("No se pudo cambiar de directorio\n");
			fflush(stdout);
		}else if(chdir(mambo->argv[1])==-1){ 
			printf("El directorio no existe %s \n",mambo->argv[1]);
			fflush(stdout);
		}
		return 0;
	}else if ( (strcmp(mambo->nombre,"r")==0) || (strcmp(mambo->nombre,"r 0")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 1")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 2")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 3")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 4")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 5")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 6")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 7")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 8")==0&&mambo->argc==1) || (strcmp(mambo->nombre,"r 9")==0&&mambo->argc==1)){
		  int prueba;
		 
		  if ( (strcmp(mambo->nombre,"r 0")==0 )) prueba=0;
		  else if ( strcmp(mambo->nombre,"r")==0) prueba=cantidad_comando-1;	
		  else  prueba=(int ) mambo->nombre[2]-48;
		
		

		  if(changdiryexit( (struct orden_t *)  &historial[prueba])){
			
		      
		    //  printf("%s historial \n",historial[prueba].argv[0]);
		      
		      if(strcmp(historial[prueba].nombre,"NULL")!=0){ 
				if(pipeline(historial[prueba].argv[0])){
				char *direc;
				direc=(char *)malloc(MAX_ARGS*sizeof(char *));	
				
				
				direc=buscarPath(historial[prueba].argv,dirs);
				
				 int pid=fork();
				if(pid==0){
					int  a1=EjecutarComando(direc,(struct orden_t *)  &historial[prueba]);
					if(a1==-1){
						fprintf(stderr,"%s: comando no dencontrado\n",historial[prueba].nombre);
    					
					 
						exit(0);
						}
				}else{
					wait(&pid);
				}
			}
			sleep(1);
			
			} else{ 
	  		printf("no se ha introducido ese numero de comandos \n"); 
	  		fflush(stdout);
	  		}
		}
		return 0;
	}
	
	return 1;
}

int main()
{
	int i=0;
		int j=0;
    historial=(struct orden_t *)malloc(10*sizeof(struct orden_t));
	  for(i=0;i<10;i++){
       
	  historial[i].nombre=(char*)malloc(sizeof(char)*5);
	  strcpy(historial[i].nombre,"NULL");
	// historial[i].argv=(char **)malloc(sizeof(char *));
     historial[i].argv=(char**)malloc(sizeof(char*));
		j=0;
	while(j<9)
	{
		historial[i].argv[j] = (char *) malloc(LONG_MAX_ARG);
		 strcpy(historial[i].argv[0],"NULL1");
		
		j++;
	}
	
   }
	
    dirs=(char **)malloc(MAX_ARGS*sizeof(char *));
	i=analizarPath(dirs);
	
	//signal(SIGQUIT, &salgo);
	struct orden_t orden;
	
		
	while(1){
		
		sleep(2);
		if (signal (SIGINT, catch) == SIG_ERR){
			perror ("No se puede cambiar signal\n");
			fflush(stdout);
		}
	
		char *bufer;
		bufer=(char *)malloc(256*sizeof(char *));
		char *bufer2;
		bufer2=(char *)malloc(MAX_ARGS*sizeof(char *));	
		char *bufer_aux;
		bufer_aux=(char *)malloc(256*sizeof(char *));
		
		mostrarIndicador ( );
		
		leerOrden( bufer);
	
		strcpy(bufer_aux,bufer);
		//printf("retorna buffer %s\n", bufer);
		
	  if(pipeline(bufer)){


			//printf("retorna buffer %s\n", bufer);
		 	if( analizarOrden(bufer,(struct orden_t *)  &orden)){
			//printf("%s \n",orden.argv[0]);
			if(changdiryexit((struct orden_t *)  &orden)){
			//printf("%d\n ",cantidad_comando);
			
				if(cantidad_comando>9)
				{
					i =0;
					while(i<9)
					{
						 historial[i].nombre=(char*)malloc(sizeof(char)*(strlen(historial[i+1].nombre)+1));
						 strcpy(historial[i].nombre,historial[i+1].nombre);
						 historial[i].argc=historial[i+1].argc;
						 historial[i].argv=(char**)malloc(sizeof(char*)*(historial[i+1].argc+1));
						for(j=0;j<historial[i+1].argc;j++){
							historial[i].argv[j]=(char*)malloc(sizeof(char)*(strlen(historial[i+1].argv[j])+1));
							strcpy(historial[i].argv[j],historial[i+1].argv[j]);
							
						} 
						i++;	
					}
					cantidad_comando=9;
				}
				 // printf("%s  \n",orden.argv[0]);
				  strcpy(historial[cantidad_comando].nombre,orden.nombre);
				  historial[cantidad_comando].argc=orden.argc;
					 for(j=0;j<orden.argc+1;j++){
					strcpy(historial[cantidad_comando].argv[j],orden.argv[j]);
					 }
				  historial[cantidad_comando].argv[j]=NULL;
				  strcpy(historial[cantidad_comando].argv[0],orden.argv[0]);
				   cantidad_comando++;
				   i=0;
				   if((bufer2=buscarPath(orden.argv, dirs))!=NULL)
				   {
				    
					int pid=fork();
					if(pid==0){

						 i= EjecutarComando(bufer2,(struct orden_t *)  &orden);   
							sleep(2);
						 if(i==-1){ 
						 
						 exit(0);
						}		 
						 exit(0);
					}else{
					
						wait(&pid);
					  
					 }
				}
			}else{
				sleep(2);
				int o;
				o=strlen(orden.nombre);
				if((strcmp(orden.nombre,"time")==0||(orden.nombre[o-1]=='e'&&orden.nombre[o-2]=='m'&&orden.nombre[o-3]=='i'&&orden.nombre[o-4]=='t'&&orden.nombre[o-5]=='/'))||strcmp(orden.nombre,"cd")==0){
	
				if(cantidad_comando>9)
				{
					i =0;
					while(i<9)
					{
						 historial[i].nombre=(char*)malloc(sizeof(char)*(strlen(historial[i+1].nombre)+1));
						 strcpy(historial[i].nombre,historial[i+1].nombre);
						 historial[i].argc=historial[i+1].argc;
						 historial[i].argv=(char**)malloc(sizeof(char*)*(historial[i+1].argc+1));
							 for(j=0;j<historial[i+1].argc;j++){
							historial[i].argv[j]=(char*)malloc(sizeof(char)*(strlen(historial[i+1].argv[j])+1));
							strcpy(historial[i].argv[j],historial[i+1].argv[j]);
							} 
						i++;	
					}
					cantidad_comando=9;
				
				}
			  
				strcpy(historial[cantidad_comando].nombre,orden.nombre);
				historial[cantidad_comando].argc=orden.argc;
				if(strcmp(orden.nombre,"cd")==0)
				{
			
				historial[cantidad_comando].argv[0]=orden.nombre;
				historial[cantidad_comando].argv[1]=NULL;
				}else{
				 for(j=0;j<orden.argc&&orden.argc;j++){
					historial[cantidad_comando].argv[j]=orden.argv[j];
					}
			
				historial[cantidad_comando].argv[j]=NULL;		
			 	
				strcpy(historial[cantidad_comando].argv[0],orden.argv[0]);
				}
				
						cantidad_comando++;
			
				}
			}
		}
	}else{
		sleep(1);
		 
		 if( analizarOrden(bufer_aux,(struct orden_t *)  &orden))
		// printf("%s historial \n",orden.argv[0]);
		if(cantidad_comando>9)
			{
				i =0;
				while(i<9)
				{
					 historial[i].nombre=(char*)malloc(sizeof(char)*(strlen(historial[i+1].nombre)+1));
					 strcpy(historial[i].nombre,historial[i+1].nombre);
					 historial[i].argc=historial[i+1].argc;
					 historial[i].argv=(char**)malloc(sizeof(char*)*(historial[i+1].argc+1));
						 for(j=0;j<historial[i+1].argc;j++){
						historial[i].argv[j]=(char*)malloc(sizeof(char)*(strlen(historial[i+1].argv[j])+1));
						strcpy(historial[i].argv[j],historial[i+1].argv[j]);
						} 
					i++;	
				}
				cantidad_comando=9;
			}
			  strcpy(historial[cantidad_comando].nombre,orden.nombre);
	   //ls -la|grep proyec.c

		historial[cantidad_comando].argc=orden.argc;
		historial[cantidad_comando].argv[0]=orden.nombre;
		//printf("Historial de la orden:%s\n",historial[cantidad_comando].argv[0]);
		historial[cantidad_comando].nombre=orden.nombre;
		historial[cantidad_comando].argv[1]=NULL;		
				cantidad_comando++;
	
	
	}
}
	return  0;
	
	
}
