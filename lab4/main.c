#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define DATABASE "database.db"

GtkWidget *window;
GtkWidget *view;
sqlite3 *db;

void execute_sql(const char *sql, int (*callback)(void*,int,char**,char**), void *data) {
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, callback, data, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

static int display_callback(void *widget, int argc, char **argv, char **azColName) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    for (int i = 0; i < argc; i++) {
        gtk_text_buffer_insert(buffer, &end, azColName[i], -1);
        gtk_text_buffer_insert(buffer, &end, " = ", -1);
        gtk_text_buffer_insert(buffer, &end, argv[i] ? argv[i] : "NULL", -1);
        gtk_text_buffer_insert(buffer, &end, "\n", -1);
    }
    gtk_text_buffer_insert(buffer, &end, "\n", -1);

    return 0;
}

void display_data(GtkWidget *widget, gpointer data) {
    const char *sql = (const char*)data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_set_text(buffer, "", -1);
    execute_sql(sql, display_callback, view);
}

void insert_data(GtkWidget *widget, gpointer data) {
    const char *sql = (const char*)data;
    execute_sql(sql, 0, 0);
    display_data(widget, "SELECT * FROM Преподаватели;");
}

void add_data(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog, *content_area;
    GtkWidget *fio_entry, *dob_entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Add Data",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *fio_label = gtk_label_new("ФИО:");
    gtk_grid_attach(GTK_GRID(grid), fio_label, 0, 0, 1, 1);

    fio_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), fio_entry, 1, 0, 1, 1);

    GtkWidget *dob_label = gtk_label_new("Дата рождения (YYYY-MM-DD):");
    gtk_grid_attach(GTK_GRID(grid), dob_label, 0, 1, 1, 1);

    dob_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), dob_entry, 1, 1, 1, 1);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *fio = gtk_entry_get_text(GTK_ENTRY(fio_entry));
        const char *dob = gtk_entry_get_text(GTK_ENTRY(dob_entry));

        char sql[256];
        snprintf(sql, sizeof(sql), "INSERT INTO Преподаватели (ФИО, Дата_рождения) VALUES ('%s', '%s');", fio, dob);
        insert_data(widget, sql);
    }

    gtk_widget_destroy(dialog);
}

void delete_data(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog, *content_area;
    GtkWidget *id_entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Delete Data",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *id_label = gtk_label_new("ID записи для удаления:");
    gtk_grid_attach(GTK_GRID(grid), id_label, 0, 0, 1, 1);

    id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *id_str = gtk_entry_get_text(GTK_ENTRY(id_entry));
        int id = atoi(id_str);

        char sql[256];
        snprintf(sql, sizeof(sql), "DELETE FROM Преподаватели WHERE id=%d;", id);
        execute_sql(sql, 0, 0);
        display_data(widget, "SELECT * FROM Преподаватели;");
    }

    gtk_widget_destroy(dialog);
}

void delete_position(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog, *content_area;
    GtkWidget *id_entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Delete Position",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *id_label = gtk_label_new("ID должности для удаления:");
    gtk_grid_attach(GTK_GRID(grid), id_label, 0, 0, 1, 1);

    id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 0, 1, 1);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *id_str = gtk_entry_get_text(GTK_ENTRY(id_entry));
        int id = atoi(id_str);

        char sql[256];
        snprintf(sql, sizeof(sql), "DELETE FROM Должности WHERE id=%d;", id);
        execute_sql(sql, 0, 0);
        display_data(widget, "SELECT * FROM Должности;");
    }

    gtk_widget_destroy(dialog);
}

void add_position(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog, *content_area;
    GtkWidget *position_entry, *teacher_combo;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Add Position",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *teacher_label = gtk_label_new("Преподаватель ID:");
    gtk_grid_attach(GTK_GRID(grid), teacher_label, 0, 0, 1, 1);

    teacher_combo = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), teacher_combo, 1, 0, 1, 1);

    GtkWidget *position_label = gtk_label_new("Наименование:");
    gtk_grid_attach(GTK_GRID(grid), position_label, 0, 1, 1, 1);

    position_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), position_entry, 1, 1, 1, 1);

    const char *sql = "SELECT id FROM Преподаватели;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            char id_str[10];
            snprintf(id_str, sizeof(id_str), "%d", id);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(teacher_combo), id_str);
        }
        sqlite3_finalize(stmt);
    }

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *selected_id = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(teacher_combo));
        const char *position = gtk_entry_get_text(GTK_ENTRY(position_entry));

        char sql[256];
        snprintf(sql, sizeof(sql), "INSERT INTO Должности (Преподаватель_id, Наименование) VALUES (%s, '%s');", selected_id, position);
        execute_sql(sql, 0, 0);
        display_data(widget, "SELECT * FROM Должности;");
    }

    gtk_widget_destroy(dialog);
}

void create_tables() {
    const char *sql_create_teachers = "CREATE TABLE IF NOT EXISTS Преподаватели (id INTEGER PRIMARY KEY AUTOINCREMENT, ФИО TEXT, Дата_рождения TEXT);";
    execute_sql(sql_create_teachers, 0, 0);
    const char *sql_create_positions = "CREATE TABLE IF NOT EXISTS Должности (id INTEGER PRIMARY KEY AUTOINCREMENT, Преподаватель_id INTEGER, Наименование TEXT, FOREIGN KEY (Преподаватель_id) REFERENCES Преподаватели(id) ON DELETE CASCADE);";
    execute_sql(sql_create_positions, 0, 0);

    const char *sql_create_trigger =
        "CREATE TRIGGER IF NOT EXISTS delete_teacher_position "
        "AFTER DELETE ON Преподаватели "
        "FOR EACH ROW "
        "BEGIN "
        "DELETE FROM Должности WHERE Преподаватель_id = OLD.id; "
        "END;";
    execute_sql(sql_create_trigger, 0, 0);
}

void activate(GtkApplication *app, gpointer user_data) {
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SQLite GTK+ Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *display_button = gtk_button_new_with_label("Display Data");
    g_signal_connect(display_button, "clicked", G_CALLBACK(display_data), "SELECT * FROM Преподаватели;");
    gtk_grid_attach(GTK_GRID(grid), display_button, 0, 0, 1, 1);

    GtkWidget *add_button = gtk_button_new_with_label("Add Data");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_button, 1, 0, 1, 1);

    GtkWidget *delete_button = gtk_button_new_with_label("Удалить");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 2, 0, 1, 1);

    GtkWidget *add_position_button = gtk_button_new_with_label("Add Position");
    g_signal_connect(add_position_button, "clicked", G_CALLBACK(add_position), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_position_button, 3, 0, 1, 1);

    GtkWidget *delete_position_button = gtk_button_new_with_label("Delete Position");
    g_signal_connect(delete_position_button, "clicked", G_CALLBACK(delete_position), NULL);
    gtk_grid_attach(GTK_GRID(grid), delete_position_button, 4, 0, 1, 1);

    GtkWidget *display_positions_button = gtk_button_new_with_label("Display Positions");
    g_signal_connect(display_positions_button, "clicked", G_CALLBACK(display_data), "SELECT * FROM Должности;");
    gtk_grid_attach(GTK_GRID(grid), display_positions_button, 5, 0, 1, 1);

    view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 780, 550);
    gtk_container_add(GTK_CONTAINER(scrolled_window), view);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 1, 6, 1);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    int status;
    GtkApplication *app;

    if (sqlite3_open(DATABASE, &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    create_tables();

    app = gtk_application_new("com.example.SQLiteGTK", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    sqlite3_close(db);

    return status;
}
