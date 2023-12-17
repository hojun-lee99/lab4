#include <gtk/gtk.h>
#include <glib.h>

// 계산기 상태를 나타내는 열거형
typedef enum {
    STATE_NONE,     // 초기 상태
    STATE_INPUT,    // 숫자 입력 중
    STATE_OPERATOR, // 연산자 입력 중
    STATE_RESULT    // 결과 표시 중
} CalculatorState;

// 계산기 데이터 구조체
typedef struct {
    GtkWidget *entry;          // 텍스트 입력을 표시하는 엔트리 위젯
    CalculatorState state;     // 계산기의 현재 상태
    double result;             // 계산 결과
    char operator;             // 현재 사용 중인 연산자
    gboolean clear_on_next;    // 다음 입력 시 현재 내용을 지울 지 여부
} CalculatorData;

// 버튼 클릭 시 호출되는 콜백 함수
void on_button_clicked(GtkWidget *widget, gpointer data) {
    CalculatorData *calc_data = (CalculatorData *)data;
    const gchar *label = gtk_button_get_label(GTK_BUTTON(widget));

    if (calc_data->clear_on_next) {
        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "");
        calc_data->clear_on_next = FALSE;
    }

    // 버튼 라벨에 따라 처리
    if (g_ascii_strcasecmp(label, "=") == 0) {
        // "=" 버튼 클릭 시 계산 결과 표시
        double value = atof(gtk_entry_get_text(GTK_ENTRY(calc_data->entry)));
        switch (calc_data->operator) {
            case '+':
                calc_data->result += value;
                break;
            case '-':
                calc_data->result -= value;
                break;
            case '*':
                calc_data->result *= value;
                break;
            case '/':
                if (value != 0.0)
                    calc_data->result /= value;
                else
                    gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "Error");
                break;
            default:
                calc_data->result = value;
                break;
        }
        gchar result_str[32];
        g_snprintf(result_str, sizeof(result_str), "%.6f", calc_data->result);
        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), result_str);
        calc_data->state = STATE_RESULT;
    } else if (g_ascii_strup("+-*/", *label) != NULL)//else if (g_ascii_strchr("+-*/", *label) != NULL) 
    {
        // 연산자 버튼 클릭 시 현재 입력 상태를 저장하고 연산자 설정
        if (calc_data->state == STATE_INPUT) {
            double value = atof(gtk_entry_get_text(GTK_ENTRY(calc_data->entry)));
            switch (calc_data->operator) {
                case '+':
                    calc_data->result += value;
                    break;
                case '-':
                    calc_data->result -= value;
                    break;
                case '*':
                    calc_data->result *= value;
                    break;
                case '/':
                    if (value != 0.0)
                        calc_data->result /= value;
                    else
                        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "Error");
                    break;
                default:
                    calc_data->result = value;
                    break;
            }
        }
        calc_data->operator = *label;
        gchar operator_str[2] = {calc_data->operator, '\0'};
        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), operator_str);
        calc_data->state = STATE_OPERATOR;
    } else if (g_ascii_strcasecmp(label, "C") == 0) {
        // "C" 버튼 클릭 시 계산기 초기화
        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "");
        calc_data->result = 0.0;
        calc_data->operator = '\0';
        calc_data->state = STATE_NONE;
    } else {
        // 숫자 버튼 클릭 시 현재 입력에 추가
        gchar *current_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(calc_data->entry)));
        gchar *new_text = g_strconcat(current_text, label, NULL);
        gtk_entry_set_text(GTK_ENTRY(calc_data->entry), new_text);
        g_free(current_text);
        calc_data->state = STATE_INPUT;
    }
}

int main(int argc, char *argv[]) {
    // GTK 초기화
    gtk_init(&argc, &argv);

    // 윈도우 생성
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 그리드 생성
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 엔트리 생성
    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 버튼 레이아웃
    gchar *button_labels[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+",
        "C"
    };

    CalculatorData calc_data;
    calc_data.entry = entry;
    calc_data.state = STATE_NONE;
    calc_data.result = 0.0;
    calc_data.operator = '\0';
    calc_data.clear_on_next = FALSE;

    // 버튼 생성 및 클릭 시 콜백 함수 연결
    for (int i = 0; i < G_N_ELEMENTS(button_labels); ++i) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), &calc_data);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, 1 + i / 4, 1, 1);
    }

    // 윈도우 및 그리드 보이기
    gtk_widget_show_all(window);

    // GTK 메인 루프 실행
    gtk_main();

    return 0;
}
