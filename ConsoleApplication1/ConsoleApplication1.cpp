/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				Севастопольский государственный университет
					Кафедра «Информационные системы»

			Программа для работы с базой учебных групп и дисциплин
								Текст программы
																 РАЗРАБОТАЛ
													Студент гр. ИТ/б-22-4-о
														     Пархоменко А.А.
									 2023
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Программа работает с базой данных сотовых телефонов, которая считывается из тек-
стового файла. Каждая строка файла содержит запись о группе (6 символов), диспиплине
(6 символов), количестве оценок (отл., хор., удовл., неуд.), количестве пропущенных
лекций и количестве пропущенных ПЗ.
	Основные функции программы:
		- создание таблицы;
		- вывод таблицы на экран;
		- добавление нового элемента в таблицу;
		- удаление элемента по ключевому полю;
		- корректировка записи;
		- сортировка данных (по возрастанию и убыванию);
		- поиск записи по шифру дисциплины;
		- сохранение списка в файл (текстовый и бинарный);
		- чтение списка из файла (текстового и бинарного);
		- высчитывание среднего балла и общего количества оценок для каждой группы
		- очистка памяти и выход
	Вариант задания 17. Утверждено 09.09.2023
	Среда программирования Visual Studio 2022
	Дата последней коррекции: 17.12.2023 .
	Версия 1.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*--------------Директивы препроцессора----------*/
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <fstream>
/*----------------------------------------*/

/*--------------Пространство имён----------*/
using namespace std; 
/*----------------------------------------*/

/*-------------Значения для скроллинга--------*/
const int RowsOnPage = 5; //Количество строк на экране
int CurrentRowV = 0; //Текущая строка (для просмотра)
int CurrentRowO = 0; //Текущая строка (для обработки)
/*----------------------------------------*/

/*------------Счетчик для уникальных ID------------*/
int id_number = 0;
/*----------------------------------------*/

/*------------Значения для строк----------*/
const int FLen = 51; //Длина имени файла
const int SLen = 7; //Длина строки
const int ELen = 5; //Длина расширения
const int ILen = 5; //Длина int
/*----------------------------------------*/

/*--------------Информационная структура-----------*/
struct student {
	int number; //порядковый номер
	char group_code[SLen]; //шифр группы 
	char discipline_code[SLen]; //шифр дисциплины
	int marks[4]; //массив с количеством оценок (marks[0] - отл., marks[1] - хор., marks[2] - уд., marks[3] - неуд.)
	int L_skips; //количество пропусков лекций
	int PZ_skips; //количество пропусков ПЗ
};
/*----------------------------------------*/

/*--------------Двунаправленный список-----------*/
struct elist {
	student data; //информационное поле
	elist* left; //указатель на элемент слева
	elist* right; //указатель на элемент справа
};
/*----------------------------------------*/

/*-------------Структуры для обработки-----------*/
struct studentO {
	char group_code[SLen]; //шифр группы 
	float aver_mark; //средний балл
	int marks[4]; //массив с количеством оценок (marks[0] - отл., marks[1] - хор., marks[2] - уд., marks[3] - неуд.)
};
struct elistO {
	studentO data; //информационное поле
	elistO* left; //указатель на элемент слева
	elistO* right; //указатель на элемент справа
};
/*----------------------------------------*/

/*--------------Прототипы функций-----------*/
//ДОП ФУНКЦИИ ИЛИ ПРОВЕРКИ
const char* Input_str(string phrase); //ввод строки с проверками
int Input_int(string phrase); //ввод int с проверками
void Input_filename(char f[FLen], string phrase); //ввод имени файла с проверками
void Input_key(elist* begin, int& key, string phrase); //ввод ключа для пунктов 4,5
void Input_flag(int& flag, string phrase); //ввод флага
student Read_data(); //чтение информации (ввод инф. поля)
void PrintMenu(int currentSelection); //Вывод пунтктов главного меню
void Print_el(elist* tmp); //Вывод одного элемента
//ФУНКЦИИ ДЛЯ СОРТИРОВКИ
void split(elist* begin, elist** a, elist** b); //разбиение списка на два
elist* Imerge(elist* a, elist* b); //Слияние двух подсписков в порядке возрастания
elist* Dmerge(elist* a, elist* b); //Слияние двух подсписков в порядке убывания
//ОСНОВНЫЕ ФУНКЦИИ КУРСОВОЙ
elist* Add_to_start(elist* begin, student s); //Добавление в начало
elist* Add_to_end(elist* begin, elist* end, student s); //Добавление в конец
elist* Organize_V(elist* begin, elist** end); //Организация списка
void Print_table(elist* begin); //Вывод таблицы со скроллингом
elist* Del_key_el(elist* begin, elist** end, int num_key); //Удаление по ключу
elist* Modification(elist* begin, int num_key); //Корректировка по ключу
void Imergesort(elist** begin); //Сортировка по возрастанию
void Dmergesort(elist** begin); //Сортировка по убыванию
elist* Search(elist* begin, elist** end, char discipline_for_search[SLen], elist* begin2, elist** end2); //Поиск по шифру дисциплины
void Add_to_bin_file(char f[FLen], elist* begin); //Добавление в бинарный файл
elist* Read_from_bin_file(char f[FLen], elist* begin, elist** end); //Чтение из бинарного файла
void Add_to_txt_file(char f[FLen], elist* begin); //Сохранение в текстовый файл
elist* Read_from_txt_file(char f[FLen], elist* begin, elist** end); //Чтение из текстового файла
void Memory_cleaning(elist* begin); //Очистка памяти
//ФУНКЦИИ ДЛЯ ОБРАБОТКИ
elistO* Add_to_startO(elistO* begin, studentO s); //Добавление в начало списка-обработки
elistO* Add_to_endO(elistO* begin, elistO* end, studentO s); //Добавление в конец списка-обработки
elistO* OrganizeO(elistO* beginO, elistO** endO, elist* begin); //Организация списка-обработки
void Obrabotka_V(elistO* beginO); //Вывод на экран результатов обработки
void Obrabotka_file(char f[FLen], elistO* beginO); //Сохранение таблицы в файл
void Memory_cleaningO(elistO* begin); //Очистка памяти списка-обработки
/*----------------------------------------*/

/*--------------main-----------*/
int main() {
	//SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000); //Для полного экрана

	SetConsoleCP(1251); //Для русского языка
	SetConsoleOutputCP(1251); //Для русского языка

	//Начало, конец списка;
	elist *begin, *end;
	begin = NULL;
	end = NULL;
	//Переменная для выбора действий
	int flag_;
	//Номера элементов для удаления и модификации (пункты 4, 5)
	int num_key_for_del, num_key_for_mod;
	//Название дисциплины (пункты 8)
	char discipline_for_search[SLen];
	//Строки для работы с файлами (пункты 1, 9, 10, 11, 12, 13)
	char txt_filename_org[FLen], txt_filename_read[FLen], txt_filename_write[FLen], bin_filename_read[FLen], bin_filename_write[FLen], txt_filename_obrabotka[FLen];
	char bin_ext[ELen] = ".bin";
	char txt_ext[ELen] = ".txt";
	//Файлы (пункты 1, 9, 10, 11, 12, 13)
	ifstream fin;
	ofstream fout;

	/*--Значения для выбора пункта--*/
	const int numItems = 14; // общее количество пунктов
	int currentSelection = 0; // текущий выбранный пункт

	while (1) {
		PrintMenu(currentSelection);

		char key = _getch();// ждем ввода клавиши без вывода на экран

		switch (key) {
			case 'w': case 'W': case 'ц': case 'Ц':
				currentSelection = (currentSelection - 1 + numItems) % numItems;
				break;
			case 's': case 'S': case 'ы': case 'Ы':
				currentSelection = (currentSelection + 1) % numItems;
				break;
			case 13: { //Enter
				system("cls");
				switch (currentSelection + 1) {
					//--case 1: ОРГАНИЗАЦИЯ СПИСКА (ввод с клавиатуры или файл)--
					case 1: {
						if (begin != NULL) {
							cout << "\033[1;31m";
							puts("Список не пуст!");
							cout << "\033[0m\n";
							Input_flag(flag_, "Хотите очистить память для возможности создать новый список? 0 - нет (выход), 1 - да (память будет очищена): ");
							

							if (flag_ == 0) {
								system("pause");
								system("cls");
								break;
							}

							if (flag_ == 1) {
								Memory_cleaning(begin);
								begin = NULL;
								end = NULL;
								cout << "\033[1;31m";
								puts("Память очищена!\n Можете воспользоваться данным пунктом (1) ещё раз для создания нового списка!");
								cout << "\033[0m\n";
								system("pause");
								system("cls");
								break;
							}
						}
						Input_flag(flag_, "Хотите ввести данные вручную или из текстового файла? 0 - вручную, 1 - текстовый файл : ");

						if (flag_ == 0) {
							begin = Organize_V(begin, &end);
						}

						if (flag_ == 1) {
							cout << "\033[1;31m";
							puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
							cout << "\033[0m\n";

							//-Ввод имени файла и проверка-
							Input_filename(txt_filename_org, "Введите имя текстового файла для организации таблицы: ");

							strcat(txt_filename_org, txt_ext);

							begin = Read_from_txt_file(txt_filename_org, begin, &end);
						}
						system("pause");
						system("cls");
						break;
					}
						  //--case 2: ВЫВОД ТАБЛИЦЫ С ФУНКЦИЕЙ СКРОЛЛИНГА--
					case 2: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						Print_table(begin);
						system("cls");
						break;
					}
					//--case 3: ДОБАВЛЕНИЕ В КОНЕЦ--
					case 3: {
						cout << "\033[1;31m" << "ДОБАВЛЕНИЕ: " << "\033[0m\n";
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список был пуст! Элемент будет первым в списке!");
							cout << "\033[0m\n";
							begin = Add_to_start(begin, Read_data());
							end = begin;
							id_number = 1;
							end->data.number = id_number;
						}
						else {
							end = Add_to_end(begin, end, Read_data());
							end->data.number = ++id_number;
						}
						cout << "\033[1;31m";
						puts("ЭЛЕМЕНТ БЫЛ ДОБАВЛЕН В КОНЕЦ!");
						cout << "\033[0m\n";
						system("pause");
						system("cls");
						break;
					}
					//--case 4: УДАЛЕНИЕ ЭЛЕМЕНТА ПО КЛЮЧУ--
					case 4: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("УДАЛЕНИЕ ЭЛЕМЕНТА БУДЕТ ПРОИЗВОДИТЬСЯ ПО КЛЮЧУ (т.е. ID элементов таблицы)!");
						cout << "\033[0m\n";
						Input_key(begin, num_key_for_del, "Введите порядковый номер элемента таблицы, который хотите удалить: ");
						begin = Del_key_el(begin, &end, num_key_for_del);
						cout << "\033[1;32m";
						puts("Удаление успешно произведено!");
						puts("Для просмотра результатов удаления воспользуйтесь пунктом 2 - ПРОСМОТР ТАБЛИЦЫ!");
						cout << "\033[0m\n";
						system("pause");
						system("cls");
						break;
					}
					//--case 5: КОРРЕКТИРОВКА ЭЛЕМЕНТА--
					case 5: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст или в списке недостаточно элементов!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список или пунктом 3 для добавления!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("МОДИФИКАЦИЯ ЭЛЕМЕНТА БУДЕТ ПРОИЗВОДИТЬСЯ ПО КЛЮЧУ (т.е. ID элементов таблицы)!");
						cout << "\033[0m\n";
						Input_key(begin, num_key_for_mod, "Введите порядковый номер элемента таблицы, который хотите изменить: ");
						begin = Modification(begin, num_key_for_mod);
						system("pause");
						system("cls");
						break;
					}
					//--case 6: СОРТИРОВКА ПО ВОЗРАСТАНИЮ--
					case 6: {
						if (begin == NULL || begin->right == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст или в списке недостаточно элементов!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список или пунктом 3 для добавления!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						Imergesort(&begin);
						cout << "\033[1;31m";
						puts("СОРТИРОВКА ПО ВОЗРАСТАНИЮ ПРОИЗВЕДЕНА!");
						cout << "\033[0m\n";
						cout << "\033[1;32m";
						puts("Для просмотра результатов сортировки воспользуйтесь пунктом 2 - ПРОСМОТР ТАБЛИЦЫ!");
						cout << "\033[0m\n";
						for (end = begin; end->right != NULL; end = end->right);
						system("pause");
						system("cls");
						break;
					}
						  //--case 7: СОРТИРОВКА ПО УБЫВАНИЮ--
					case 7: {
						if (begin == NULL || begin->right == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст или в списке недостаточно элементов!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список или пунктом 3 для добавления!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						Dmergesort(&begin);
						cout << "\033[1;31m";
						puts("СОРТИРОВКА ПО УБЫВАНИЮ ПРОИЗВЕДЕНА!");
						cout << "\033[0m\n";
						cout << "\033[1;32m";
						puts("Для просмотра результатов сортировки воспользуйтесь пунктом 2 - ПРОСМОТР ТАБЛИЦЫ!");
						cout << "\033[0m\n";
						for (end = begin; end->right != NULL; end = end->right);
						system("pause");
						system("cls");
						break;
					}
						  //--case 8: ПОИСК ЭЛЕМЕНТА ПО ПОЛЮ "шифр дисциплины"--
					case 8: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("Поиск элемента будет производиться по полю ШИФР ДИСЦИПЛИНЫ!");
						cout << "\033[0m";
						strcpy(discipline_for_search, Input_str("Введите шифр дисциплины для поиска: "));
						elist* begin2 = NULL; elist* end2 = NULL;
						begin2 = Search(begin, &end, discipline_for_search, begin2, &end2);
						if (begin2 != NULL) {
							Print_table(begin2);
							Memory_cleaning(begin2);
							begin2 = NULL; end2 = NULL;
							system("cls");
							break;
						}
						system("pause");
						system("cls");
						break;
					}
						  //--case 9: СОХРАНЕНИЕ ТАБЛИЦЫ В БИНАРНЫЙ ФАЙЛ--
					case 9: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
						cout << "\033[0m\n";

						//Ввод имени файла и проверка имени файла
						Input_filename(bin_filename_write, "Введите имя бинарного файла для сохранения таблицы: ");

						strcat(bin_filename_write, bin_ext);

						Add_to_bin_file(bin_filename_write, begin);
						system("pause");
						system("cls");
						break;
					}
						  //--case 10: ЧТЕНИЕ ТАБЛИЦЫ ИЗ БИНАРНОГО ФАЙЛА--
					case 10: {
						cout << "\033[1;31m";
						puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
						cout << "\033[0m\n";
						Input_filename(bin_filename_read, "Введите имя бинарного файла для чтения таблицы: ");
						strcat(bin_filename_read, bin_ext);

						//Ввод имени файла и проверка
						begin = Read_from_bin_file(bin_filename_read, begin, &end);
						system("pause");
						system("cls");
						break;
					}
						   //--case 11: СОХРАНЕНИЕ ТАБЛИЦЫ В ТЕКСТОВЫЙ ФАЙЛ--
					case 11: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
						cout << "\033[0m\n";
						//Ввод имени файла и проверка имени файла
						Input_filename(txt_filename_write, "Введите имя текстового файла для сохранения таблицы: ");
						strcat(txt_filename_write, txt_ext);
						Add_to_txt_file(txt_filename_write, begin);
						system("pause");
						system("cls");
						break;
					}
						   //--case 12: ЧТЕНИЕ ТАБЛИЦЫ ИЗ ТЕКСТОВОГО ФАЙЛА--
					case 12: {
						cout << "\033[1;31m";
						puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
						cout << "\033[0m\n";
						Input_filename(txt_filename_read, "Введите имя текстового файла для чтения таблицы: ");
						strcat(txt_filename_read, txt_ext);
						//Создание нового списка
						begin = Read_from_txt_file(txt_filename_read, begin, &end);
						system("pause");
						system("cls");
						break;
					}
						   //--case 13: ОБРАБОТКА ТАБЛИЦЫ (вывод на экран и в файл)--
					case 13: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nСначала воспользуйтесь пунктом 1 для того, чтобы создать список!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							break;
						}
						cout << "\033[1;31m";
						puts("РАСШИРЕНИЕ ФАЙЛА ВВОДИТЬ НЕ НАДО! Иначе будет ошибка!");
						cout << "\033[0m\n";
						//Ввод имени файла и проверка имени файла
						Input_filename(txt_filename_obrabotka, "Введите имя текстового файла для сохранения обработки: ");

						strcat(txt_filename_obrabotka, txt_ext);

						elistO* beginO = NULL; elistO* endO = NULL;
						beginO = OrganizeO(beginO, &endO, begin);
						Obrabotka_file(txt_filename_obrabotka, beginO);
						Obrabotka_V(beginO);
						Memory_cleaningO(beginO);
						beginO = NULL; endO = NULL;
						system("cls");
						break;
					}
						   //--case 14: ВЫХОД ИЗ ПРОГРАММЫ И ОЧИСТКА ПАМЯТИ--
					case 14: {
						if (begin == NULL) {
							cout << "\033[1;31m";
							puts("Список пуст!\nОчищать память не надо!");
							cout << "\033[0m\n";
							system("pause");
							system("cls");
							return 0;
						}
						Memory_cleaning(begin);
						begin = NULL;
						end = NULL;
						cout << "\033[1;31m";
						puts("Память была очищена! Программа завершает работу!");
						cout << "\033[0m\n";
						system("pause");
						system("cls");
						return 0;
					}
				}
				break;
			}
		}
	}
}
/*----------------------------------------*/

/*-------------Проверки--------------*/
//Проверка на ввод строк
const char* Input_str(string phrase) {
	char s[SLen];
	bool flag = false;
	do {
		cout << "\033[1;32m" << "  *Ввод строки(max - 6 символов)*  " << "\033[0m" << phrase;
		cin.getline(s, SLen);
		// Проверка на пустоту строки
		if (s[0] == '\0') {
			cout << "\033[1;31m" << "Строка не должна быть пустой! Попробуйте снова." << "\033[0m" << endl;
			continue;
		}

		// Проверка на переполнение буфера
		if (cin.fail()) {
			cout << "\033[1;31m" << "Произошло переполнение буфера! Попробуйте снова." << "\033[0m" << endl;
			// Сбрасываем флаги потока
			cin.clear();
			//Очистка буфера
			cin.ignore(INT_MAX, '\n');
			continue;
		}
		//Проверка на ввод строки, полностью состоящей из пробелов
		size_t j = 0;
		for (; j < strlen(s) && s[j] == ' '; j++);
		if (j == strlen(s)) {
			cout << "\033[1;31m" << "Строка не должна состоять полностью из пробелов! Попробуйте снова." << "\033[0m" << endl;
			continue;
		}
		flag = true;
		// Проверка на наличие символов от 'А' до 'Я' и пробелов
		for (size_t i = 0; i < strlen(s); ++i) {
			if ((s[i] < 'А' || s[i] > 'Я') && (s[i] < 'а' || s[i] > 'я') && (s[i] < 'A' || s[i] > 'Z') && (s[i] < 'a' || s[i] > 'z') && (s[i] != ' ') && (s[i] < '0' || s[i] > '9')) {
				flag = false;
				cout << "\033[1;31m" << "Строка должна содержать только буквы (русские/английские), цифры, пробелы! Попробуйте снова." << "\033[0m" << endl;
				break;
			}
		}
	} while (!flag);
	return s;
}
//Проверка на ввод чисел
int Input_int(string phrase) {
	int a;
	char intt[ILen];
	bool flag = false;
	do {
		cout << "\033[1;32m" << "  *Ввод int числа (max - 4 символа)*  " << "\033[0m" << phrase;
		cin.getline(intt, ILen);
		// Проверка на пустоту строки
		if (intt[0] == '\0') {
			cout << "\033[1;31m" << "Строка не должна быть пустой. Попробуйте снова." << "\033[0m" << endl;
			continue;
		}
		// Проверка на переполнение буфера
		if (cin.fail()) {
			cout << "\033[1;31m" << "Произошло переполнение буфера!" << "\033[0m" << endl;
			// Очищаем буфер ввода
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			continue;
		}
		flag = true;
		// Проверка на наличие цифр
		for (size_t i = 0; i < strlen(intt); ++i) {
			if (intt[i] < '0' || intt[i] > '9') {
				flag = false;
				cout << "\033[1;31m" << "Вы ввели что-то кроме цифр! Попробуйте снова." << "\033[0m" << endl;
				break;
			}
		}
	} while (!flag);
	a = stoi(intt);
	return a;
}
//Проверка на ввод имени файла
void Input_filename(char f[FLen], string phrase) {
	bool flag = false;
	do {
		cout << "\033[1;32m" << "  *Ввод имени файла(max - 50 символов)*  " << "\033[0m" << phrase;
		cin.getline(f, FLen);
		// Проверка на пустоту имени
		if (f[0] == '\0') {
			cout << "\033[1;31m" << "Имя файла не должно быть пустым! Попробуйте снова." << "\033[0m" << endl;
			continue;
		}

		// Проверка на переполнение буфера
		if (cin.fail()) {
			cout << "\033[1;31m" << "Произошло переполнение буфера! Попробуйте снова." << "\033[0m" << endl;
			// Сбрасываем флаги потока
			cin.clear();
			//Очистка буфера
			cin.ignore(INT_MAX, '\n');
			continue;
		}
		//Проверка на ввод имени файла, полностью состоящего из пробелов
		size_t j = 0;
		for (; j < strlen(f) && f[j] == ' '; j++);
		if (j == strlen(f)) {
			cout << "\033[1;31m" << "Имя файла не должно состоять полностью из пробелов! Попробуйте снова." << "\033[0m" << endl;
			continue;
		}
		flag = true;
		// Проверка на наличие специальных символов
		for (size_t i = 0; i < strlen(f); ++i) {
			if (f[i] == '/' ||
				f[i] == ':' ||
				f[i] == '*' ||
				f[i] == '<' ||
				f[i] == '>' ||
				f[i] == '|' ||
				f[i] == '?' ||
				f[i] == '*' ||
				f[i] == '@' ||
				f[i] == '.') {
				flag = false;
				cout << "\033[1;31m" << "Имя файла не должно содержать спец.символов ('@', '/', '<' и т.д.), не должно быть пустым! Попробуйте снова." << "\033[0m" << endl;
				break;
			}
		}
	} while (!flag);
}
//Проверка на ввод ключа
void Input_key(elist* begin, int &key, string phrase) {
	bool f = false;
	while (!f) {
		key = Input_int(phrase);
		for (elist* temp = begin; temp != NULL; temp = temp->right) {
			if (key == temp->data.number) {
				f = true;
				break;
			}
		}
		if (f == false) {
			cout << "\033[1;31m";
			puts("Вы ввели неправильный номер!\nВведите заново пожалуйста!");
			cout << "\033[0m\n";
		}
	}
}
//Проверка на ввод флага
void Input_flag(int &flag, string phrase) {
	bool f = false;
	while (!f) {
		flag = Input_int(phrase);
		if (flag == 0 || flag == 1) {
			f = true;
		}
		else {
			cout << "\033[1;31m";
			puts("Error! Некорректный вариант ввода! Только 0 или 1");
			cout << "\033[0m";
		}
	}
}
/*----------------------------------------*/

/*-----------Вывод одного элемента---------*/
void Print_el(elist* tmp) {
	int underscoreCount = 173;
	cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	cout << '|' << setw(5) << "ID" << '|'
		<< setw(50) << "Шифр группы" << '|'
		<< setw(50) << "Шифр дисциплины" << '|'
		<< setw(7) << "Отл." << '|'
		<< setw(7) << "Хор." << '|'
		<< setw(7) << "Удовл." << '|'
		<< setw(7) << "Неуд." << '|'
		<< setw(15) << "Пропуски Л" << '|'
		<< setw(15) << "Пропуски ПЗ" << '|'
		<< endl;
	cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	cout << '|' << setw(5) << tmp->data.number << '|'
		<< setw(50) << tmp->data.group_code << '|'
		<< setw(50) << tmp->data.discipline_code << '|'
		<< setw(7) << tmp->data.marks[0] << '|'
		<< setw(7) << tmp->data.marks[1] << '|'
		<< setw(7) << tmp->data.marks[2] << '|'
		<< setw(7) << tmp->data.marks[3] << '|'
		<< setw(15) << tmp->data.L_skips << '|'
		<< setw(15) << tmp->data.PZ_skips << '|'
		<< endl;
	cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	cout << endl;
} 
/*----------------------------------------*/

/*----------Вывод меню--------------------*/
void PrintMenu(int currentSelection) {
	system("cls"); // очистим экран (для Windows)

	cout << "\033[1;31m";
	cout << "ВЫБОР ДЕЙСТВИЯ:" << endl;
	cout << "\033[0m\n";
	for (int i = 0; i < 14; ++i) {
		if (currentSelection == i) {
			cout << "\033[1;32m"; // устанавливаем зеленый цвет текста
			cout << "-> ";
		}
		cout << "\t";
		switch (i + 1) {
			case 1:
				cout << "Организация таблицы\n";
				break;
			case 2:
				cout << "Просмотр таблицы\n";
				break;
			case 3:
				cout << "Добавление новой записи (в конец)\n";
				break;
			case 4:
				cout << "Удаление записи по ключу\n";
				break;
			case 5:
				cout << "Корректировка записи по ключу\n";
				break;
			case 6:
				cout << "Сортировка данных в таблице по возрастанию (по кол-ву пропущенных лекций)\n";
				break;
			case 7:
				cout << "Сортировка данных в таблице по убыванию (по кол-ву пропущенных лекций)\n";
				break;
			case 8:
				cout << "Поиск записи (по шифру дисциплины)\n";
				break;
			case 9:
				cout << "Сохранение таблицы в бинарный файл\n";
				break;
			case 10:
				cout << "Чтение данных из бинарного файла\n";
				break;
			case 11:
				cout << "Сохранение таблицы в текстовый файл\n";
				break;
			case 12:
				cout << "Чтение данных из текстового файла\n";
				break;
			case 13:
				cout << "Обработка таблицы\n";
				break;
			case 14:
				cout << "Очистка данных и выход\n";
				break;
		}

		if (currentSelection == i) {
			cout << "\033[0m"; // сбрасываем цвет обратно на стандартный
		}
	}
	puts("\n\n\n\n");
	cout << "\033[1;31m";
	puts("Допускается использование русской раскладки и CAPSLOCK!");
	cout << "\033[0m\n";
	cout << "\033[1;32m";
	puts("-ВВЕРХ: w;\n-ВНИЗ: s;");
	cout << "\033[0m";
}
/*----------------------------------------*/

/*---------Чтение информации----------*/
student Read_data() {
	student s;
	strcpy(s.group_code, Input_str("Введите шифр группы: "));
	strcpy(s.discipline_code, Input_str("Введите шифр дисциплины: "));
	s.marks[0] = Input_int("Введите количество отл. оценок: ");
	s.marks[1] = Input_int("Введите количество хор. оценок: ");
	s.marks[2] = Input_int("Введите количество удовл. оценок: ");
	s.marks[3] = Input_int("Введите количество неудовл. оценок: ");
	s.L_skips = Input_int("Введите количество пропущенных лекций: ");
	s.PZ_skips = Input_int("Введите количество пропущенных ПЗ: ");
	return s;
}
/*----------------------------------------*/

/*-----------Добавление в начало----------*/
elist* Add_to_start(elist* begin, student s) {
	elist* t = new elist;
	if (t == NULL) {
		cout << "Ошибка выделения памяти" << endl;
		return begin;
	}
	t->data = s;
	t->left = NULL;
	if (begin == NULL) {
		begin = t;
		t->right = NULL;
	}
	else {
		t->right = begin;
		begin->left = t;
		begin = t;
	}
	return begin;
}
/*----------------------------------------*/

/*-----------Добавление в конец-----------*/
elist* Add_to_end(elist* begin, elist* end, student s) {
	elist* t = new elist;
	if (t == NULL) {
		cout << "Ошибка выделения памяти" << endl;
		return end;
	}
	t->data = s;
	t->right = NULL;
	if (begin == NULL) {
		t->left = NULL;
		begin = t;
	}
	else {
		t->left = end;
		end->right = t;
		end = t;
	}
	return end;
}
/*----------------------------------------*/

/*----------------Организация списка при вводе с клавиатуры------------*/
elist* Organize_V(elist* begin, elist** end) {
	int flag_;

	do {
		if (begin == NULL) {
			begin = Add_to_start(begin, Read_data());
			*end = begin;
		}
		else {
			*end = Add_to_end(begin, *end, Read_data());
		}
		cout << "\033[1;31m" << "--Добавление--" << "\033[0m\n";
		Input_flag(flag_, "Добавляем элемент? (0 - нет, 1 - да): ");

		(*end)->data.number = ++id_number;

	} while (flag_);

	return begin;
}
/*----------------------------------------*/

/*----------------Просмотр списка---------------*/
void Print_table(elist* begin) {
	char c;
	int underscoreCount = 173;
	do {
		system("cls");

		// Вывод строки с нужным количеством символов '_'
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
		cout << '|' << setw(5) << "ID" << '|'
			<< setw(50) << "Шифр группы" << '|'
			<< setw(50) << "Шифр дисциплины" << '|'
			<< setw(7) << "Отл." << '|'
			<< setw(7) << "Хор." << '|'
			<< setw(7) << "Удовл." << '|'
			<< setw(7) << "Неуд." << '|'
			<< setw(15) << "Пропуски Л" << '|'
			<< setw(15) << "Пропуски ПЗ" << '|'
			<< endl;
		int RowsNumV = 0;
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
		for (elist* temp = begin; temp != NULL; temp = temp->right) {
			if ((RowsNumV >= CurrentRowV) && (RowsNumV < CurrentRowV + RowsOnPage)) {
				cout << '|' << setw(5) << temp->data.number << '|'
					<< setw(50) << temp->data.group_code << '|'
					<< setw(50) << temp->data.discipline_code << '|'
					<< setw(7) << temp->data.marks[0] << '|'
					<< setw(7) << temp->data.marks[1] << '|'
					<< setw(7) << temp->data.marks[2] << '|'
					<< setw(7) << temp->data.marks[3] << '|'
					<< setw(15) << temp->data.L_skips << '|'
					<< setw(15) << temp->data.PZ_skips << '|'
					<< endl;
			}
			RowsNumV++;
		}
		// Вывод строки с нужным количеством символов '_'
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;

		puts("\n\n\n\n\n\n\n\n\n");

		cout << "\033[1;31m";
		puts("Допускается использование русской раскладки и CAPSLOCK!");
		cout << "\033[0m\n";

		cout << "\033[1;32m";
		puts("-ВВЕРХ: w;\n-ВНИЗ: s;\n-ВЫХОД: q");
		cout << "\033[0m\n";
		cout << "______\n";

		cout << "\033[1;31m";
		puts("При наличии в списке меньше 5 элементов скроллинг работать не будет..");
		cout << "\033[0m\n";

		c = _getch();
		if (c == 'w' || c == 'W' || c == 'ц' || c == 'Ц') {
			if (CurrentRowV > 0) {
				CurrentRowV--;
			}
		}
		else if (c == 's' || c == 'S' || c == 'ы' || c == 'Ы') {
			if (RowsNumV > CurrentRowV + RowsOnPage) {
				CurrentRowV++;
			}
		}
	} while (c != 'q' && c != 'Q' && c != 'й' && c != 'Й');
	CurrentRowV = 0;
	return;
}
/*----------------------------------------*/

/*----------Удаление элемента по ключу----------*/
elist* Del_key_el(elist* begin, elist** end, int num_key) {
	//вывод удаляемого элемента
	elist* tmp_v = begin;
	for (; tmp_v->data.number != num_key; tmp_v = tmp_v->right);
	system("cls");
	cout << "\033[1;31m";
	puts("---Удаляемый элемент---");
	cout << "\033[0m";
	Print_el(tmp_v);
	//---
	
	//Особые случаи: 

	//Элемент является единственным в списке
	if (num_key == begin->data.number && begin == *end) {
		delete begin;
		begin = NULL;
		*end = NULL;
		return begin;
	}

	//Элемент стоит в начале
	if (num_key == begin->data.number && begin->right != NULL) {
		elist* tmp = begin;
		begin = begin->right;
		begin->left = NULL;
		delete tmp;
		return begin;
	}

	//Элемент стоит в конце
	if (num_key == (*end)->data.number) {
		elist* tmp = *end;
		*end = (*end)->left;
		(*end)->right = NULL;
		delete tmp;
		return begin;
	}


	//Общее удаление - элемент в середине
	elist* tmp = begin;
	elist* tmp2;
	for (; tmp->data.number != num_key; tmp = tmp->right);
	tmp2 = tmp;
	tmp2->left->right = tmp->right;
	tmp2->right->left = tmp->left;
	delete tmp;

	return begin;
}
/*----------------------------------------*/

/*----------Модификация элемента (изменение полей) по ключу и предварительный поиск----------*/
elist* Modification(elist* begin, int num_key) {
	elist* temp = begin;
	for (; temp->data.number != num_key; temp = temp->right);
	system("cls");
	cout << "\033[1;31m";
	puts("---Корректируемый элемент---");
	cout << "\033[0m";
	Print_el(temp);
	int a; //для switch-case
	puts("Выберите, какое поле вы бы хотели изменить:");
	puts("- 1 - Шифр группы");
	puts("- 2 - Шифр дисциплины");
	puts("- 3 - Оценки на выбор");
	puts("- 4 - Количество пропусков ПЗ");
	puts("- 5 - Количество пропусков лекций");
	puts("- 0 - Выход");
	cout << endl;
	a = Input_int("Ввод значения: ");
	system("cls");
	switch (a) {
		case 1:
			strcpy(temp->data.group_code, Input_str("Введите новый шифр группы: "));
			break;
		case 2:
			strcpy(temp->data.discipline_code, Input_str("Введите новый шифр дисциплины: "));
			break;
		case 3: {
			system("cls");
			puts("Выберите, какие оценки вы бы хотели изменить:");
			puts("- 1 - Отличные оценки");
			puts("- 2 - Хорошие оценки");
			puts("- 3 - Удовлетворительные оценки");
			puts("- 4 - Неудовлетворительные оценки");
			puts("- 0 - Выход");
			a = Input_int("Ввод значения: ");
			system("cls");
			switch (a) {
				case 1:
					temp->data.marks[0] = Input_int("Введите новое количество отл. оценок: ");
					break;
				case 2:
					temp->data.marks[1] = Input_int("Введите новое количество хор. оценок: ");
					break;
				case 3:
					temp->data.marks[2] = Input_int("Введите новое количество удовл. оценок: ");
					break;
				case 4:
					temp->data.marks[3] = Input_int("Введите новое количество неудовл. оценок: ");
					break;
				case 0:
					puts("--ВЫХОД--");
					break;
				default:
					puts("Неверный ввод!");
					puts("--ВЫХОД--");
			}
			break;
		}
		case 4:
			temp->data.L_skips = Input_int("Введите новое пропущенных лекций: ");
			break;
		case 5:
			temp->data.PZ_skips = Input_int("Введите новое пропущенных ПЗ: ");
			break;
		case 0:
			puts("--ВЫХОД--");
			break;
		default:
			puts("Неверный ввод!");
			puts("--ВЫХОД--");
	}
	return begin;
}
/*----------------------------------------*/

/*-----------------ФУНКЦИИ ДЛЯ СОРТИРОВКИ----------*/
/*----------Разбиение на 2 подсписка--------------*/
void split(elist* begin, elist** a, elist** b) {
	elist* slow = begin;
	elist* fast = begin->right;

	while (fast) {
		fast = fast->right;
		if (fast != NULL) {
			slow = slow->right;
			fast = fast->right;
		}
	}

	*b = slow->right;
	slow->right = NULL;
}
/*----------Слияние двух подсписков в порядке возрастания--------------*/
elist* Imerge(elist* a, elist* b) {
	if (a == NULL) {
		return b;
	}
	if (b == NULL) {
		return a;
	}

	if (a->data.L_skips < b->data.L_skips) {
		a->right = Imerge(a->right, b);
		a->right->left = a;
		a->left = NULL;
		return a;
	}
	else {
		b->right = Imerge(a, b->right);
		b->right->left = b;
		b->left = NULL;
		return b;
	}
}
/*----------Слияние двух подсписков в порядке убывания--------------*/
elist* Dmerge(elist* a, elist* b) {
	if (a == NULL) {
		return b;
	}
	if (b == NULL) {
		return a;
	}

	if (a->data.L_skips > b->data.L_skips) {
		a->right = Dmerge(a->right, b);
		a->right->left = a;
		a->left = NULL;
		return a;
	}
	else {
		b->right = Dmerge(a, b->right);
		b->right->left = b;
		b->left = NULL;
		return b;
	}
}
/*----------------------------------------*/

/*----------Сортировка по возрастанию--------------*/
void Imergesort(elist** begin) {
	if (*begin == NULL || (*begin)->right == NULL) {
		return;
	}

	elist* a = *begin, * b = NULL;
	split(*begin, &a, &b);

	Imergesort(&a);
	Imergesort(&b);

	*begin = Imerge(a, b);

	return;
}
/*----------------------------------------*/

/*----------Сортировка по убыванию----------------*/
void Dmergesort(elist** begin) {
	if (*begin == NULL || (*begin)->right == NULL) {
		return;
	}

	elist* a = *begin, * b = NULL;
	split(*begin, &a, &b);

	Dmergesort(&a);
	Dmergesort(&b);

	*begin = Dmerge(a, b);

	return;
}
/*----------------------------------------*/

/*----------Поиск элемента по полю ШИФР ДИСЦИПЛИНЫ----------*/
elist* Search(elist* begin, elist** end, char d[SLen], elist* begin2, elist** end2) {
	elist* temp = begin;
	int flag_;
	while (temp) {
		if (strcmp(temp->data.discipline_code, d) == 0) {
			if (begin2 == NULL) {
				begin2 = Add_to_start(begin2, temp->data);
				*end2 = begin2;
			}
			else {
				*end2 = Add_to_end(begin2, *end2, temp->data);
			}
		}
		temp = temp->right;
	}
	if (begin2 == NULL) {
		cout << "\033[1;31m";
		puts("ERROR! Такого элемента в списке нет!");
		cout << "\033[0m";
		Input_flag(flag_, "Хотите добавить? 0 - нет, 1 - да: ");
		if (flag_ == 1) {
			puts("Добавление элемента в конец..");
			*end = Add_to_end(begin, *end, Read_data());
			(*end)->data.number = ++id_number;
		}
		if (flag_ == 0) {
			puts("Как пожелаете..");
			return NULL;
		}
	}
	return begin2;
}
/*----------------------------------------*/

/*--------Запись в бинарный файл------*/
void Add_to_bin_file(char bin_filename_write[FLen], elist* begin) {
	FILE* bin = fopen(bin_filename_write, "wb");
	if (!bin) {
		cout << "\033[1;31m";
		puts("Ошибка открытия бинарного файла для записи таблицы!");
		cout << "\033[0m\n";
		return;
	} 
	elist* temp;
	student s2;
	fseek(bin, 0, SEEK_END);
	temp = begin;
	for (; temp != NULL; temp = temp->right) {
		s2 = temp->data;
		fwrite(&s2, sizeof(s2), 1, bin);
	}
	fclose(bin);
	puts("Данные были загружены в bin файл!");
	return;
}
/*----------------------------------------*/

/*----------Чтение из бинарного файла--------*/
elist* Read_from_bin_file(char bin_filename_read[FLen], elist* begin, elist** end) {
	FILE* bin = fopen(bin_filename_read, "rb");
	int flag_;
	if (!bin) {
		cout << "\033[1;31m";
		puts("Ошибка открытия бинарного файла для чтения таблицы!");
		cout << "\033[0m\n";
		return begin;
	}
	if (begin != NULL) {
		cout << "\033[1;31m";
		puts("Список не пуст!");
		cout << "\033[0m\n";
		Input_flag(flag_, "Хотите очистить память для возможности создать новый список? 0 - нет (оставляем этот список и выход), 1 - да (память будет очищена): ");
		if (flag_ == 1) {
			Memory_cleaning(begin);
			begin = NULL;
			*end = NULL;
			puts("Память очищена!\n\n");
		}
		if (flag_ == 0) {
			fclose(bin);
			return begin;
		}
	}
	size_t a;
	student s;
	rewind(bin);

	id_number = 0;

	while (1) {
		a = fread(&s, sizeof(s), 1, bin);
		if (a < 1) {
			break;
		}
		if (begin == NULL) {
			begin = Add_to_start(begin, s);
			*end = begin;
		}
		else {
			*end = Add_to_end(begin, *end, s);
		}

		(*end)->data.number = ++id_number;
	}
	
	fclose(bin);
	puts("Данные были загружены из bin файла!");
	return begin;
}
/*----------------------------------------*/

/*------Запись в текстовый файл-------*/
void Add_to_txt_file(char txt_filename_write[FLen], elist* begin) {
	ofstream fout;
	fout.open(txt_filename_write);
	if (!fout) {
		cout << "\033[1;31m";
		puts("Ошибка открытия текстового файла для сохранения таблицы!");
		cout << "\033[0m\n";
		return;
	}
	elist* temp_txt;
	student s;
	int count = 0;
	temp_txt = begin;
	for (; temp_txt != NULL; temp_txt = temp_txt->right) {
		s = temp_txt->data;
		fout << s.group_code << '\n';
		fout << s.discipline_code << '\n';
		fout << s.marks[0] << '\n';
		fout << s.marks[1] << '\n';
		fout << s.marks[2] << '\n';
		fout << s.marks[3] << '\n';
		fout << s.L_skips << '\n';
		fout << s.PZ_skips << '\n';
	}
	fout.close();
	puts("Данные были загружены в txt файл!");
	return;
}
/*----------------------------------------*/

/*----------Чтение списка из текстового файла----------*/
elist* Read_from_txt_file(char txt_filename_read[FLen], elist* begin, elist** end) {
	ifstream fin;
	int flag_;
	fin.open(txt_filename_read);
	if (!fin) {
		cout << "\033[1;31m";
		puts("Ошибка открытия текстового файла для чтения таблицы!");
		cout << "\033[0m\n";
		return begin;
	}
	if (begin != NULL) {
		cout << "\033[1;31m";
		puts("Список не пуст!");
		cout << "\033[0m\n";
		Input_flag(flag_, "Хотите очистить память для возможности создать новый список? 0 - нет (оставляем этот список и выход), 1 - да (память будет очищена): ");
		if (flag_ == 1) {
			Memory_cleaning(begin);
			begin = NULL;
			*end = NULL;
			puts("Память очищена!\n\n");
		}
		if (flag_ == 0) {
			fin.close();
			return begin;
		}
	}
	student s;
	elist* tmp = begin;

	id_number = 0;

	while (fin.peek() != EOF) {
		fin.getline(s.group_code, SLen);
		fin.getline(s.discipline_code, SLen);
		fin >> s.marks[0]; fin.ignore();
		fin >> s.marks[1]; fin.ignore();
		fin >> s.marks[2]; fin.ignore();
		fin >> s.marks[3]; fin.ignore();
		fin >> s.L_skips; fin.ignore();
		fin >> s.PZ_skips; fin.ignore();
		if (begin == NULL) {
			begin = Add_to_start(begin, s);
			*end = begin;
		}
		else {
			*end = Add_to_end(begin, *end, s);
		}

		(*end)->data.number = ++id_number;
	}
	fin.close();
	puts("Данные были загружены из txt файла!");
	return begin;
}
/*----------------------------------------*/

/*-----------------------------ОБРАБОТКА СПИСКА-----------------*/
/*-----------Добавление в начало (Обработка)----------*/
elistO* Add_to_startO(elistO* begin, studentO s) {
	elistO* t = new elistO;
	if (t == NULL) {
		cout << "Ошибка выделения памяти" << endl;
		return begin;
	}
	t->data = s;
	t->left = NULL;
	if (begin == NULL) {
		begin = t;
		t->right = NULL;
	}
	else {
		t->right = begin;
		begin->left = t;
		begin = t;
	}
	return begin;
}
/*-----------Добавление в конец (Обработка)-----------*/
elistO* Add_to_endO(elistO* begin, elistO* end, studentO s) {
	elistO* t = new elistO;
	if (t == NULL) {
		cout << "Ошибка выделения памяти" << endl;
		return end;
	}
	t->data = s;
	t->right = NULL;
	if (begin == NULL) {
		t->left = NULL;
		begin = t;
	}
	else {
		t->left = end;
		end->right = t;
		end = t;
	}
	return end;
}
/*----------Организация списка для обработки----------*/
elistO* OrganizeO(elistO* beginO, elistO** endO, elist* begin) {
	studentO s;
	elist* tmp = begin;
	elistO* tmpO = NULL;
	bool flag;
	do {
		flag = true;

		if (beginO != NULL) {
			for (elistO* temp = beginO; temp != NULL; temp = temp->right) {
				if (strcmp(temp->data.group_code, tmp->data.group_code) == 0) {
					tmpO = temp;
					flag = false;
				}
			}
		}

		if (flag == false) {
			tmpO->data.marks[0] += tmp->data.marks[0];
			tmpO->data.marks[1] += tmp->data.marks[1];
			tmpO->data.marks[2] += tmp->data.marks[2];
			tmpO->data.marks[3] += tmp->data.marks[3];
		}
		else {
			strcpy(s.group_code, tmp->data.group_code);
			s.marks[0] = tmp->data.marks[0];
			s.marks[1] = tmp->data.marks[1];
			s.marks[2] = tmp->data.marks[2];
			s.marks[3] = tmp->data.marks[3];
			if (beginO == NULL) {
				beginO = Add_to_startO(beginO, s);
				*endO = beginO;
			}
			else {
				*endO = Add_to_endO(beginO, *endO, s);
			}
		}
		tmp = tmp->right;
	} while (tmp);
	float count, sum;
	for (elistO* temp = beginO; temp != NULL; temp = temp->right) {
		count = temp->data.marks[0] + temp->data.marks[1] + temp->data.marks[2] + temp->data.marks[3];
		sum = temp->data.marks[0] * 5 + temp->data.marks[1] * 4 + temp->data.marks[2] * 3 + temp->data.marks[3] * 2;
		temp->data.aver_mark = (float)sum / count;
	}
	return beginO;
}
/*-------------Обработка таблицы (вывод на экран)----------------*/
void Obrabotka_V(elistO* beginO) {
	char c;
	int underscoreCount = 108;
	do {
		system("cls");

		// Вывод строки с нужным количеством символов '_'
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
		cout << '|'
			<< setw(50) << "Шифр группы" << '|'
			<< setw(24) << "Средний балл" << '|'
			<< setw(7) << "Отл." << '|'
			<< setw(7) << "Хор." << '|'
			<< setw(7) << "Удовл." << '|'
			<< setw(7) << "Неуд." << '|'
			<< endl;
		int RowsNumO = 0;
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
		for (elistO* temp = beginO; temp != NULL; temp = temp->right) {
			if ((RowsNumO >= CurrentRowO) && (RowsNumO < CurrentRowO + RowsOnPage)) {
				cout << '|' << setw(50) << temp->data.group_code << '|'
					<< setw(24) << fixed << setprecision(3) << temp->data.aver_mark << '|'
					<< setw(7) << temp->data.marks[0] << '|'
					<< setw(7) << temp->data.marks[1] << '|'
					<< setw(7) << temp->data.marks[2] << '|'
					<< setw(7) << temp->data.marks[3] << '|'
					<< endl;
			}
			RowsNumO++;
		}
		// Вывод строки с нужным количеством символов '_'
		cout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;

		puts("\n\n\n\n\n\n\n\n\n");
		cout << "\033[1;31m";
		puts("Допускается использование русской раскладки и CAPSLOCK!");
		cout << "\033[0m\n";

		cout << "\033[1;32m";
		puts("-ВВЕРХ: w;\n-ВНИЗ: s;\n-ВЫХОД: q");
		cout << "\033[0m\n";
		cout << "______\n";

		cout << "\033[1;31m";
		puts("При наличии в списке меньше 5 элементов скроллинг работать не будет..");
		cout << "\033[0m\n";

		c = _getch();
		if (c == 'w' || c == 'W' || c == 'ц' || c == 'Ц') {
			if (CurrentRowO > 0) {
				CurrentRowO--;
			}
		}
		else if (c == 's' || c == 'S' || c != 'ы' || c != 'Ы') {
			if (RowsNumO > CurrentRowO + RowsOnPage) {
				CurrentRowO++;
			}
		}
	} while (c != 'q' && c != 'Q' && c != 'й' && c != 'Й');
	CurrentRowO = 0;
	return;
}
/*-------------Обработка таблицы (файл)----------------*/
void Obrabotka_file(char txt_filename_obrabotka[FLen], elistO* beginO) {
	ofstream fout;
	fout.open(txt_filename_obrabotka);
	if (!fout) {
		cout << "\033[1;31m";
		puts("Ошибка открытия текстового файла для сохранения таблицы!");
		cout << "\033[0m\n";
		return;
	}
	elistO* temp;
	studentO s;
	temp = beginO;
	int underscoreCount = 108;
	fout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	fout << '|'
		<< setw(50) << "Шифр группы" << '|'
		<< setw(24) << "Средний балл" << '|'
		<< setw(7) << "Отл." << '|'
		<< setw(7) << "Хор." << '|'
		<< setw(7) << "Удовл." << '|'
		<< setw(7) << "Неуд." << '|'
		<< endl;
	fout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	for (elistO* temp = beginO; temp != NULL; temp = temp->right) {
		fout << '|' << setw(50) << temp->data.group_code << '|'
			<< setw(24) << fixed << setprecision(3) << temp->data.aver_mark << '|'
			<< setw(7) << temp->data.marks[0] << '|'
			<< setw(7) << temp->data.marks[1] << '|'
			<< setw(7) << temp->data.marks[2] << '|'
			<< setw(7) << temp->data.marks[3] << '|'
			<< endl;
	}
	fout << '+' << setw(underscoreCount) << setfill('_') << '+' << setfill(' ') << endl;
	fout.close();
	cout << "\033[1;32m";
	puts("РЕЗУЛЬТАТЫ ОБРАБОТКИ БЫЛИ СОХРАНЕНЫ В ТЕКСТОВЫЙ ФАЙЛ..");
	cout << "\033[0m";
	system("pause");
	return;
}
/*----------------Очистка памяти (Обработка)----------------*/
void Memory_cleaningO(elistO* begin) {
	elistO* temp;
	temp = begin;
	for (begin = begin->right; begin != NULL; begin = begin->right) {
		delete temp;
		temp = begin;
	}
	return;
}
/*------------------------------------------------------------------------*/


/*------Очистка памяти---------*/
void Memory_cleaning(elist* begin) {
	elist* temp;
	temp = begin;
	for (begin = begin->right; begin != NULL; begin = begin->right) {
		delete temp;
		temp = begin;
	}
	return;
}
/*----------------------------------------*/