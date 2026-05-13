#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_TITLE          "# "
#define WORD_CHAPTER        "## "
#define WORD_INDENT_1       "> "
#define WORD_INDENT_2       "> > "
#define WORD_INDENT_3       "> > > "
#define WORD_INDENT_4       "> > > > "
#define WORD_QUESTION       "* "
#define WORD_OPTION_1       "1. "
#define WORD_OPTION_2       "2. "
#define WORD_OPTION_3       "3. "
#define WORD_OPTION_4       "4. "
#define WORD_NEWLINE        "---"
#define WORD_PLACEHOLDER    "[placeholder]"

#define STR_SIZE            (1 << 12)
#define str_equal(s1, s2)   (strcmp(s1, s2) == 0)
#define str_equal_n(s1, s2) (strncmp(s1, s2, strlen(s2)) == 0)
#define next_letter(c)      (c == 'Z' ? 'Z' : c + 1)
#define error_exit(...)                     \
  do {                                      \
    fprintf(stderr, "error: " __VA_ARGS__); \
    fprintf(stderr, "\n");                  \
    exit(1);                                \
  } while (0)

typedef char string[STR_SIZE];

// 去掉 fgets 读入字符串末尾的换行符
char* fgets_no_endl(char* str, int size, FILE* stream) {
  char* ret = fgets(str, size, stream);
  if (ret == NULL) return NULL;
  size_t len = strlen(str);
  if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
  return ret;
}

// 获取关键字之后的具体内容
char* get_content(char* str) {
  char* ret = strchr(str, ' ');
  if (ret == NULL) return str + strlen(str);
  return ret + 1;
}

int main(int argc, char** argv) {
  if (argc == 1) {
    printf("mmp - Markup Math Paper\n");
    return 0;
  }

  // 输入文件
  char* input_path = argv[1];
  printf("input path: %s\n", input_path);

  FILE* input = fopen(input_path, "r");
  if (input == NULL) error_exit("invalid input path: %s", input_path);

  // 模板文件
  string template_path = {0};
  strcpy(template_path, argv[0]);
  if (strrchr(template_path, '\\') == NULL) {  // 程序无完整路径
    strcpy(template_path, "template.html");
  } else {                                                    // 程序有完整路径
    strcpy(strrchr(template_path, '\\'), "\\template.html");  // 程序目录下的 template.html
  }

  printf("template path: %s\n", template_path);
  FILE* template = fopen(template_path, "r");
  if (template == NULL) error_exit("invalid template path: %s", template_path);

  // 输出文件
  string output_path = {0};
  strcpy(output_path, argv[1]);
  if (strrchr(output_path, '.') == NULL) {       // 无扩展名
    strcat(output_path, ".html");                // 直接在末尾补上 ".html"
  } else {                                       // 有扩展名
    strcpy(strrchr(output_path, '.'), ".html");  // 替换扩展名为 ".html"
  }

  printf("output path: %s\n", output_path);
  FILE* output = fopen(output_path, "w");

  string line = {0}, end = {0};
  for (; fgets(line, STR_SIZE, template) != NULL; fputs(line, output)) {
    char* find = strstr(line, WORD_PLACEHOLDER);
    if (find != NULL) {
      strcpy(end, find + strlen(WORD_PLACEHOLDER));  // 保存 WORD_PLACEHOLDER 之后的内容
      while (*find) *(find++) = '\0';                // 清空 WORD_PLACEHOLDER 及之后的内容
      fputs(line, output);
      break;
    }
  }

  unsigned question_count = 0;
  char option_letter = 'A' - 1;
  while (fgets_no_endl(line, STR_SIZE, input) != NULL) {
    if (strlen(line) == 0) continue;
    char* content = get_content(line);
    if (str_equal_n(line, WORD_TITLE)) {  // 标题
      fprintf(output, "<div class='title'>%s</div>", content);
      fprintf(output, "<div class='subject'>数&emsp;学</div>");
    } else if (str_equal_n(line, WORD_CHAPTER)) {  // 小节标题
      fprintf(output, "<div class='chapter'>%s</div>", content);
    } else if (str_equal_n(line, WORD_QUESTION)) {  // 题干
      question_count++, option_letter = 'A' - 1;
      fprintf(output, "<div class='question'>");
      if (question_count <= 9) fprintf(output, "&ensp;");
      fprintf(output, "%u．", question_count);
      if (question_count >= 15) {
        fprintf(output, "<span style='margin-left: -0.5em'>%s</span>", content);
      } else {
        fprintf(output, "%s", content);
      }
      fprintf(output, "</div>");
    } else if (str_equal_n(line, WORD_OPTION_1)) {  // 选择题选项 1
      option_letter = next_letter(option_letter);
      fprintf(output, "<span class='option-1'>%c．%s</span>", option_letter, content);
    } else if (str_equal_n(line, WORD_OPTION_2)) {  // 选择题选项 2
      option_letter = next_letter(option_letter);
      fprintf(output, "<span class='option-2'>%c．%s</span>", option_letter, content);
    } else if (str_equal_n(line, WORD_OPTION_3)) {  // 选择题选项 3
      option_letter = next_letter(option_letter);
      fprintf(output, "<span class='option-3'>%c．%s</span>", option_letter, content);
    } else if (str_equal_n(line, WORD_OPTION_4)) {  // 选择题选项 4
      option_letter = next_letter(option_letter);
      fprintf(output, "<span class='option-4'>%c．%s</span>", option_letter, content);
    } else if (str_equal_n(line, WORD_INDENT_1)) {  // 1 缩进
      fprintf(output, "<div>&emsp;&emsp;%s</div>", content);
    } else if (str_equal_n(line, WORD_INDENT_2)) {  // 2 缩进
      fprintf(output, "<div>&emsp;&emsp;&emsp;&emsp;%s</div>", content);
    } else if (str_equal_n(line, WORD_INDENT_3)) {  // 3 缩进
      fprintf(output, "<div>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;%s</div>", content);
    } else if (str_equal_n(line, WORD_INDENT_4)) {  // 4 缩进
      fprintf(output, "<div>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;%s</div>", content);
    } else if (str_equal_n(line, WORD_NEWLINE)) {  // 换行
      fprintf(output, "<br />");
    } else {
      fprintf(output, "%s", line);
    }
  }

  fputs(end, output);
  for (; fgets(line, STR_SIZE, template) != NULL; fputs(line, output));

  fclose(template);
  fclose(output);
  fclose(input);

  return 0;
}
