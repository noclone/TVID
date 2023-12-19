#include "display.h"
#include "format.h"

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <sys/time.h>

#define FRAME_RATE 25

long long current_timestamp_milliseconds() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void display(const char* inputFolder, const char*outputFolder, int num_entries, struct dirent **namelist) {
    // Initialize GTK
    gtk_init(NULL, NULL);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Viewer");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 300, 200);

    // Connect the destroy signal to the callback function
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    GtkWidget *image = gtk_image_new();

    // Add the image widget to the window
    gtk_container_add(GTK_CONTAINER(window), image);

    // Show all widgets in the window
    gtk_widget_show_all(window);

    const char* outputFileName = "tmp.ppm";
    long long timestamp_ms = current_timestamp_milliseconds();
    for (int i = 0; i < num_entries; ++i) {
        if (namelist[i]->d_type == DT_REG) {
            if (current_timestamp_milliseconds() - timestamp_ms < 1000 / FRAME_RATE) {
                g_usleep(1000 * (1000 / FRAME_RATE - (current_timestamp_milliseconds() - timestamp_ms)));
            }
            timestamp_ms = current_timestamp_milliseconds();
            char *inputFilename = malloc(sizeof(char) * 1000);
            sprintf(inputFilename, "%s/%s", inputFolder, namelist[i]->d_name);
            pgmToPpm(inputFilename, outputFileName);
            free(inputFilename);

            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(outputFileName, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
            g_object_unref(pixbuf);

            // Update the window
            gtk_widget_queue_draw(window);
            gtk_main_iteration();
        }
        free(namelist[i]);
    }
}