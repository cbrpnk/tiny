// Include gtk
#include <gtk/gtk.h>

static void activate (GtkApplication* app, gpointer user_data) {
    GtkWidget *window;


    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    gtk_widget_show(window);
    
    GtkWidget *gl_area = gtk_gl_area_new();
    gtk_window_set_child(GTK_WINDOW(window), gl_area);
    gtk_window_fullscreen(window);
}

void _start() {
//int main (int argc, char **argv) {
    asm volatile("sub $8, %rsp\n");
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    
    
    g_application_run (G_APPLICATION (app), 0, NULL);
    asm volatile(".intel_syntax noprefix");
	asm volatile("push 231"); //exit_group
	asm volatile("pop rax");
	// asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
    //g_object_unref (app);
    //return 0;
}
