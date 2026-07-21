#include "msxgl.h"
#include "vdp.h"
#include "device/msx-hid.h"
#include "device/joymega.h"
#include "arkos/akg_player.h"
#include "string.h"
#include "musica.h"
#include "efeitos.h"
#include "sprites.h"
#include "tiles.h"
#include "maps.h"

volatile	u8 g_vBlank, g_joy8=0xFF, g_prevJoy8=0xFF, g_keyrow;
volatile 	u8 g_xj, g_yj, g_xd, g_yd, g_acertos, g_erros, g_colide, g_recorde=50;
volatile    char g_buffer[33];

void VDP_InterruptHandler();
void WaitVBlank();
inline void LimpaTela();
inline void DesenhaTela(const unsigned char *tela);
inline void Imprime(u8 x, u8 y, const char *texto);
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

inline void LimpaTela(){
    VDP_FillVRAM_16K(0, g_ScreenLayoutLow, 768);
}

inline void DesenhaTela(const unsigned char *tela){
    VDP_WriteVRAM_16K(tela, g_ScreenLayoutLow, 768);
}

void Imprime(u8 x, u8 y, const char *texto){
    // y<<5 é igual a y*32, mas muito mais rápido no Z80
    // Está entre parênteses porque deslocamento de bits tem menos prioridade que operador aritmético
    VDP_WriteVRAM_16K(texto, g_ScreenLayoutLow+x+(y<<5), String_Length(texto));
}

void Splash(){
    u8 espera=0;
    DesenhaTela(clube_msx);
    while(++espera<120){
        processInputs();
        if(JoyMega_IsPressedB(g_joy8) && !JoyMega_IsPressedB(g_prevJoy8))
            break;
        WaitVBlank();
    }
    DesenhaTela(chienloco);
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
    DesenhaTela(title);
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
    g_xd = 168;
    g_yd = 96;
    g_xj = 16;
    g_yj = 16;
    DesenhaTela(game);
    AKG_Play(0, (const void*)0xA000);
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
        if(g_xj+3 >=g_xd && g_xj+3 < g_xd+16)
            if(g_yj+1 >=g_yd && g_yj+1< g_yd+16)
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
            VDP_SetSpriteSM1(3, g_xd, g_yd,0, 4);
            VDP_SetSpriteSM1(2, g_xd, g_yd,8, 15);
        }
        else{
            VDP_SetSpriteSM1(3, g_xd, g_yd, 0, 4);
            VDP_SetSpriteSM1(2, g_xd, g_yd, 4, 15);
        }
        if(JoyMega_IsPressedB(g_joy8))
            VDP_SetSpriteSM1(0, g_xj,g_yj, 16, 8);
        else
            VDP_SetSpriteSM1(0, g_xj,g_yj, 12, 8);
        VDP_SetSpriteSM1(1, g_xj,g_yj, 20, 9);
        String_Format(g_buffer, "ACERTOS: %d", g_acertos);
        Imprime(0, 0, g_buffer);
        String_Format(g_buffer, "ERROS: %d", g_erros);
        Imprime(16, 0, g_buffer);
    }
    AKG_Play(1, (const void*)0xA000);
    // Coloca os sprites fora da tela
    for(u8 conta=0;conta<4;++conta)
        VDP_SetSpriteSM1(conta, 0, 193, 0, 0);

}

void GameOver(){
    u16 tempo=0;
    DesenhaTela(gameover);
    if(g_acertos>g_recorde){
        g_recorde=g_acertos;
        Imprime(9, 1, "NOVO RECORDE!");
    }
    String_Format(g_buffer, "SEU PLACAR: %d", g_acertos);
    Imprime(3, 19, g_buffer);
    String_Format(g_buffer, "RECORDE: %d", g_recorde);
    Imprime(3, 21, g_buffer);
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
	VDP_SetMode(VDP_MODE_SCREEN2);
	VDP_SetColor(1);
    LimpaTela();
    for(u8 conta=0;conta<3;++conta){
        VDP_WriteVRAM_16K(tiles_Patterns, g_ScreenPatternLow+2048*conta, 2048);
        VDP_WriteVRAM_16K(tiles_Colors, g_ScreenColorLow+2048*conta, 2048);
    }
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