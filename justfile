make:
    gcc -Wall -lm -o main main.c $(pkg-config --libs --cflags libpng)

deb:
    gcc -g -lm -o main main.c $(pkg-config --libs --cflags libpng)

clean:
    rm main
