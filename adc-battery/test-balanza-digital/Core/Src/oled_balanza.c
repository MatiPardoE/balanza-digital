/*
 * oled_balanza.c
 *
 *  Created on: Oct 24, 2021
 *      Author: tobias
 */

#include "main.h"
#include "bitmap.h"

/**
 * \fn 		: int count_digits(int number)
 * \brief 	: Cuenta la cantidad de digitos de un numero int
 * \details : Utilizada como funcion util para otras primitivas
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int number
 * \return 	: int digits
 * */
int count_digits(int number)
{
	int digits = 0;
	while(number)
	{
		number /= 10;
		digits++;
	}
	return digits;
}

/**
 * \fn 		: void printoled_battery(int state)
 * \brief 	: Imprime el icono de bateria correspondiente
 * \details : Los estados van de 4 (llena) a 0(vacia)
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int state
 * \return 	: none
 * */
void printoled_battery(int state)
{
	SSD1306_DrawBitmap(95, 0, battery_icon_4, 32, 18, 0); //LIMPIO LA ZONA
	SSD1306_UpdateScreen();
	switch(state){
	case 0:
		SSD1306_DrawBitmap(95, 0, battery_icon_0, 32, 18, 1); //ORIGINAL
		break;
	case 1:
		SSD1306_DrawBitmap(95, 0, battery_icon_1, 32, 18, 1); //ORIGINAL
		break;
	case 2:
		SSD1306_DrawBitmap(95, 0, battery_icon_2, 32, 18, 1); //ORIGINAL
		break;
	case 3:
		SSD1306_DrawBitmap(95, 0, battery_icon_3, 32, 18, 1); //ORIGINAL
		break;
	case 4:
		SSD1306_DrawBitmap(95, 0, battery_icon_4, 32, 18, 1); //ORIGINAL
		break;
	default:
		state = 0;
	}
	SSD1306_UpdateScreen();
}

/**
 * \fn 		: void printoled_menu(void)
 * \brief 	: Imprime el menu principal
 * \details : none
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: none
 * \return 	: none
 * */void printoled_menu()
{
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("MENU", &Font_7x10, 1);
	SSD1306_GotoXY(0,10);
	SSD1306_Puts("1- Pesar", &Font_7x10, 1);
	SSD1306_GotoXY(0,20);
	SSD1306_Puts("2- Calibrar", &Font_7x10, 1);
	SSD1306_GotoXY(0,30);
	SSD1306_Puts("3- Tarar", &Font_7x10, 1);
	SSD1306_GotoXY(0,40);
	SSD1306_Puts("4- Calcular Precio", &Font_7x10, 1);
	SSD1306_GotoXY(0,50);
	SSD1306_Puts("5- Conectar a PC", &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

 /**
  * \fn 		: void printoled_weight(int weight,int unit)
  * \brief 	: Imprime el peso incluyendo g o kg
  * \details : El peso se ingresa en gramos, unit=0 -> g, unit=1 (peso>1000) -> kg
  * \author 	: Gaston Cebreiro
  * \date   	: 09/09/2021
  * \param 	: [in] int weight
  * \param 	: [in] int unit
  * \return 	: none
  * */
void printoled_weight(int weight,int unit)
{
	int i,w,digits;
	int div = 1;

	if(!weight)
		digits = 1;
	else
		digits = count_digits(weight);

	if(weight < 1000)
		unit = 0;
	char weight_shown[digits+1+unit];

	if(digits > 1){
		for(i=1 ; i<digits ; i++)
			div*=10;
	}
	for(i=0 ; i<digits+unit ; i++){
		if( unit==1 && i==1 )
			weight_shown[i] = ',';
		else{
		w = (weight/div);
		weight_shown[i] = w + '0';
		weight -= w*div;
		div /= 10;
		}
	}
	weight_shown[i] = '\0';

	SSD1306_GotoXY(60-8*digits-20*unit,20);
	SSD1306_Puts(weight_shown, &Font_16x26, 1);
	if(unit){
		SSD1306_GotoXY(95,20);
		SSD1306_Puts("Kg", &Font_16x26, 1);
	}
	else{
		SSD1306_GotoXY(65+8*digits,20);
		SSD1306_Puts("g", &Font_16x26, 1);
	}
	SSD1306_UpdateScreen();
}

// IMPRIMIR EL TEXTO DE INICIO DE LA BALANZA
void printoled_start()
{
	 SSD1306_GotoXY(10,10);
	 SSD1306_Puts(" BALANZA",&Font_11x18,1);
	 SSD1306_GotoXY(10,30);
	 SSD1306_Puts(" DIGITAL", &Font_11x18, 1);
	 SSD1306_UpdateScreen();
}

/**
 * \fn 		: void printoled_number(int number)
 * \brief 	: Imprime el digito (0 a 9) recibido agregandolo al anterior
 * \details : Si recibe -1 limpia la pantalla, sino agrega un digito al numero, hasta 5
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int number
 * \return 	: none
 * */
void printoled_number(int number)
{
	static int i=0;
	static char number_shown[6];
	number_shown[i] = number + '0';
	number_shown[i+1] = '\0';
	if(number >= 0){
		SSD1306_GotoXY(55-6*i,20);
		SSD1306_Puts(number_shown, &Font_16x26, 1);
		SSD1306_UpdateScreen();
		i++;
	}
	else{
		i=0;
		number_shown[0] = '\0';;
		SSD1306_Clear();
	}
}

/**
 * \fn 		: void printoled_price(int price)
 * \brief 	: Imprime el precio
 * \details : Incluye el simbolo $
 * \author 	: Gaston Cebreiro
 * \date   	: 09/09/2021
 * \param 	: [in] int price
 * \return 	: none
 * */
void printoled_price(int price)
{
	int i,p,digits;
	int div = 1;
	digits = count_digits(price);
	char price_shown[digits+1];

	if(digits > 1){
			for(i=1 ; i<digits ; i++)
				div*=10;
	}
	/* ESTA VERSION ES CON EL SIGNO PESOS MAS CHICO PERO SE VE MEJOR*/
	SSD1306_GotoXY(52-8*digits,23);
	SSD1306_Putc('$', &Font_11x18, 1);
	for(i=0 ; i<digits ; i++)
	{
			p = (price/div);
			price_shown[i] = p + '0';
			price -= p*div;
			div /= 10;
	}
	price_shown[i] = '\0';

	SSD1306_GotoXY(65-8*digits,20);
	SSD1306_Puts(price_shown, &Font_16x26, 1);
	/**/
	/*//ESTA VERSION ES CON EL SIGNO PESOS GRANDE PERO SE VE FEO
	price_shown[0] = '$';
	for(i=1 ; i<=digits ; i++)
	{
			p = (price/div);
			price_shown[i] = p + '0';
			price -= p*div;
			div /= 10;
	}
	price_shown[i] = '\0';

	SSD1306_GotoXY(60-8*digits,20);
	SSD1306_Puts(price_shown, &Font_16x26, 1);
	*/

	SSD1306_UpdateScreen();
}
