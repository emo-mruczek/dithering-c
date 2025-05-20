make:
    gcc -o main main.c $(pkg-config --libs --cflags libpng)

clean:
    rm main
