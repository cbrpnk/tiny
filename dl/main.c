#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <gtk/gtk.h>

int main() {
    void *libgtk = dlopen("libgtk-3.so", RTLD_LAZY|RTLD_GLOBAL);
    if(!libgtk) {
        fprintf(stderr, "Error no libX11\n");
        exit(1);
    }
    
    
    
    dlclose(libgtk);
    return 0;
}
