
//
//  main.cpp
//  bmpApplication
//
//  Created by Peter on 13/10/2017.
//
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>
#include "windows.h"

int N, M, color;

using namespace std;

void fillheader(char header[]) {
    BITMAPFILEHEADER bmp_fileheader;
    memset(&bmp_fileheader, 0, sizeof(bmp_fileheader));
    
    bmp_fileheader.bfType = 0x4d42;
    bmp_fileheader.bfSize = color ? 54 + M * ceil(3 * N / 4.0) * 4 : 54 + 1024;
    bmp_fileheader.bfReserved1 = 0;
    bmp_fileheader.bfReserved2 = 0;
    bmp_fileheader.bfOffBits = color ? 54 : 54 + 256 * 4;
    memcpy(header, &bmp_fileheader, 14);
    
    BITMAPINFOHEADER bmp_infoheader;
    memset(&bmp_infoheader, 0, 40);
    bmp_infoheader.biSize = 40;
    bmp_infoheader.biWidth = N;
    bmp_infoheader.biHeight = M;
    bmp_infoheader.biPlanes = 1;
    bmp_infoheader.biBitCount = color ? 24 : 8;
    memcpy(header + 14, &bmp_infoheader, 40);
}

void fillpalette(char palette[]) {
    int i, j = 0;
    
    if (color == 2) {
        return;
    } else {
        for (i = 0; i < 1024; i += 4) {
            palette[i] = j;
            palette[i + 1] = j;
            palette[i + 2] = j;
            palette[i + 3] = 0;
            j++;
        }
    }
}

void filldata(char data[], int **r, int **g, int **b) {
    int i, j;
    int lineSize;
    
    if (color == 2) {
        lineSize = ceil(3 * N / 4.0) * 4;
        
        for (i = M - 1; i >= 0; i--) {
            for (j = 0; j < N; j++) {
                data[(M - 1 - i) * lineSize + j * 3] = b[i][j];
                data[(M - 1 - i) * lineSize + j * 3 + 1] = g[i][j];
                data[(M - 1 - i) * lineSize + j * 3 + 2] = r[i][j];
            }
        }
    }
    else {
        lineSize = ceil(N / 4.0) * 4;
        
        for (i = M - 1; i >= 0; i--) {
            for (j = 0; j < N; j++) {
                data[(M - 1 - i) * lineSize + (j)] = b[i][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int i, j, **r=0, **g=0, **b=0;
    
    ifstream f;
    
    char const *filename;
    
    if (argc > 1) {
        filename = argv[1];
    } else {
        int switchIndex = 0;
        cout << "colored[0] or mono[1]?: ";
        cin >> switchIndex;
        switch (switchIndex) {
            case 0:
                filename = "colored.txt";
                break;
            case 1:
                filename = "mono.txt";
                break;
            default:
                cout << "None of the above";
                break;
        }
    }
    
    f.open(filename);
    
    if (f.fail()) {
        cerr << "Not Found\n";
        return -1;
    }
    
    f >> N >> M >> color;
    
    b = new int*[M];
    for (i = 0; i < M; i++) {
        b[i] = new int[N];
    }
    
    if (color == 2) {
        g = new int*[M];
        for (i = 0; i < M; i++) {
            g[i] = new int[N];
        }
        
        r = new int*[M];
        for (i = 0; i < M; i++) {
            r[i] = new int[N];
        }
    }
    
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            if (color) {
                f >> r[i][j] >> g[i][j] >> b[i][j];
            }
            else {
                f >> b[i][j];
            }
        }
    }
    
    f.close();
    
    char *bmpfilename = new char[strlen(filename) + 4];
    
    strcpy(bmpfilename, filename);
    strcat(bmpfilename, ".bmp");
    FILE* bmpfile = fopen(bmpfilename, "wb");
    
    char header[54];
    char palette[4 * 256];
    
    int datasize = M * ceil(3 * N / 4.0) * 4;
    char* data = new char[datasize];
    
    fillheader(header);
    fillpalette(palette);
    filldata(data, r, g, b);

    fwrite(header, 1, 54, bmpfile);
    
    if (!color) {
        fwrite(palette, 1, 4 * 256, bmpfile);
    }
    
    //printf("Current dir: \n");
    
    fwrite(data, 1, datasize, bmpfile);
    fclose(bmpfile);
    
    return 0;
}
