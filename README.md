# criador.leitor.cod.barras
Criador e Leitor de Código de Barras padrão EAN-8

Arquivos para criar e ler códigos de barra no padrão EAN-8.
Comandos para criar os arquivos com uso de gcc:
Leitor: gcc ler_codigo_barras.c funcoes_compartilhadas.h -o leitor_codigo_barras -g -W
Criador: gcc criar_codigo_barras.c funcoes_compartilhadas.h -o criador_codigo_barras -g -W
Irá criar os arquivos executáveis leitor_codigo_barras.exe e criador_codigo_barras.exe respectivamente
