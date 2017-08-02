#!/bin/sh
# Codigo que muestra menu de mundos de pacman

# Muestra el menu general
_menu()
{
    
	figlet "PACMAN"
  
    echo "Escoge una opcion:"
    echo
    echo "1) Mapa original de Pacman 31 x 28"
    echo "2) Mapa alterno de Pacman 31 x 28"
    echo "3) Mapa personalizado"
    echo
    echo "9) Salir"
    echo
    echo "DISCLAIMER Opcion 3: Ajustar manualmente Alto y Ancho en shell script, los"
    echo "mapas pueden tener de 1 a 4 fantasmas y los mapas deben ser creados celda"
    echo "a celda, sin usar CTRL+C y CTRL+V, sino el juego no funcionara correctamente"
    echo
    echo
    echo -n "Indica una opcion: "
}

 
 
# opcion por defecto
opc="0"
 
# bucle mientas la opcion indicada sea diferente de 9 (salir)
until [ "$opc" -eq "9" ];
do
    case $opc in
        1)
            ./pacman 31 28 mapa2.txt
            clear
            _menu
            ;;

        2)
            ./pacman 31 28 mapa3.txt
            clear
            _menu
            ;;

# ACA SE MODIFICA EL ANCHO Y ALTO vvv

        3)
            ./pacman 9 10 mapa.txt
            clear
            _menu
            ;;

        *)
            # Esta opcion se ejecuta si no es ninguna de las anteriores
            clear
            _menu
            ;;

    esac

    read opc
done

clear
