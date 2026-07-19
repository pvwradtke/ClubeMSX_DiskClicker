#include "msxgl.h"
#include "vdp.h"
#include "print.h"
#include "device/msx-hid.h"
#include "device/joymega.h"
#include "font/font_mgl_std1.h"
#include "arkos/akg_player.h"
#include "musica.h"
#include "efeitos.h"

volatile	u8 g_vBlank, g_joy8=0xFF, g_prevJoy8=0xFF, g_keyrow;
volatile 	u8 g_xj=16, g_yj=16, g_acertos, g_erros, g_colide, g_recorde=50;
volatile    char g_buffer[33];

void VDP_InterruptHandler();
void WaitVBlank();
void Splash();
void Titulo();
void Jogo();

inline void processInputs(){
	g_prevJoy8=g_joy8;
    g_joy8=JoyMega_Read3(INPUT_PORT1);	
	g_keyrow = Keyboard_Read(8);
	if(IS_KEY_PRESSED(g_keyrow, KEY_UP))
		g_joy8 &= ~JOYMEGA_IN_UP;
	if(IS_KEY_PRESSED(g_keyrow, KEY_DOWN))
		g_joy8 &= ~JOYMEGA_IN_DOWN;
	if(IS_KEY_PRESSED(g_keyrow, KEY_LEFT))
		g_joy8 &= ~JOYMEGA_IN_LEFT;
	if(IS_KEY_PRESSED(g_keyrow, KEY_RIGHT))
		g_joy8 &= ~JOYMEGA_IN_RIGHT;
	if(IS_KEY_PRESSED(g_keyrow, KEY_SPACE))
		g_joy8 &= ~JOYMEGA_IN_B;
	g_keyrow = Keyboard_Read(6);
	if(IS_KEY_PRESSED(g_keyrow, KEY_F1))
		g_joy8 &= ~JOYMEGA_IN_START;
	g_keyrow = Keyboard_Read(4);
	if(IS_KEY_PRESSED(g_keyrow, KEY_N))
		g_joy8 &= ~JOYMEGA_IN_A;
	if(IS_KEY_PRESSED(g_keyrow, KEY_M))
		g_joy8 &= ~JOYMEGA_IN_C;
}

void VDP_InterruptHandler(){
	g_vBlank=1;
    AKG_Update();
}

void WaitVBlank(){
	g_vBlank=0;
	while(g_vBlank == 0) { }
}

void Splash(){
    u8 espera=0;
    Print_Clear();
    Print_SetColor(3, 0);
    Print_DrawTextAt(7, 10, "REVISTA CLUBE MSX");
    Print_DrawTextAt(11, 12, "Apresenta");
    while(++espera<120){
        processInputs();
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            break;
        WaitVBlank();
    }
    Print_Clear();
    Print_SetColor(8, 0);
    Print_DrawTextAt(6, 9, "Um Exemplo MSXgl da");
    Print_DrawTextAt(11, 11, "CHIEN LOCO");
    espera=0;
    while(++espera<120){
        processInputs();
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            break;
        WaitVBlank();
    }
}

void Titulo(){
    u8  inicia=FALSE;
    Print_Clear();
    Print_SetColor(15, 0);
    String_Format(g_buffer, "Recorde: %d", g_recorde);
    Print_DrawTextAt(0, 0, g_buffer);
    Print_DrawTextAt(10, 8, "DISK CLICKER");
    Print_DrawTextAt(4, 14, "Aperte Espaco ou Botao B");
    Print_DrawTextAt(3, 18, "Programa: Paulo PV Radtke");
    Print_DrawTextAt(3, 19, "Grafico: Mario Cavalcanti");
    Print_DrawTextAt(9, 20, "Musica: Totta");
    Print_DrawTextAt(8, 23, "2026, CHIEN LOCO");

    while(!inicia){
        processInputs();
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            inicia=TRUE;
        WaitVBlank();
    }
    AKG_PlaySFX(2, ARKOS_CHANNEL_C,0);
    // Espera um segundo para tocar o som
    inicia=0;
    while(++inicia<=60){
        WaitVBlank();
    }
}

void Jogo(){
    g_erros=0;
    g_acertos=0;
    Print_Clear();
    AKG_Play(0, (const void*)0xA000);
    Print_SetColor(15, 0);
    while(g_erros<3){
        processInputs();
        if(JoyMega_IsPressedLeft(g_joy8))
            --g_xj;
        else if(JoyMega_IsPressedRight(g_joy8))
            ++g_xj;
        if(JoyMega_IsPressedUp(g_joy8)) 
            --g_yj;
        else if(JoyMega_IsPressedDown(g_joy8))
            ++g_yj;
        g_colide=FALSE;
        if(g_xj+3 >=120 && g_xj+3 <= 135)
            if(g_yj+1 >=88 && g_yj+1<= 103)
                g_colide=TRUE;
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            if(g_colide){
                ++g_acertos;
                AKG_PlaySFX(0, ARKOS_CHANNEL_C,0);
            }
            else{
                ++g_erros;
                AKG_PlaySFX(3, ARKOS_CHANNEL_C,0);
            }
        WaitVBlank();
        if(JoyMega_IsPressedB(g_joy8) && g_colide){
            VDP_SetSpriteSM1(3, 120, 88,0, 4);
            VDP_SetSpriteSM1(2, 120, 88,8, 15);
        }
        else{
            VDP_SetSpriteSM1(3, 120, 88, 0, 4);
            VDP_SetSpriteSM1(2, 120, 88, 4, 15);
        }
        if(JoyMega_IsPressedB(g_joy8))
            VDP_SetSpriteSM1(0, g_xj,g_yj, 16, 8);
        else
            VDP_SetSpriteSM1(0, g_xj,g_yj, 12, 8);
        VDP_SetSpriteSM1(1, g_xj,g_yj, 20, 9);
        String_Format(g_buffer, "Acertos: %d", g_acertos);
        Print_DrawTextAt(0, 0, g_buffer);
        String_Format(g_buffer, "Erros: %d", g_erros);
        Print_DrawTextAt(0, 23, g_buffer);
    }
    AKG_Play(1, (const void*)0xA000);
    // Coloca os sprites fora da tela
    for(u8 conta=0;conta<4;++conta)
        VDP_SetSpriteSM1(conta, 0, 193, 0, 0);

}

void GameOver(){
    u16 tempo=0;
    Print_Clear();
    Print_SetColor(15, 0);
    if(g_acertos>g_recorde){
        g_recorde=g_acertos;
        Print_DrawTextAt(9, 4, "Novo Recorde!");
    }
    Print_DrawTextAt(11, 11, "GAME OVER");
    String_Format(g_buffer, "Seu Placar: %d", g_acertos);
    Print_DrawTextAt(3, 20, g_buffer);
    String_Format(g_buffer, "Recorde: %d", g_recorde);
    Print_DrawTextAt(3, 21, g_buffer);
    AKG_PlaySFX(4, ARKOS_CHANNEL_C,0);
    while(++tempo<=300){
        processInputs();
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            break;
        WaitVBlank();
    }
}

void main()
{
	Bios_SetKeyClick(FALSE);
	VDP_SetMode(VDP_MODE_SCREEN1);
	VDP_SetColor(1);
    Print_Initialize();
    Print_SetMode(PRINT_MODE_TEXT);
    Print_SetTextFont(g_Font_MGL_Std1,33);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	VDP_LoadSpritePattern((const void*)sprites, 0, 24);
    DisableInterrupt();
    AKG_Play(1, (const void*)0xA000);
    AKG_InitSFX((const void*)0x9000);
	EnableInterrupt();
    VDP_EnableVBlank(TRUE);
	BIOS_SetHookCallback(H_TIMI, VDP_InterruptHandler);
    while(TRUE)
	{
        Splash();
        Titulo();
        Jogo();
        GameOver();
    }
}