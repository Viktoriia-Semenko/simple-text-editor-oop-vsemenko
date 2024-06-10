#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

class UndoRedoBuffer
{
private:
    char*** three_states;
    int row_number;
    int buffer_size;
    int index;
    int count_states;
    int undo_buffer_size = 3;

public:
    UndoRedoBuffer(int rows, int buffer)
    : buffer_size(buffer), row_number(rows), index(0), count_states(0) // ініціалізація обʼєктів класу
    {
        three_states = (char***)malloc(undo_buffer_size * sizeof(char**));
        if (three_states == nullptr){
            cerr << "Memory allocation failed" << endl;
            exit(EXIT_FAILURE);
        }

        // виділення памʼяті для трьох команд
        for (int i = 0; i < undo_buffer_size; i++) {
            three_states[i] = (char**)malloc(row_number * sizeof(char*));
            if (three_states[i] == nullptr){
                cerr << "Memory allocation failed" << endl;
                exit(EXIT_FAILURE);
            }
            for (int j = 0; j < row_number; j++) {
                three_states[i][j] = (char*)malloc(buffer_size * sizeof(char));
                if (three_states[i][j] == nullptr){
                    cerr << "Memory allocation failed" << endl;
                    exit(EXIT_FAILURE);
                }
                three_states[i][j][0] = '\0';
            }

        }
    }

    ~UndoRedoBuffer() {
        for (int i = 0; i < undo_buffer_size; i++) {
            for (int j = 0; j < row_number; j++) {
                free(three_states[i][j]);
                three_states[i][j] = nullptr;
            }
            free(three_states[i]);
            three_states[i] = nullptr;
        }
        free(three_states);
        three_states = nullptr;
    }

    void save_state(char** text) { // зберігає поточний стан тексту
        if (three_states == nullptr){
            return;
        }
        for (int i = 0; i < row_number; ++i) { // проходження по всіх рядках тексту
            strncpy(three_states[index][i], text[i], buffer_size - 1);
            three_states[index][i][buffer_size - 1] = '\0';
        }
        index = (index + 1) % 3; // оновлення індексу, щоб вказати потім на наступний буфер (їх три)
        if (count_states < 3) {
            count_states++;
        }
    }

    bool load_state(char** text) { // вивантаження стану
        if (count_states == 0) { // якщо немає станів для відміни
            cout << "No undo could be done." << endl;
            return false;
        }
        index = (index + 2) % 3; // переміщення індексу назад до останнього збереженого стану
        for (int i = 0; i < row_number; ++i) {
            strncpy(text[i], three_states[index][i], buffer_size - 1);
            text[i][buffer_size - 1] = '\0';
        }
        count_states--;

        return true;
    }

};


class Text
{
private:
    char** text;
    char* clipboard; // буфер обміну
    int row_number;
    int buffer_size;
    int line_count;
    UndoRedoBuffer undo_redo_buffer;

    static bool file_exists(const char *filename){
        FILE* file_pointer = fopen(filename, "r");
        bool is_exists = false;
        if (file_pointer != nullptr){
            is_exists = true;
            fclose(file_pointer);
        }
        return is_exists;
    }
    void save_state() {
        undo_redo_buffer.save_state(text);
    }

public:
    explicit Text (int rows = 10, int buffer = 256)
    : text(nullptr), row_number(rows), buffer_size(buffer), line_count(0), undo_redo_buffer(rows, buffer), clipboard(nullptr)
    {
        text = (char**)malloc(row_number * sizeof(char*));
        if (text == nullptr){
            cerr << "Cannot allocate memory for this input" << endl;
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < row_number; i++){
            text[i] = (char*) malloc(buffer_size * sizeof(char));
            if (text[i] == nullptr){
                cerr << "Cannot allocate memory for this input" << endl;
                exit(EXIT_FAILURE);
            }
            text[i][0] = '\0'; // ініціалізуємо кожен рядок як порожній
        }

        clipboard = (char*)malloc(buffer_size * sizeof(char));
        if (clipboard == nullptr) {
            cerr << "Cannot allocate memory for clipboard" << endl;
            exit(EXIT_FAILURE);
        }
        clipboard[0] = '\0';
    }

    ~Text() {
        for (int i = 0; i < row_number; i++) {
            free(text[i]);
            text[i] = nullptr;
        }
        free(text);
        free(clipboard);
    }

    static void print_help() {
        cout << "This program is the 'Simple Text Editor'\n"
                << "It implements the following commands:\n"
                << "0 - See the commands\n"
                << "1 - Append text symbols to the end\n"
                << "2 - Start the new line\n"
                << "3 - Saving the information\n"
                << "4 - Loading the information\n"
                << "5 - Print the current text to console\n"
                << "6 - Insert the text by line and symbol index\n"
                << "7 - Search for the text\n"
                << "8 - Delete the text by line and index\n"
                << "9 - Undo latest command\n"
                << "11 - Cut text\n"
                << "12 - Paste text\n"
                << "13 - Copy text\n"
                << "14 - Insert text with replacement"
    }

    void append_text_to_end(){
        save_state();
        char* buffer = nullptr; // цей вказівник ще не використовується
        size_t local_buffer_size = 0;
        ssize_t input_length; // довжина рядка що зчитали

        cout << "Enter a text to append: ";
        cin.ignore();
        // динамічно виділяємо памʼять та зберігаємо текст в буфері
        input_length = getline(&buffer, &local_buffer_size, stdin);

        if (input_length == -1){
            cerr << "Error while reading input.\n" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0'; // видалення нового рядка

        // якщо немає ніякого текст, то починаємо зпочатку
        if (line_count == 0){
            strncpy(text[line_count], buffer, local_buffer_size); // копіюємо введений текст у перший рядок
            line_count ++;
        } else{
            size_t current_length = strnlen(text[line_count - 1], buffer_size); // довжина останнього рядка (без вставленого тексту)
            size_t new_length = current_length + strnlen(buffer, local_buffer_size) + 1; // довжина того шо було + новий текст + \0

            // якщо нова довжина більша за виділену кількість в буфері
            if (new_length > local_buffer_size) {
                // перевиділяємо памʼять для вказівника
                text[line_count - 1] = (char*) realloc(text[line_count - 1], new_length * sizeof(char));
                if (text[line_count - 1] == nullptr) {
                    cerr << "Memory reallocation failed\n" << endl;
                    free(buffer);
                    return;
                }
            }
            strncat(text[line_count - 1], buffer, local_buffer_size); // додаємо введений текст до кінця останнього рядка
        }
        free(buffer);
    }

    void start_new_line(){
        save_state();
        if (line_count >= row_number){
            row_number *= 2; // якщо недостатньо рядків, то виділяємо в два рази більше
            text = (char**)realloc(text, row_number * sizeof(char*));
            if (text == nullptr){
               cerr << "Fail to reallocate memory\n" << endl;
                exit(EXIT_FAILURE);
            }
            for (int i = line_count; i < row_number; i++) {
                text[i] = (char*)malloc(buffer_size * sizeof(char)); // виділяємо памʼять для нових рядків
                if (text[i] == nullptr){
                    cerr << "Memory allocating is failed\n" << endl;
                    exit(EXIT_FAILURE);
                }
                text[i][0] = '\0';
            }
        }
        text[line_count][0] = '\0';
        line_count++; // до підрахунку рядків додаємо рядок
        cout << "New line is started.\n" << endl;
    }

    void save_info() {
        char save_name[100];
        cout << "Enter the file name for saving: ";
        cin >> save_name;

        FILE* file;
        if (file_exists(save_name)) {
            cout << "Do you want to overwrite this file (y/n)?: ";
            char response;
            cin.ignore();
            cin.get(response);

            if (response == 'y') {
                file = fopen(save_name, "w");
            } else if (response == 'n') {
                file = fopen(save_name, "a");
                fseek(file, 0, SEEK_END);
            } else {
                std::cout << "Invalid answer" << std::endl;
                return;
            }

        } else {
            file = fopen(save_name, "w");
        }
        if (file == nullptr) {
            cerr << "Error while opening file" << endl;
            return;
        }

        for (int i = 0; i < line_count; ++i) {
            fprintf(file, "%s\n", text[i]);
        }
        fclose(file);
        cout << "Text has been saved successfully" << endl;
    }

    static void load_info() {
        char load_name[100];
        cout << "Enter the file name for loading: ";
        cin >> load_name;

        FILE* file = fopen(load_name, "r");
        if (file == nullptr) {
            cerr << "Error opening file." << endl;
            return;
        }

        char my_string[256];
        while (fgets(my_string, sizeof(my_string), file) != nullptr) {
            cout << my_string;
        }
        fclose(file);
        cout << "Text has been loaded successfully" << endl;
    }

    void print_text() const{

        if (text == nullptr) {
            cout << "Text is empty" << endl;
            return;
        }
        for (int i = 0; i < line_count; ++i) {
            if (text[i] != nullptr){
                cout << text[i] << endl;
            }
        }
    }

    void insert_text_by_line() {
        save_state();
        int line, index;
        char* buffer = nullptr;
        size_t local_buffer_size = 0;
        ssize_t input_length;

        cout << "Choose line and index: ";
        cin >> line >> index;
        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        cout << "Enter text to insert: ";
        cin.ignore();
        input_length = getline(&buffer, &local_buffer_size, stdin);
        if (input_length == -1) {
            cerr << "Error reading input" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0';

        size_t current_length = strnlen(text[line], buffer_size);
        size_t local_buffer_length = strnlen(buffer, local_buffer_size);
        size_t new_length = current_length + local_buffer_length + 1;

        if (new_length > buffer_size) {
            text[line] = (char*)realloc(text[line], new_length * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                return;
            }
        }

        char* temporary_buffer = (char*)malloc((current_length - index + 1) * sizeof(char));
        if (temporary_buffer == nullptr) {
            cerr << "Memory allocation failed" << endl;
            free(buffer);
            return;
        }

        strncpy(temporary_buffer, text[line] + index, current_length - index);
        temporary_buffer[current_length - index] = '\0';
        text[line][index] = '\0';
        strncat(text[line], buffer, local_buffer_length);
        strncat(text[line], temporary_buffer, current_length - index);

        free(temporary_buffer);
        free(buffer);

        cout << "Text has been inserted." << endl;
    }

    void search_text() const {
        char buffer[256];
        cout << "Enter text to search: ";
        cin.ignore();
        fgets(buffer, buffer_size, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        bool is_found = false;
        for (int i = 0; i < line_count; ++i) {
            char* position = text[i];
            while ((position = strstr(position, buffer)) != nullptr) {
                cout << "Text is present in this position: " << i << " " << position - text[i] << endl;
                position += strnlen(buffer, buffer_size);
                is_found = true;
            }
        }
        if (!is_found) {
            cout << "There is no match for the given text." << endl;
        }
    }

    void delete_text(){
        save_state();
        // input
        int line, index, num_of_symbols;
        cout << "Choose line, index and number of symbols: ";
        cin >> line >> index >> num_of_symbols;

        // validation for the input
        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }
        else if (index + num_of_symbols > strnlen(text[line], buffer_size)) {
            cout << "The number of symbols you want to deleted is bigger than number of symbols in buffer" << endl;
            return;
        }

        size_t current_length = strnlen(text[line], buffer_size); // довжина тексту в буфері зараз

        char* temporary_buffer = (char*)malloc((current_length) * sizeof(char));
        if (temporary_buffer == nullptr){
            cerr << "Memory allocation failed" << endl;
            return;
        }
        strncpy(temporary_buffer, text[line], index); // текст який був до індексу, після якого видаляти
        temporary_buffer[index] = '\0'; // нульове закінчення
        strncat(temporary_buffer, text[line] + index + num_of_symbols, strnlen(text[line], buffer_size) - index - num_of_symbols);
        // додаємо текст який залишився після видаленого

        strncpy(text[line], temporary_buffer, buffer_size - 1);
        text[line][buffer_size - 1] = '\0';

        free(temporary_buffer);
        cout << "Text has been deleted successfully." << endl;

    }

    void undo_command() {
        if (!undo_redo_buffer.load_state(text)) { // якщо буде false
            cout << "Undo failed. No previous index is available." << endl;
        } else {
            cout << "Undo successful." << endl;
        }
    }

    void cut_text() {
        save_state();
        int line, index, num_of_symbols;
        cout << "Choose line, index and number of symbols: ";
        cin >> line >> index >> num_of_symbols;

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }
        if (index + num_of_symbols > strnlen(text[line], buffer_size)) {
            cout << "The number of symbols you want to cut is bigger than the number of symbols in buffer" << endl;
            return;
        }

        strncpy(clipboard, text[line] + index, num_of_symbols); // копіємо обраний текст в буфер обміну
        clipboard[num_of_symbols] = '\0'; // зануляємо текст що додали в буфер

        // те саме робимо що і в delete
        size_t current_length = strnlen(text[line], buffer_size); // довжина тексту в буфері зараз

        char* temporary_buffer = (char*)malloc((current_length) * sizeof(char));
        if (temporary_buffer == nullptr){
            cerr << "Memory allocation failed" << endl;
            return;
        }
        strncpy(temporary_buffer, text[line], index); // текст який був до індексу, після якого видаляти
        temporary_buffer[index] = '\0'; // нульове закінчення
        strncat(temporary_buffer, text[line] + index + num_of_symbols, strnlen(text[line], buffer_size) - index - num_of_symbols);
        // додаємо текст який залишився після видаленого

        strncpy(text[line], temporary_buffer, buffer_size - 1);
        text[line][buffer_size - 1] = '\0';

        free(temporary_buffer);
        cout << "Text has been deleted successfully." << endl;
    }

    void copy_text() {
        int line, index, num_of_symbols;
        cout << "Choose line, index and number of symbols: ";
        cin >> line >> index >> num_of_symbols;

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        strncpy(clipboard, text[line] + index, num_of_symbols);
        clipboard[num_of_symbols] = '\0';
        cout << "Text has been copied to clipboard." << endl;
    }

    void paste_text() {
        save_state();
        int line, index;
        cout << "Choose line and index: ";
        cin >> line >> index;

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        size_t clipboard_length = strnlen(clipboard, buffer_size); // довжина тексту в буфері обміну
        size_t current_length = strnlen(text[line], buffer_size); // довжина тексту в буфері
        size_t new_length = current_length + clipboard_length + 1; // нова довжина + \0

        if (new_length > buffer_size) { // виділяємо памʼять
            text[line] = (char*)realloc(text[line], new_length * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                return;
            }
        }

        char* temporary_buffer = (char*)malloc((current_length - index + 1) * sizeof(char));
        // зберігання тексту що йде після вставки
        if (temporary_buffer == nullptr) {
            cerr << "Memory allocation failed" << endl;
            return;
        }

        strncpy(temporary_buffer, text[line] + index, current_length - index); // текст що йде після вставленого
        temporary_buffer[current_length - index] = '\0';
        text[line][index] = '\0';
        strncat(text[line], clipboard, clipboard_length); // додавання тексту з буфера обміну до рядка
        strncat(text[line], temporary_buffer, current_length - index); // додавання тексту що йде після вставленого

        free(temporary_buffer);
        cout << "Text has been pasted from clipboard." << endl;
    }

    void insert_with_replacement() {
        save_state();
        char* buffer = nullptr; // зберігання інпут тексту
        int line, index;
        size_t local_buffer_size = 0;
        cout << "Choose line and index: ";
        cin >> line >> index;
        cin.ignore();

        if (line >= line_count || index > strnlen(text[line], buffer_size)) {
            cout << "You entered an invalid line or index." << endl;
            return;
        }

        cout << "Write text: ";
        ssize_t input_length = getline(&buffer, &local_buffer_size, stdin);

        if (input_length == -1) {
            cerr << "Error while reading input.\n" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0';

        size_t new_length = index + strnlen(buffer, local_buffer_size); // довжина вставленого тексту після вставки
        size_t current_length = strnlen(text[line], buffer_size); // початкова довжина тексту, який в буфері без змін

        if (new_length >= buffer_size) {
            text[line] = (char*)realloc(text[line], (new_length + 1) * sizeof(char));
            if (text[line] == nullptr) {
                cerr << "Memory reallocation failed." << endl;
                free(buffer);
                return;
            }
            buffer_size = new_length + 1;
        }

        size_t replace_length = current_length - index; // обчислення довжини тексту, який треба замінити
        if (replace_length > strnlen(buffer, local_buffer_size)) // чи довжина заміни більша за довжину нового тексту
            replace_length = strnlen(buffer, local_buffer_size); // тоді довжина заміна - довжина тексту інпут

        strncpy(text[line] + index, buffer, replace_length); // копіювання в рядок починаючи з індекса вставки
        if (replace_length < strnlen(buffer, local_buffer_size))
            strncat(text[line], buffer + replace_length, strnlen(buffer + replace_length, local_buffer_size));
        // додавання тексту що залишився після вставки нового

        free(buffer);

        cout << "Text has been inserted with replacement." << endl;
    }
};

enum Commands {
    COMMAND_HELP = 0,
    COMMAND_APPEND = 1,
    COMMAND_NEW_LINE = 2,
    COMMAND_SAVE = 3,
    COMMAND_LOAD = 4,
    COMMAND_PRINT = 5,
    COMMAND_INSERT_LI = 6,
    COMMAND_SEARCH = 7,
    COMMAND_DELETE = 8,
    COMMAND_UNDO = 9,
    COMMAND_CUT = 11,
    COMMAND_COPY = 12,
    COMMAND_PASTE = 13,
    COMMAND_INSERT_REPLACE = 14

};

int main() {
    Text text;
    int user_command;

    text.print_help();
    while (true) {
        char continue_input[2];
        cout << "Enter the command: ";
        if (scanf("%d", &user_command) != 1) {
            cout << "Invalid input. Please, enter the number given in the help-menu.\n" << endl;
            int a;
            while ((a = getchar()) != '\n') {}
            continue;
        }
        switch (user_command) {
            case COMMAND_HELP:
                text.print_help();
                break;
            case COMMAND_APPEND:
                text.append_text_to_end();
                break;
            case COMMAND_NEW_LINE:
                text.start_new_line();
                break;
            case COMMAND_SAVE:
                text.save_info();
                break;
            case COMMAND_LOAD:
                text.load_info();
                break;
            case COMMAND_PRINT:
                text.print_text();
                break;
            case COMMAND_INSERT_LI:
                text.insert_text_by_line();
                break;
            case COMMAND_SEARCH:
                text.search_text();
                break;
            case COMMAND_DELETE:
                text.delete_text();
                break;
            case COMMAND_UNDO:
                text.undo_command();
                break;
            case COMMAND_CUT:
                text.cut_text();
                break;
            case COMMAND_COPY:
                text.copy_text();
                break;
            case COMMAND_PASTE:
                text.paste_text();
                break;
            case COMMAND_INSERT_REPLACE:
                text.insert_with_replacement();
                break;
            default:
                printf("This command is not implemented\n");
        }
        cout << "Do you want to continue?:(y/n) ";
        cin >> continue_input;
        if (strcmp(continue_input, "y") == 0) {
            continue;
        } else if (strcmp(continue_input, "n") == 0) {
            break;
        } else cout << "Enter a valid answer\n" << endl;
    }
    return 0;
}
