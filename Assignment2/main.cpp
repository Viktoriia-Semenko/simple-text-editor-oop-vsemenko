#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

class Text
{
private:
    char** text;
    int row_number;
    int buffer_size;
    int line_count;

    bool file_exists(const char *filename){
        FILE* file_pointer = fopen(filename, "r");
        bool is_exists = false;
        if (file_pointer != nullptr){
            is_exists = true;
            fclose(file_pointer);
        }
        return is_exists;
    }
public:
    Text (int rows = 10, int buffer = 256)
    : row_number(rows), buffer_size(buffer), line_count(0)
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
    }

    ~Text() {
        for (int i = 0; i < row_number; i++) {
            free(text[i]);
        }
        free(text);
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
                << "7 - Search for the text\n";
    }

    void append_text_to_end(){
        char* buffer = nullptr; // цей вказівник ще не використовується
        size_t buffer_size = 0;
        ssize_t input_length; // довжина рядка що зчитали

        cout << "Enter a text to append: " << endl;
        cin.ignore();
        // динамічно виділяємо памʼять та зберігаємо текст в буфері
        input_length = getline(&buffer, &buffer_size, stdin);

        if (input_length == -1){
            cerr << "Error while reading input.\n" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0'; // видалення нового рядка

        // якщо немає ніякого текст, то починаємо зпочатку
        if (line_count == 0){
            strncpy(text[line_count], buffer, buffer_size); // копіюємо введений текст у перший рядок
            line_count ++;
        } else{
            size_t current_length = strlen(text[line_count - 1]); // довжина останнього рядка (без вставленого тексту)
            size_t new_length = current_length + strlen(buffer) + 1; // довжина того шо було + новий текст + \0

            // якщо нова довжина більша за виділену кількість в буфері
            if (new_length > buffer_size) {
                // перевиділяємо памʼять для вказівника
                text[line_count - 1] = (char*) realloc(text[line_count - 1], new_length * sizeof(char));
                if (text[line_count - 1] == nullptr) {
                    cerr << "Memory reallocation failed\n" << endl;
                    free(buffer);
                    return;
                }
            }
            strcat(text[line_count - 1], buffer); // додаємо введений текст до кінця останнього рядка
        }
        free(buffer);
    }
    void start_new_line(){
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

    void load_info() {
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

    void print_text() const {
        for (int i = 0; i < line_count; ++i) {
            cout << text[i] << endl;
        }
    }

    void insert_text_by_line() {
        int line, index;
        char* buffer = nullptr;
        size_t buf_size = 0;
        ssize_t input_length;

        cout << "Choose line and index: ";
        cin >> line >> index;
        if (line >= line_count || index > strlen(text[line])) {
            cout << "You entered invalid line or index." << endl;
            return;
        }

        cout << "Enter text to insert: ";
        cin.ignore();
        input_length = getline(&buffer, &buf_size, stdin);
        if (input_length == -1) {
            cerr << "Error reading input" << endl;
            free(buffer);
            return;
        }

        buffer[input_length - 1] = '\0';

        size_t current_length = strlen(text[line]);
        size_t buffer_length = strlen(buffer);
        size_t new_length = current_length + buffer_length + 1;

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

        strcpy(temporary_buffer, text[line] + index);
        text[line][index] = '\0';
        strcat(text[line], buffer);
        strcat(text[line], temporary_buffer);

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
                position += strlen(buffer);
                is_found = true;
            }
        }
        if (!is_found) {
            cout << "There is no match for the given text." << endl;
        }
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
