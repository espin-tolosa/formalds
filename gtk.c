#include <gtk/gtk.h>

static void update_hmi(GtkLabel *label, const char * msg)
{
    gtk_label_set_text(label, msg);

    GMainContext *context = g_main_context_default();

    while (g_main_context_pending(context))
    {
        g_main_context_iteration(context, FALSE);
    }
}

static void main_start (GtkWidget *widget, gpointer data)
{
    GtkLabel *label = GTK_LABEL(data);

    update_hmi(label, "Status:\n1");

    g_usleep(1000000);

    update_hmi(label, "Status:\n1 -> 2");

    g_usleep(1000000);

    update_hmi(label, "Status:\n1 -> 2 -> 3");
}

static void activate (GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *label;

    // Create a window
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Force HMI");
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);

    // Create a box container
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW (window), box);

    // Create a button
    button = gtk_button_new_with_label ("Start");

    // Create a label
    label = gtk_label_new("Status");

    // Connect button click signal to callback function
    g_signal_connect (button, "clicked", G_CALLBACK (main_start), label);

    // Add the label and button to the box
    gtk_box_append(GTK_BOX(box), button);
    gtk_box_append(GTK_BOX(box), label);

    // Show the window
    gtk_window_present (GTK_WINDOW (window));
}

int main ( int argc, char ** argv )
{
    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    int status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
