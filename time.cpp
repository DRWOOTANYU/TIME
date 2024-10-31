#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <ctime>
#include <iostream>
#include <sstream>

const int END_YEAR = 2089;
const int END_MONTH = 3;
const int END_DAY = 22;

void set_desktop_window_type(GtkWidget *window);

gboolean update_time_and_timer(gpointer label) {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    std::ostringstream time_stream;
    time_stream << (ltm->tm_hour < 10 ? "0" : "") << ltm->tm_hour << ":"
                << (ltm->tm_min < 10 ? "0" : "") << ltm->tm_min << ":"
                << (ltm->tm_sec < 10 ? "0" : "") << ltm->tm_sec;

    tm end_tm = {0};
    end_tm.tm_year = END_YEAR - 1900;
    end_tm.tm_mon = END_MONTH - 1;
    end_tm.tm_mday = END_DAY;

    time_t end_time = mktime(&end_tm);
    double seconds_left = difftime(end_time, now);

    if (seconds_left <= 0) {
        gtk_label_set_text(GTK_LABEL(label), "The End");
        return FALSE;
    }

    int days_left = seconds_left / (60 * 60 * 24);
    int hours_left = (seconds_left / (60 * 60)) - (days_left * 24);
    int minutes_left = (seconds_left / 60) - (days_left * 24 * 60) - (hours_left * 60);
    int seconds_only = (int)seconds_left % 60;

    std::ostringstream full_text;
    full_text << "Current Time: " << time_stream.str() << "\n"
              << "Time left: " << days_left << "d "
              << hours_left << "h " << minutes_left << "m "
              << seconds_only << "s";

    gtk_label_set_text(GTK_LABEL(label), full_text.str().c_str());
    return TRUE;
}

gint start_x, start_y;

gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer window) {
    if (event->button == 1) {
        gtk_window_get_position(GTK_WINDOW(window), &start_x, &start_y);
        start_x -= event->x_root;
        start_y -= event->y_root;
    }
    return TRUE;
}

gboolean on_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer window) {
    gtk_window_move(GTK_WINDOW(window), start_x + event->x_root, start_y + event->y_root);
    return TRUE;
}

void set_desktop_window_type(GtkWidget *window) {
    GdkWindow *gdk_window = gtk_widget_get_window(window);
    Display *display = gdk_x11_display_get_xdisplay(gdk_display_get_default());
    Window x11_window = gdk_x11_window_get_xid(gdk_window);

    Atom wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom wm_window_type_desktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom wm_state_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
    Atom wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);

    XChangeProperty(display, x11_window, wm_window_type, XA_ATOM, 32,
                    PropModeReplace, reinterpret_cast<unsigned char *>(&wm_window_type_desktop), 1);

    Atom states[] = { wm_state_sticky, wm_state_above };
    XChangeProperty(display, x11_window, wm_state, XA_ATOM, 32,
                    PropModeReplace, reinterpret_cast<unsigned char *>(states), 2);
}

gboolean on_map_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    set_desktop_window_type(GTK_WIDGET(user_data));
    return FALSE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Desktop Clock and Timer");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 120);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_widget_set_opacity(GTK_WIDGET(window), 0.7);
    GtkWidget *label = gtk_label_new(nullptr);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_container_add(GTK_CONTAINER(window), label);
    g_timeout_add_seconds(1, update_time_and_timer, label);
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press), window);
    g_signal_connect(window, "motion-notify-event", G_CALLBACK(on_motion_notify), window);
    g_signal_connect(window, "map-event", G_CALLBACK(on_map_event), window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}