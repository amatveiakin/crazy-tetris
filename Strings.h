#ifndef CRAZYTETRIS_STRINGS_H
#define CRAZYTETRIS_STRINGS_H

#include <string>

//=============================== Recource files ===============================

// TODO: add ..._NAME to file name constants

// Hmmm... I hope something will work :-)
#if defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32) ||  defined(WIN32) ||  defined(_WIN64) ||  defined(WIN64)
const std::wstring  PATH_DELIMITER = L"\\";
#else
const std::wstring  PATH_DELIMITER = L"/";
#endif
const std::wstring  RECOURCE_FOLDER = L"Resources" + PATH_DELIMITER;
const std::wstring  PIECE_TEMPLATES_FILE = RECOURCE_FOLDER + L"Pieces";
const std::wstring  SETTINGS_FILE = L"Settings";
const std::wstring  ACCOUNTS_FILE = L"Accounts";

const std::wstring  TEXTURES_FOLDER = RECOURCE_FOLDER + L"Textures" + PATH_DELIMITER;
const std::wstring  BONUS_IMAGES_FOLDER = TEXTURES_FOLDER + L"Bonuses" + PATH_DELIMITER;



//=============================== Error messages ===============================

const std::wstring  ERR_INTERNAL_ERROR = L"Внутренняя ошибка: \"%s\"";
const std::wstring  ERR_FILE_NOT_FOUND = L"Файл \"%s\" не найден.";
const std::wstring  ERR_FILE_CORRUPTED = L"Файл \"%s\" повреждён.";
// const std::wstring  ERR_FILE_READ_ERROR = L"Не удаётся прочитать файл \"%s\".";
const std::wstring  ERR_FILE_WRITE_ERROR = L"Не удаётся записать файл \"%s\".";
const std::wstring  ERR_EMPTY_BLOCK = L"Пустая фигура в файле \"" + PIECE_TEMPLATES_FILE + L"\".";

#endif
