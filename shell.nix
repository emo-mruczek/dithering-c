{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
    name = "labiryncior";

    packages = 
         with pkgs; [
            libpng
            gdb
            libgcc
            just
            pkg-config
            valgrind
            imagemagick
         ];
}
