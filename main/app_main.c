//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       _              //
//               _    _       _      _        _     _   _   _    _   _   _        _   _  _   _          //
//           |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|    //    
//         |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_   //
//                                                                                       /              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

// Área de inclusão das bibliotecas
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "HCF_IOTEC.h"
#include "HCF_LCD.h"
#include "HCF_ADC.h"
#include "HCF_MP.h"

#define senha_certa "1111"

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0;
static char tecla = '-';
char escrever[40];
char senha_escrita[5] = "";
uint8_t posicao_da_senha = 0;

void reset_sistema() {
    posicao_da_senha = 0;
    senha_escrita[0] = '\0';
    escreve_lcd(1, 0, "Digite a senha:");
    escreve_lcd(2, 0, "                ");
}

void app_main(void) {
    escrever[39] = '\0';
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versao do IDF: %s", esp_get_idf_version());

    iniciar_iotec();
    entradas = io_le_escreve(saidas);
    iniciar_lcd();
    escreve_lcd(1, 0, "Digite a senha:");
    iniciar_adc_CHX(0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while (1) {
        tecla = le_teclado();
        
        if (tecla >= '0' && tecla <= '9' && posicao_da_senha < 4) {
            senha_escrita[posicao_da_senha++] = tecla;
            senha_escrita[posicao_da_senha] = '\0';
            escreve_lcd(2, posicao_da_senha, "*");
        }
        else if (tecla == '=' && posicao_da_senha == 4) {
            if (strcmp(senha_escrita, senha_certa) == 0) {
                escreve_lcd(1, 0, "Senha correta!");
                escreve_lcd(2, 0, "Porta aberta  ");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                io_le_escreve(0b00000001);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                io_le_escreve(0b00000000);
                reset_sistema();
            } else {
                escreve_lcd(1, 0, "Senha incorreta!");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                reset_sistema();
            }
        }
        else if (tecla == 'C') {
            reset_sistema();
        }


        //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - -  -  -  -  -  -  -  -  -  -  Escreva seu só até aqui!!! //
        //________________________________________________________________________________________________________________________________________________________//
        vTaskDelay(10 / portTICK_PERIOD_MS);    // delay mínimo obrigatório, se retirar, pode causar reset do ESP
    }
    // caso erro no programa, desliga o módulo ADC
    adc_limpar();

    /////////////////////////////////////////////////////////////////////////////////////   Fim do ramo principal

 }

