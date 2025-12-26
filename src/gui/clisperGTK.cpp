#include "clisperGTK.h"

#include <gtk/gtk.h>

namespace {

void OnActivate(GtkApplication* app, gpointer /*user_data*/) {
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Clisper");
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 240);

    GtkWidget* label = gtk_label_new("Hello from Clisper GTK");
#if GTK_MAJOR_VERSION >= 4
    gtk_window_set_child(GTK_WINDOW(window), label);
    gtk_window_present(GTK_WINDOW(window));
#else
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_widget_show_all(window);
#endif
}

} // namespace

int ClisperGTK::run(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("io.clisper.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(OnActivate), nullptr);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
