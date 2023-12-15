#include "display.h"

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void display(char* inputFilename) {
    // Initialize GTK
    gtk_init(NULL, NULL);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Viewer");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 300, 200);

    // Connect the destroy signal to the callback function
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Load the PPM image using GdkPixbuf
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(inputFilename, NULL);
    if (!pixbuf) {
        g_printerr("Failed to load image.\n");
        exit(EXIT_FAILURE);
    }

    // Create an image widget and set the PPM image
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    // Add the image widget to the window
    gtk_container_add(GTK_CONTAINER(window), image);

    // Show all the widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Free the GdkPixbuf
    g_object_unref(pixbuf);
}