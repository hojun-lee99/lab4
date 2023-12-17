#include <gtk/gtk.h>
#include <stdio.h>

void quit (GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}
void btn_Clicked (GtkWidget *label, gpointer data)
{
    label += data;
}

int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *label1;
    GtkWidget *hbox1, *hbox2, *hbox3, *hbox4, *hbox5;
    GtkWidget *vbox;
    GtkWidget *btn0, *btn1, *btn2, *btn3, *btn4, *btn5, *btn6, *btn7, *btn8, *btn9,
                *btn_AC, *btn_Enter, *btn_plus, *btn_minus, *btn_multi, *btn_divid, *btn_dot;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Calc");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 450);
    /* 윈도우를 닫을 때 "destory" 시그널이 발생*/
    g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);

    label1 = gtk_label_new("0");
    btn0 = gtk_button_new_with_label ("0");
    btn1 = gtk_button_new_with_label ("1");
    btn2 = gtk_button_new_with_label ("2");
    btn3 = gtk_button_new_with_label ("3");
    btn4 = gtk_button_new_with_label ("4");
    btn5 = gtk_button_new_with_label ("5");
    btn6 = gtk_button_new_with_label ("6");
    btn7 = gtk_button_new_with_label ("7");
    btn8 = gtk_button_new_with_label ("8");
    btn9 = gtk_button_new_with_label ("9");
    btn_AC = gtk_button_new_with_label ("AC");
    btn_Enter = gtk_button_new_with_label ("Enter");
    btn_plus = gtk_button_new_with_label ("+");
    btn_minus = gtk_button_new_with_label ("-");
    btn_multi = gtk_button_new_with_label ("*");
    btn_divid = gtk_button_new_with_label ("/");
    btn_dot = gtk_button_new_with_label (".");
    

    hbox1 = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 5);
    hbox2 = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 5);
    hbox3 = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 5);
    hbox4 = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 5);
    hbox5 = gtk_box_new ( GTK_ORIENTATION_HORIZONTAL, 5);
    vbox = gtk_box_new ( GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(hbox1), btn_AC, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), btn_Enter, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), btn7, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), btn8, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), btn9, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), btn_plus, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), btn4, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), btn5, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), btn6, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), btn_minus, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox4), btn1, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox4), btn2, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox4), btn3, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox4), btn_multi, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox5), btn0, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox5), btn_dot, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox5), btn_divid, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(vbox), label1, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox3, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox4, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox5, TRUE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}