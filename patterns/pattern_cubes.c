

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"

typedef struct cube_t {
   uint8_t x;
   uint8_t y;
   uint8_t l;
   uint8_t colorRed;
   uint8_t colorGreen;
   uint8_t colorBlue;
} cube_t;

cube_t cube[3] = {
        [0]{.x = 0,
            .y = 0,
            .l = 0,
            .colorRed = 15,
            .colorGreen = 0,
            .colorBlue = 0,
           },
        [1]{.x = 0,
            .y = 2,
            .l = 6,
            .colorRed = 0,
            .colorGreen = 15,
            .colorBlue = 0,
           },
        [2]{.x = 2,
            .y = 0,
            .l = 0,
            .colorRed = 0,
            .colorGreen = 0,
            .colorBlue = 15,
           }};

void render_cubes(void) {
   uint8_t c,x,y,l;
   allLedsColor(0, 0, 0);
   for (c=0;c<3;c++) {
       for (x=cube[c].x;x<cube[c].x+2;x++) {
           for (y=cube[c].y;y<cube[c].y+2;y++) {
               for (l=cube[c].l;l<cube[c].l+2;l++) {
//                   printf("(%d,%d,%d) = (%d,%d,%d)\n",l,x,y,cube[c].colorRed,cube[c].colorGreen,cube[c].colorBlue);
                   setLed(l,x,y, cube[c].colorRed,cube[c].colorGreen,cube[c].colorBlue);
}   }   }   }   }

void make_cubes(void) {

    FILE *ptr = fopen("cubes.pat","wb");
    strncpy(header.name,"Cubes",16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    render_cubes();
    writeFrame(ptr);

    cube[0].l = 1;
    cube[1].l = 5;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 2;
    cube[1].l = 4;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 3;
    cube[1].x = 1;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 4;
    cube[1].x = 2;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 5;
    cube[1].l = 3;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 6;
    cube[1].l = 2;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 5;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 4;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 3;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 2;
    render_cubes();
    writeFrame(ptr);

    cube[0].l = 1;
    render_cubes();
    writeFrame(ptr);

    fclose(ptr);
}
