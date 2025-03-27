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
#include <stdio.h>                           // Inclusão da biblioteca para funções de entrada/saída padrão
#include <string.h>                          // Inclusão da biblioteca para manipulação de strings
#include "freertos/FreeRTOS.h"               // Inclusão das funções do FreeRTOS para multitarefa
#include "freertos/task.h"                   // Inclusão das funções de tarefa do FreeRTOS
#include "esp_log.h"                         // Inclusão para uso do log no ESP32

#include "HCF_IOTEC.h"                       // Inclusão do arquivo de cabeçalho da biblioteca I/O
#include "HCF_LCD.h"                         // Inclusão do arquivo de cabeçalho da biblioteca LCD
#include "HCF_ADC.h"                         // Inclusão do arquivo de cabeçalho da biblioteca ADC
#include "HCF_MP.h"                          // Inclusão do arquivo de cabeçalho da biblioteca MP (módulo de controle)

// Definições
#define IN(x) (entradas >> x) & 1            // Macro para ler os bits individuais das entradas
#define senha_certa "1111"                 // Definição da senha correta (agora "1111")
#define tentativas_max 3                     // Definindo o limite de tentativas (máximo 3 tentativas)

// Variáveis globais
static const char *TAG = "Placa";           // Definição do TAG para log (identificação do módulo)
static uint8_t entradas, saidas = 0;       // Variáveis para armazenar os estados das entradas e saídas (simuladas)
static char tecla = '-';                    // Variável para armazenar a tecla pressionada
char escrever[40];                          // Array para armazenar mensagens a serem exibidas no LCD
char senha_escrita[5] = "";               // Buffer para armazenar a senha digitada (máximo de 4 caracteres)
uint8_t posicao_da_senha = 0;                 // Variável que indica a posição atual da senha digitada
uint8_t tentativas = 0;                    // Contador de tentativas realizadas

// Função para resetar a tela e o sistema de senha
void reset_sistema()
{
    posicao_da_senha = 0;                      // Reseta a posição da senha
    senha_escrita[0] = '\0';               // Limpa a senha digitada
    escreve_lcd(1, 0, "Digite a senha:");   // Exibe no LCD a mensagem "Digite a senha:"
    escreve_lcd(2, 0, "                ");  // Limpa a segunda linha do LCD
}

// Função principal
void app_main(void)
{
    escrever[39] = '\0';                    // Garante que o buffer de escrita tem terminação nula
    
    ESP_LOGI(TAG, "Iniciando...");          // Log no terminal indicando que o sistema foi iniciado
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version()); // Exibe a versão do IDF sendo usada

    // Inicializações
    iniciar_iotec();                        // Inicializa o sistema de I/O
    entradas = io_le_escreve(saidas);       // Lê as entradas e escreve as saídas (simulando com a função)
    
    iniciar_lcd();                          // Inicializa o LCD
    escreve_lcd(1, 0, "Digite a senha:");   // Exibe a mensagem "Digite a senha:" na primeira linha do LCD

    iniciar_adc_CHX(0);                     // Inicializa o ADC (potencialmente para monitoramento de sinais analógicos)
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Aguarda 1 segundo para garantir que a inicialização seja concluída

    while (1)  // Loop infinito, que mantém o sistema funcionando
    {
        // Verifica se o número de tentativas excedeu o limite
        if (tentativas >= tentativas_max) {  // Se as tentativas forem maiores ou iguais ao máximo permitido
            escreve_lcd(1, 0, "Max. Tentativas!");   // Exibe no LCD que o número máximo de tentativas foi atingido
            escreve_lcd(2, 0, "Sistema Bloqueado");  // Exibe no LCD que o sistema foi bloqueado
            vTaskDelay(5000 / portTICK_PERIOD_MS);  // Aguarda 5 segundos para bloquear o sistema
            tentativas = 0;  // Reseta o contador de tentativas após o bloqueio
            reset_sistema();  // Chama a função que reseta o sistema (limpa a senha e exibe a mensagem inicial)
        }

        tecla = le_teclado();  // Lê a tecla pressionada no teclado

        // Lógica para digitar a senha
        if (tecla >= '0' && tecla <= '9' && posicao_da_senha < 4)  // Se a tecla for um número e não ultrapassar 4 dígitos
        {
            senha_escrita[posicao_da_senha++] = tecla;  // Armazena o dígito na posição correspondente
            senha_escrita[posicao_da_senha] = '\0';  // Garante que a string termine corretamente

            // Atualiza LCD com a máscara de asteriscos (****)
            sprintf(escrever, "****");  // A string escrita é sempre "****"
            escreve_lcd(2, 0, escrever);  // Exibe a máscara no LCD
        }
        // Verifica se a senha foi digitada e o sinal "=" foi pressionado
        else if (tecla == '=' && posicao_da_senha == 4)  // Se pressionado "=" e 4 caracteres foram digitados
        {
            if (strcmp(senha_escrita, senha_certa) == 0)  // Compara a senha digitada com a senha correta
            {
                // Exibe mensagens de sucesso ao validar a senha
                escreve_lcd(1, 0, "Senha correta!");  // Exibe no LCD "Senha correta!"
                escreve_lcd(2, 0, "Porta aberta  ");  // Exibe no LCD "Porta aberta"
                
                vTaskDelay(2000 / portTICK_PERIOD_MS);  // Aguarda 2 segundos antes de continuar

                // Simula a abertura da porta
                io_le_escreve(0b00000001);  // Comando para simular a abertura da porta (ativando a saída)

                vTaskDelay(2000 / portTICK_PERIOD_MS);  // Aguarda 2 segundos para a porta ficar aberta

                // Simula o fechamento da porta
                io_le_escreve(0b00000000);  // Comando para simular o fechamento da porta (desativando a saída)

                reset_sistema();  // Chama a função para resetar o sistema após a operação bem-sucedida
            }
            else  // Se a senha digitada estiver incorreta
            {
                tentativas++;  // Incrementa o contador de tentativas
                escreve_lcd(1, 0, "Senha incorreta!");  // Exibe no LCD que a senha está incorreta
                sprintf(escrever, "Tentativas: %d", tentativas);  // Prepara a string com o número de tentativas
                escreve_lcd(2, posicao_da_senha, escrever);

                vTaskDelay(2000 / portTICK_PERIOD_MS);  // Aguarda 2 segundos para o usuário visualizar a mensagem

                reset_sistema();  // Reseta o sistema (limpa o LCD e reinicia o processo)
            }
            posicao_da_senha = 0;
            senha_escrita[0] = '\0';
        }

        else if (tecla == 'C'){
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
