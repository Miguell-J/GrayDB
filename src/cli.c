#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/cli.h"
#include "../include/dsl.h"

void cli_logo() {
    printf("\n");
    printf("  .g8'''bgd  `7MM'''Mq.        db      `YMM'   `MM'`7MM'''Yb.   `7MM'''Yp, \n");
    printf(".dP'     `M    MM   `MM.      ;MM:       VMA   ,V    MM    `Yb.   MM    Yb\n");
    printf("dM'       `    MM   ,M9      ,V^MM.       VMA ,V     MM     `Mb   MM    dP\n");
    printf("MM             MMmmdM9      ,M  `MM        VMMP      MM      MM   MM'''bg.\n");
    printf("MM.    `7MMF'  MM  YM.      AbmmmqMA        MM       MM     ,MP   MM    `Y\n");
    printf("`Mb.     MM    MM   `Mb.   A'     VML       MM       MM    ,dP'   MM    ,9\n");
    printf("  `'bmmmdPY  .JMML. .JMM..AMA.   .AMMA.   .JMML.   .JMMmmmdP'   .JMMmmmd9  \n");
    printf("                       GrayDB - By Miguel Julio\n");
    printf("-----------------------------------------------------------------------------\n");
}

void cli_help() {
    printf("\nCommands:\n");
    printf("  create table ...        -> create new table\n");
    printf("  insert into ...         -> insert register\n");
    printf("  select * from ...       -> search register\n");
    printf("  delete from ...         -> delete register\n");
    printf("  help                    -> show help\n");
    printf("  clear                   -> clear terminal\n");
    printf("  exit                    -> exit program\n\n");
}

void cli_clear() {
    printf("\033[H\033[J");  // limpa terminal
}

void cli_start() {
    cli_logo();
    cli_help();
    dsl_repl();
}