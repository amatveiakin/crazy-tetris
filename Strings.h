#ifndef CRAZYTETRIS_STRINGS_H
#define CRAZYTETRIS_STRINGS_H

#include <string>

//=============================== Recource files ===============================

const std::string  RECOURCE_FOLDER = "Resources\\";
const std::string  PIECE_TYPES_FILE = RECOURCE_FOLDER + "Pieces";
const std::string  SETTINGS_FILE = "Settings";



//=============================== Error messages ===============================

const std::string  ERR_INTERNAL_ERROR = "Внутренная ошибка: \"%s\".";
const std::string  ERR_FILE_NOT_FOUND = "Файл \"%s\" не найден.";
const std::string  ERR_FILE_CORRUPTED = "Файл \"%s\" повреждён.";
const std::string  ERR_FILE_READ_ERROR = "Не удаётся прочитать файл \"%s\".";
const std::string  ERR_FILE_WRITE_ERROR = "Не удаётся записать файл \"%s\".";

#endif
