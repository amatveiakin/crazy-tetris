#ifndef CRAZYTETRIS_STRINGS_H
#define CRAZYTETRIS_STRINGS_H

#include <string>

//=============================== Recource files ===============================

// Hmmm... I hope something will work :-)
#if defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32) ||  defined(WIN32) ||  defined(_WIN64) ||  defined(WIN64)
const std::string  PATH_DELIMITER = "\\";
#else
const std::string  PATH_DELIMITER = "/";
#endif
const std::string  RECOURCE_FOLDER = "Resources" + PATH_DELIMITER;
const std::string  PIECE_TEMPLATES_FILE = RECOURCE_FOLDER + "Pieces";
const std::string  SETTINGS_FILE = "Settings";
const std::string  ACCOUNTS_FILE = "Accounts";



//=============================== Error messages ===============================

const std::string  ERR_FILE_NOT_FOUND = "Файл \"%s\" не найден.";
const std::string  ERR_FILE_CORRUPTED = "Файл \"%s\" повреждён.";
// const std::string  ERR_FILE_READ_ERROR = "Не удаётся прочитать файл \"%s\".";
const std::string  ERR_FILE_WRITE_ERROR = "Не удаётся записать файл \"%s\".";
const std::string  ERR_EMPTY_BLOCK = "Пустая фигура в файле \"" + PIECE_TEMPLATES_FILE + "\".";

#endif
