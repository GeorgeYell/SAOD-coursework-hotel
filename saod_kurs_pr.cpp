#include <iostream>
#include <ctime>
#include <fstream>
#include <Windows.h>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "bstree.h";

using namespace std;

// Варианты:
// Регистрация постояльцев в гостинице
// Закрытое хеширование с квадратичным опробованием
// Шейкерная сортировка
// Циклический двунаправленный список
// Прямой метод обхода дерева
// Алгоритм поиска слова в тексте Боуера и Мура (БМ)

struct guest 
{
    char fio[40] = {};
    int year_birth = 0;;
	string adress;
	string purpose;
    char nom_pass[12] = {};
};

struct reg_form
{
	char nom_pass[12] = {};
	char nom[5] = {};
	char date_check_in[11] = {};
	char date_departure[11] = {};
};
struct list
{
    reg_form field;
    struct list* next; // указатель на следующий элемент
    struct list* prev; // указатель на предыдущий элемент
};

int size_arr_guests = 100; //размер хеш-таблицы гостей
int number_guests = 0;
//int arr_collision[size_arr_guests]; // массив для регистрации коллизий
guest *guests_hesh = new guest[size_arr_guests]; 
room elem;
nodeptr root, root1, min, max;
bstree bst;
list reg_list, reg_list_root;
list* reg_list_ptr = &reg_list, *reg_list_root_ptr = &reg_list_root;
int a, choice, findele, delele;
char ch = 'y';

/*
void write()
{
    ofstream write;
    write.open("сollisions.csv");

    for (int i = 0; i < size_arr_guests; i++)
    {
        write << arr_collision[i] << endl;
    }

    write.close();
}
*/

struct list* swap(struct list* lst_one, struct list* lst_two, struct list* head)
{
    // Возвращает новый корень списка
    struct list* prev1, * prev2, * next1, * next2;
    prev1 = lst_one->prev; // узел предшествующий lst1
    prev2 = lst_two->prev; // узел предшествующий lst2
    next1 = lst_one->next; // узел следующий за lst1
    next2 = lst_two->next; // узел следующий за lst2
    if (lst_two == next1)  // обмениваются соседние узлы
    {
        lst_two->next = lst_one;
        lst_two->prev = prev1;
        lst_one->next = next2;
        lst_one->prev = lst_two;
        next2->prev = lst_one;
        prev1->next = lst_two;
    }
    else if (lst_one == next2)  // обмениваются соседние узлы
    {
        lst_one->next = lst_two;
        lst_one->prev = prev2;
        lst_two->next = next1;
        lst_two->prev = lst_one;
        next1->prev = lst_two;
        prev2->next = lst_one;
    }
    else  // обмениваются отстоящие узлы
    {
        prev1->next = lst_two;
        lst_two->next = next1;
        prev2->next = lst_one;
        lst_one->next = next2;
        lst_two->prev = prev1;
        next2->prev = lst_one;
        lst_one->prev = prev2;
        next1->prev = lst_two;
    }
    if (lst_one == head)
        return(lst_two);
    if (lst_two == head)
        return (lst_one);
    return(head);
}

struct list* init(reg_form a)  // а- значение первого узла
{
    struct list* lst;
    // выделение памяти под корень списка
    lst = (struct list*)malloc(sizeof(struct list));
    lst->field = a;
    lst->next = lst; // указатель на следующий узел
    lst->prev = lst; // указатель на предыдущий узел
    return(lst);
}

struct list* shekerSort(struct list* lst)
{
    int left = 0, count = 0;
    struct list* p;
    p = lst;

    if (p != NULL)
    {
        do
        {
            count++;
            p = p->next;
        } while (p != lst);
    }

    int right = count - 1; // левая и правая границы сортируемой области массива
    int flag = 1;  // флаг наличия перемещений
    // Выполнение цикла пока левая граница не сомкнётся с правой
    // и пока в массиве имеются перемещения
    while ((left < right) && flag > 0)
    {
        flag = 0;
        for (int i = left; i < right; i++)  //двигаемся слева направо
        {
            if (strcmp(lst->field.nom, lst->next->field.nom) > 0) // если следующий элемент меньше текущего,
            {             // меняем их местами
                struct list* next = lst->next;
                p = swap(lst, next, p);
                flag = 1;      // перемещения в этом цикле были
            }
            else
            {
                lst = lst->next;
            }
        }
        right--; // сдвигаем правую границу на предыдущий элемент
        for (int i = right; i > left; i--)  //двигаемся справа налево
        {
            if (strcmp(lst->prev->field.nom, lst->field.nom) > 0)// если предыдущий элемент больше текущего,
            {            // меняем их местами
                struct list* prev = lst->prev;
                p = swap(lst, prev, p);
                flag = 1;    // перемещения в этом цикле были
            }
            else
            {
                lst = lst->prev;
            }
        }
        left++; // сдвигаем левую границу на следующий элемент
    }
    return p;
}

struct list* addelem(list* lst, reg_form form)
{
    struct list* temp, * p;
    temp = (struct list*)malloc(sizeof(list));
    p = lst->next; // сохранение указателя на следующий узел
    lst->next = temp; // предыдущий узел указывает на создаваемый
    temp->field = form; // сохранение поля данных добавляемого узла
    temp->next = p; // созданный узел указывает на следующий узел
    temp->prev = lst; // созданный узел указывает на предыдущий узел
    p->prev = temp;
    reg_list_root_ptr = shekerSort(reg_list_root_ptr);
    return(temp);
}

struct list* deletelem(list* lst)
{
    struct list* prev, * next;
    prev = lst->prev; // узел, предшествующий lst
    next = lst->next; // узел, следующий за lst
    prev->next = lst->next; // переставляем указатель
    next->prev = lst->prev; // переставляем указатель
    if (next->next == prev->prev)
    {
        reg_list_root_ptr = NULL;
        reg_list_ptr = NULL;
    }
    if (reg_list_root_ptr == lst)
    {
        reg_list_root_ptr == next;
        free(lst);
        reg_list_root_ptr = shekerSort(next);
    }
    else
    {
        free(lst); // освобождаем память удаляемого элемента
        reg_list_root_ptr = shekerSort(reg_list_root_ptr);
    }
    return(prev);
}

void listprint(list* lst)
{
    struct list* p;
    p = lst;
    if (p != NULL)
    {
        do {
            cout << "\nНомер: " << p->field.nom << "\nНомер паспорта: " << p->field.nom_pass;
            cout << "\nДата заселения: " << p->field.date_check_in;
            cout << "\nДата выезда: " << p->field.date_departure << endl;
            p = p->next; // переход к следующему узлу
        } while (p != lst); // условие окончания обхода
    }
    else
    {
        cout << "В данный момент в гостинице никто не зарегистрирован!\n";
    }
}

int hesh_fun(char key[])
{
    int tmp_hesh = ((int)key[0] * (int)key[1] + (int)key[1] * (int)key[2] + (int)key[2] * (int)key[3] +
                    (int)key[3] * (int)key[4] + (int)key[4] * (int)key[5] + (int)key[5] * (int)key[6]
                  + (int)key[6] * (int)key[7] + (int)key[7] * (int)key[8] + (int)key[8] * (int)key[9]
                  + (int)key[9] * (int)key[10] + (int)key[10] * (int)key[0]);

    int hesh = (tmp_hesh * (int)key[0] + tmp_hesh * (int)key[5] + 
                tmp_hesh * (int)key[10] + tmp_hesh* tmp_hesh) % size_arr_guests;
    //arr_collision[hesh] += 1;
    return hesh;
}

int hesh(char key[])
{
    int address;
    address = hesh_fun(key); // вычисление хеш функции
    string n1, n2;
    n1 = guests_hesh[address].nom_pass;
    n2 = key;

    int c = 0;
    for (int x = 0; n1 != n2; x++) //разрешение коллизий квадратичным опробованием
    {
        address = address + c + (x * x); // формула вычисления следующего адреса

        if (guests_hesh[address].nom_pass[0] == '!' || guests_hesh[address].nom_pass[0] == '@') //если ячейка свободна, записываем в неё;
            return address;

        if (address >= size_arr_guests)
        {
            c++;
            address = 0;
            x = 0;
        }
        n1 = guests_hesh[address].nom_pass;
    }
    return address;
}

void fak_getline()
{
    cin.clear();            // сбрасывает все биты потока
    cin.ignore(9999, '\n'); // игнорирует ввод
    cin.sync();             // удаляет данные из буффера
}

guest* new_size_arr(guest* Obj, int amount)
{
    guest* tempObj = new guest[amount];

    for (int i = 0; i < amount; i++)
        tempObj[i].nom_pass[0] = '!';

    for (int i = 0; i < amount - 100; i++)
    {
        if (Obj[i].nom_pass[0] != '!')
            tempObj[hesh(Obj[i].nom_pass)] = Obj[i];
    }
    delete[] Obj;
    Obj = tempObj;
    return Obj;
}

void check_number_guests()
{
    if (number_guests == (size_arr_guests - 50))
    {
        size_arr_guests += 100;
        guests_hesh = new_size_arr(guests_hesh, size_arr_guests);
    }
}

bool check_pass(char nomber[])
{
    for (int i = 0; i < 10; i++)
    {
        if (nomber[4] != '-')
        {
            return false;
        }
        if (i != 4 && (nomber[i] < '0' || nomber[i] > '9'))
        {
            return false;
        }
    }
    return true;
}

void registration_new_guest()
{
    guest new_guest;
    string fio;
    bool pass;
    system("cls");
    cout << "Регистрация нового постояльца\n";
    cout << "Введите ФИО: ";
    fak_getline();
    cin.getline(new_guest.fio, 40, '\n');
    do
    {
        cout << "Введите номер паспорта (в формате NNNN-NNNNNN): ";
        cin >> new_guest.nom_pass[0] >> new_guest.nom_pass[1] >> new_guest.nom_pass[2] 
            >> new_guest.nom_pass[3] >> new_guest.nom_pass[4] >> new_guest.nom_pass[5]
            >> new_guest.nom_pass[6] >> new_guest.nom_pass[7] >> new_guest.nom_pass[8] 
            >> new_guest.nom_pass[9] >> new_guest.nom_pass[10];
        new_guest.nom_pass[11] = '\0';
        
        pass = check_pass(new_guest.nom_pass);
        cin.sync();
    } while (!pass);
    cout << "Введите год рождения: ";
    while (!(cin >> new_guest.year_birth) || new_guest.year_birth < 1900 || new_guest.year_birth > 2021)
    {
        fak_getline();
        cerr << "Ошибка ввода!\n";
        cout << "Введите год рождения: ";
    }
    fak_getline();
    cout << "Введите адрес проживания: ";
    getline(cin, new_guest.adress, '\n');
    cout << "Введите цель прибытия: ";
    getline(cin, new_guest.purpose, '\n');
    number_guests++;
    check_number_guests();
    guests_hesh[hesh(new_guest.nom_pass)] = new_guest;
}

int find_guest_pass()
{
    bool pass;
    char nomber_pass[12] = {};
    int address;
    do
    {
        pass = true;
        cout << "Введите номер паспорта (в формате NNNN-NNNNNN): ";
        fak_getline();
        cin >> nomber_pass[0] >> nomber_pass[1] >> nomber_pass[2] >> nomber_pass[3] >> nomber_pass[4] >> nomber_pass[5]
            >> nomber_pass[6] >> nomber_pass[7] >> nomber_pass[8] >> nomber_pass[9] >> nomber_pass[10];
        nomber_pass[11] = '\0';
        cin.sync();
        pass = check_pass(nomber_pass);
    } while (!pass);

    address = hesh(nomber_pass);
    return address;
}

void find_guest_fio()
{
    bool b = true;
    char name[40] = {};
    string str1, str2;
    cout << "Введите ФИО: ";
    fak_getline();
    cin.getline(name, 40, '\n');
    str1 = name;
    for (int i = 0; i < size_arr_guests; i++)
    {
        str2 = guests_hesh[i].fio;
        if (str1 == str2)
        {
            cout << guests_hesh[i].fio << "\n" << guests_hesh[i].nom_pass << "\n\n";
            b = false;
        }
    }
    if (b)
    {
        cout << "Постояльцы не найдены!\n";
        system("pause");
    }
}

void del_guest(list* lst)
{
    int n;
    bool b = true;
    n = find_guest_pass();
    struct list* p;
    p = lst;
    if (p != NULL)
    {
        do {
            string n1 = p->field.nom_pass;
            string n2 = guests_hesh[n].nom_pass;
            if (n1 == n2)
            {
                b = false;
                break;
            }
            p = p->next; // переход к следующему узлу
        } while (p != lst);
    }

    if (b)
    {
        for (int i = 0; i < 40; i++) { guests_hesh[n].fio[i] = '\0'; }
        for (int i = 0; i < 12; i++) { guests_hesh[n].nom_pass[i] = '@'; }
        guests_hesh[n].year_birth = 0;
        guests_hesh[n].adress = "";
        guests_hesh[n].purpose = "";
        cout << "Успешно\n";
    }
    else
    {
        cout << "Вы не можете удалить из базы постояльца, пока он проживает в комнате!\n";
    }
}

void del_all_guest(int n)
{
    for (int i = 0; i < 40; i++) { guests_hesh[n].fio[i] = '\0'; }
    for (int i = 0; i < 12; i++) { guests_hesh[n].nom_pass[i] = '!'; }
    guests_hesh[n].year_birth = 0;
    guests_hesh[n].adress = "";
    guests_hesh[n].purpose = "";
}

void filling_guests()
{
    guest guests[200] = {}; //массив для хранения сгенерированных гостей
    //данные для генерации
    ifstream surnames;
    surnames.open("Surnames.txt");
    string city[10] = { "Санкт-Петербург", "Луга", "Выборг", "Тосно", "Гатчина", "Пушкин", "Зеленогорск",
                        "Рощино", "Кронштадт", "Петергоф" };
    string street[10] = { "Ленина", "Славы", "Труда", "Цветочная", "Кавказская", "Пушкина", "Лермонтова",
                        "9 Мая", "Центральная", "Дальняя" };
    char nomber_house[9] = { '1','2','3','4','5','6','7','8','9' };
    string purposes[3] = { "Туризм","Работа","Постоянное проживание" };

    for (int i = 0; i < 200; i++)
    {
        surnames.getline(guests[i].fio, 40, '\n');
        guests[i].adress = "г." + city[(rand() % 10)] + ", ул." + street[(rand() % 10)] + ", д." + nomber_house[(rand() % 9)];
        guests[i].purpose = purposes[(rand() % 3)];
        guests[i].year_birth = (rand() % 30) + 1970;

        for (int n = 0; n < 11; n++) // генерация номера паспорта в формате NNNN-NNNNNN
        {
            if (n != 4)
                guests[i].nom_pass[n] = (rand() % 9) + 48;
            else
                guests[i].nom_pass[n] = '-';
        }
        guests[i].nom_pass[11] = '\0';
        guests_hesh[hesh(guests[i].nom_pass)] = guests[i];
        number_guests++;
        check_number_guests();
    }
    surnames.close();
}

void filling_tree_room()
{
    char sim [10] = { '0','1','2','3','4','5','6','7','8','9' };
    for (int i = 0; i < 100; i++)
    {
        if (i < 26) //заполнение номеров типа ЛЮКС
        {
            elem.nom[0] = 'Л';
            elem.guest_number = i % 3 + 1;
            elem.room_number = i % 3 + 1;
            elem.toilet = true;
            elem.equipment = "кондиционер, холодильник, телевизор, интернет, алкоголь";
        }
        if (i > 25 && i < 51) //заполнение номеров типа МНОГОМЕСТНЫЙ
        {
            elem.nom[0] = 'М';
            elem.guest_number = i % 5 + 3;
            elem.room_number = i % 5 + 1;
            if (i % 2 == 0) elem.toilet = true;
            else elem.toilet = false;
            elem.equipment = "холодильник, интернет";
        }
        if (i > 50 && i < 76) //заполнение номеров типа ОДНОМЕСТНЫЙ
        {
            elem.nom[0] = 'О';
            elem.guest_number = 1;
            elem.room_number = 1;
            if (i % 2 == 0) elem.toilet = true;
            else elem.toilet = false;
            elem.equipment = "интернет";
        }
        if (i > 75) //заполнение номеров типа ПОЛУЛЮКС
        {
            elem.nom[0] = 'П';
            elem.guest_number = i % 3 + 1;
            elem.room_number = i % 3 + 1;
            elem.toilet = true;
            elem.equipment = "кондиционер, холодильник, интернет";
        }
        if (i == 99)
        {
            elem.nom[1] = '1';
            elem.nom[2] = '0';
        }
        else elem.nom[1] = '0';

        if (i < 99)
            elem.nom[2] = sim[(i + 1) / 10];
        elem.nom[3] = sim[(i + 1) % 10];
        elem.nom[4] = '\0';

        bst.insert(elem, root);
    }
}

void filling_registration_list(guest guests_hesh[], list*lst)
{
    reg_form fill;
    static int n;
    for (int count = 1; count < 21; count++)
    {
        string s = to_string(count * 4);
        while  (n < 200)
        {
            if (guests_hesh[n].nom_pass[0] == '!')
            {
                n++;
                continue;
            }   
            else
            {
                strcpy_s(fill.nom_pass, guests_hesh[n].nom_pass);
                n++;
                break;
            }
        }
        if (count < 6) //заполнение номеров типа ЛЮКС
        {
            fill.nom[0] = 'Л';
            fill.nom[1] = '0';
            
            if ((count * 4) > 9)
            {
                fill.nom[2] = s[0];
                fill.nom[3] = s[1];
            }
            else
            {
                fill.nom[2] = '0';
                fill.nom[3] = s[0];
            }
        }
        if (count > 5 && count < 11) //заполнение номеров типа МНОГОМЕСТНЫЙ
        {
            fill.nom[0] = 'М';
            fill.nom[1] = '0';
            fill.nom[2] = s[0];
            fill.nom[3] = s[1];

        }
        if (count > 10 && count < 16) //заполнение номеров типа ОДНОМЕСТНЫЙ
        {
            fill.nom[0] = 'О';
            fill.nom[1] = '0';
            fill.nom[2] = s[0];
            fill.nom[3] = s[1];
        }
        if (count > 15) //заполнение номеров типа ПОЛУЛЮКС
        {
            fill.nom[0] = 'П';
            fill.nom[1] = '0';
            fill.nom[2] = s[0];
            fill.nom[3] = s[1];
        }
        int day = rand() % 21 + 10;
        int month = rand() % 9;
        int year = 2021;
        string date = to_string(day) + ".0" + to_string(month) + "." + to_string(year);
        for (int j = 0; j < 11; j++)
        {
            if (j != 10)
                fill.date_check_in[j] = date[j];
            else
                fill.date_check_in[j] = '\0';
        }
        date = to_string(day) + ".0" + to_string(month + 1) + "." + to_string(year);
        for (int j = 0; j < 11; j++)
        {
            if (j != 10)
                fill.date_departure[j] = date[j];
            else
                fill.date_departure[j] = '\0';
        }
        if (count == 1)
            reg_list_root_ptr = init(fill);
        else
            reg_list_ptr = addelem(reg_list_root_ptr, fill);
    }
}

bool check_room_nom(char nom[])
{
    for (int i = 0; i < 4; i++)
    {
        if ((nom[0] == 'Л' || nom[0] == 'П' || nom[0] == 'О' || nom[0] == 'М')
            || (i != 0 && (nom[i] >= '0' || nom[i] <= '9')))
        {
            continue;
        }
        else return false;
    }
    return true;
}

void find_room_nom()
{
    room room_f;
    bool err = false;
    do
    {
        cout << "Введите номер (в формате БЦЦЦ), который хотите найти: ";
        cin >> elem.nom[0] >> elem.nom[1] >> elem.nom[2] >> elem.nom[3];
        elem.nom[4] = '\0';
    } while (!check_room_nom(elem.nom));
    room_f = bst.find(elem, root, &err);

    if (!err)
    {
        cout << "\nНомер: " << room_f.nom << "\n";
        cout << "Колличество мест: " << room_f.guest_number << endl;
        cout << "Колличество комнат: " << room_f.room_number << endl;
        (room_f.toilet) ? cout << "Есть сануезел\n" : cout << "Санузел отсутствует\n";
        cout << "Обарудование: " << room_f.equipment << endl;
    }
}

void add_room()
{
    int b = 0;
    cout << "\nФорма добавления гостиничного номера\n";
    do
    {
        cout << "Введите номер (в формате БЦЦЦ): ";
        cin >> elem.nom[0] >> elem.nom[1] >> elem.nom[2] >> elem.nom[3];
        elem.nom[4] = '\0';
    } while (!check_room_nom(elem.nom));
    cout << "Введите кол-во постояльцев: ";
    cin >> elem.guest_number;
    cout << "Введите кол-во комнат: ";
    cin >> elem.room_number;
    cout << "Укажите есть ли в номере санузел (1 - да, 0 - нет): ";
    cin >> b;
    (b == 0) ? elem.toilet = false : elem.toilet = true;
    fak_getline();
    cout << "Напишите какое оборудование есть в номере: ";
    getline(cin, elem.equipment, '\n');

    bst.insert(elem, root);
}

void del_room(list* lst)
{
    bool b = true;
    do
    {
        cout << "Введите номер (в формате БЦЦЦ), который хотите удалить: ";
        cin >> elem.nom[0] >> elem.nom[1] >> elem.nom[2] >> elem.nom[3];
        elem.nom[4] = '\0';
    } while (!check_room_nom(elem.nom));

    struct list* p;
    p = lst;
    if (p != NULL)
    {
        do {
            string n1 = p->field.nom;
            string n2 = elem.nom;
            if (n1 == n2)
            {
                b = false;
                break;
            }
            p = p->next; // переход к следующему узлу
        } while (p != lst);
    }

    if (b)
        bst.del(elem, root);
    else
        cout << "Вы не можете удалить из базы комнату, пока в ней проживает постоялец!\n";
}

bool check_date(char nom[])
{
    if (nom[2] != '.' && nom[5] != '.')
    {
        return false;
    }
    for (int i = 0; i < 10; i++)
    {
        if ((i != 2 && i != 5) && (nom[i] < '0' || nom[i] > '9'))
        {
            return false;
        }
    }
    return true;
}

vector <list*> find_room(list* lst, char nom[])
{
    struct list* p;
    p = lst;
    vector <list*> vec_list;
    do
    {
        string n1 = p->field.nom;
        string n2 = nom;
        if (n1 == n2)
            vec_list.push_back(p);

        p = p->next; // переход к следующему узлу
    } while (p != lst); // условие окончания обхода
    return vec_list;
}

void check_in()
{
    reg_form reg;
    guest guest;
    room room_n;
    bool b = true;
    bool err = false;
    int hesh_address;
    do
    {
        cout << "Введите номер комнаты для заселения (в формате БЦЦЦ): ";
        cin >> room_n.nom[0] >> room_n.nom[1] >> room_n.nom[2] >> room_n.nom[3];
        room_n.nom[4] = '\0';
    } while (!check_room_nom(room_n.nom));

    for(int i = 0; i < 5; i++)
        reg.nom[i] = room_n.nom[i];
    
    if (reg_list_root_ptr != NULL)
        b = find_room(reg_list_root_ptr, reg.nom).size() < bst.find(room_n, root, &err).guest_number;
    if (err)
        return void();
    else if (!b)
    {
        cout << "Ошибка, данный номер полностью заселен!\n";
        return void();
    }

    hesh_address = find_guest_pass();
    if (guests_hesh[hesh_address].nom_pass[0] == '!')
    {
        cout << "Пользователь не найден!\n";
        return;
    }
    else if (guests_hesh[hesh_address].nom_pass[0] == '@')
    {
        cout << "Пользователь удалён\n";
        return;
    }
    else
    {
        for (int i = 0; i < 12; i++)
            reg.nom_pass[i] = guests_hesh[hesh_address].nom_pass[i];
    }

    do
    {
        cout << "Введите дату заселения (в формате ДД.ММ.ГГГГ): ";
        cin >> reg.date_check_in[0] >> reg.date_check_in[1] >> reg.date_check_in[2]
            >> reg.date_check_in[3] >> reg.date_check_in[4] >> reg.date_check_in[5]
            >> reg.date_check_in[6] >> reg.date_check_in[7] >> reg.date_check_in[8]
            >> reg.date_check_in[9];
        reg.date_check_in[10] = '\0';
    } while (!check_date(reg.date_check_in));

    do
    {
        cout << "Введите дату выезда (в формате ДД.ММ.ГГГГ): ";
        cin >> reg.date_departure[0] >> reg.date_departure[1] >> reg.date_departure[2]
            >> reg.date_departure[3] >> reg.date_departure[4] >> reg.date_departure[5]
            >> reg.date_departure[6] >> reg.date_departure[7] >> reg.date_departure[8]
            >> reg.date_departure[9];
        reg.date_departure[10] = '\0';
    } while (!check_date(reg.date_departure));

    if (reg_list_root_ptr != NULL)
        reg_list_ptr = addelem(reg_list_root_ptr, reg);
    else
        reg_list_root_ptr = init(reg);
    
    cout << "Успешно!\n";
}

void departure_room()
{
    reg_form reg;
    vector <list*> vec;
    do
    {
        cout << "Введите номер комнаты для выселения (в формате БЦЦЦ): ";
        cin >> reg.nom[0] >> reg.nom[1] >> reg.nom[2] >> reg.nom[3];
        reg.nom[4] = '\0';
    } while (!check_room_nom(reg.nom));

    vec = find_room(reg_list_root_ptr, reg.nom);
    
    if (vec.empty())
        cout << "Ошибка! Данная комната свободна.\n";
    else
    {
        for (int i = 0; i < vec.size(); i++)
        {
            reg_list_ptr = deletelem(vec[i]);
        }
        cout << "Успешно! Номер расселен.\n";
    }
}

void departure_guest(list* lst)
{
    reg_form reg;
    do
    {
        cout << "Введите номер паспорта постояльца, которого надо выселить (в формате NNNN-NNNNNN): ";
        cin >> reg.nom_pass[0] >> reg.nom_pass[1] >> reg.nom_pass[2]
            >> reg.nom_pass[3] >> reg.nom_pass[4] >> reg.nom_pass[5]
            >> reg.nom_pass[6] >> reg.nom_pass[7] >> reg.nom_pass[8]
            >> reg.nom_pass[9] >> reg.nom_pass[10];
        reg.nom_pass[11] = '\0';
        cin.sync();
    } while (!check_pass(reg.nom_pass));
    bool del = false;
    struct list* p;
    p = lst;
    do {
        string n1 = p->field.nom_pass;
        string n2 = reg.nom_pass;
        if (n1 == n2)
        {
            p = deletelem(p);
            del = true;
            break;
        }
        p = p->next; // переход к следующему узлу
    } while (p != lst); // условие окончания обхода

    if (del)
        cout << "Посетитель выселен из номера.\n";
    else
        cout << "Выбранный вами посетитель не проживает ни в одном номере!\n";
}

void guest_data()
{
    system("cls");
    char command;
    int command_int, hesh_address;
    do
    {
        cout << "\nРабота с данными постояльцев\n";
        cout << "Меню:\n";
        cout << "1 - Регистрация нового постояльца\n";
        cout << "2 - Поиск по ФИО\n";
        cout << "3 - Поиск по номеру паспорта\n";
        cout << "4 - Отобразить всех гостей\n";
        cout << "5 - Удаление постояльца\n";
        cout << "6 - Удаление всей базы постояльцев\n";
        cout << "0 - Назад\n";
        cout << "Введите команду: ";
        cin >> command;
        command_int = command - '0';
        switch (command_int)
        {
        case 1: // Регистрация нового постояльца
            registration_new_guest();
            cout << "Успешно\n";
            system("pause");
            break;

        case 2: // Поиск по ФИО
            cout << "Поиск по ФИО\n";
            find_guest_fio();
            break;

        case 3: // Поиск по номеру паспорта
            cout << "Поиск постояльца по номеру паспорта\n";
            hesh_address = find_guest_pass();

            if (guests_hesh[hesh_address].nom_pass[0] == '!')
                cout << "Пользователь не найден!\n";
            else if (guests_hesh[hesh_address].nom_pass[0] == '@')
                cout << "Пользователь удалён\n";
            else
            {
                cout << "ФИО: " << guests_hesh[hesh_address].fio << "\nНомер паспорта: " << guests_hesh[hesh_address].nom_pass << "\nГод рождения: "
                    << guests_hesh[hesh_address].year_birth << "\nАдрес проживания: " << guests_hesh[hesh_address].adress
                    << "\nЦель визита: " << guests_hesh[hesh_address].purpose << "\n\n";
            }
            break;

        case 4: // Отобразить всех гостей
            bool b;
            b = true;
            for (int i = 0; i < size_arr_guests; i++)
            {
                if (guests_hesh[i].nom_pass[0] != '!')
                {
                    cout << "ФИО: " << guests_hesh[i].fio << "\nНомер паспорта: " << guests_hesh[i].nom_pass << "\nГод рождения: "
                        << guests_hesh[i].year_birth << "\nАдрес проживания: " << guests_hesh[i].adress
                        << "\nЦель визита: " << guests_hesh[i].purpose << "\n\n";
                    b = false;
                }
            }
            if (b)
            {
                cout << "Постояльцы не найдены!\n";
                system("pause");
            }
            break;

        case 5: //Удаление постояльца
            cout << "Удаление постояльца\n";
            del_guest(reg_list_root_ptr);
            
            system("pause");
            break;

        case 6: // Отчистка данных о всех постояльцах
            cout << "Отчистка данных о всех постояльцах\n";
            for (int i = 0; i < size_arr_guests; i++)
            {
                if (guests_hesh[i].nom_pass[0] != '!')
                    del_all_guest(i);
            }
            cout << "Успешно\n";
            system("pause");
            break;
        case 0:
            break;
        default:
            break;
        }
    } while (command_int != 0);
}

void room_data()
{
    system("cls");
    char command;
    int command_int, hesh_address;
    string need_equip;
    bool b = false;
    bool* bptr = &b;
    cout << "Гостиница\n";
    do
    {
        cout << "\nРабота с данными о номерах\n";
        cout << "Меню:\n";
        cout << "1 - Добавление нового гостиничного номера\n";
        cout << "2 - Удаление сведений о гостиничном номере\n";
        cout << "3 - Просмотр всех гостиничных номеров\n";
        cout << "4 - Очистка всех данных о гостиничных номерах\n";
        cout << "5 - Поиск гостиничного номера по «номеру»\n";
        cout << "6 - Поиск гостиничного номера по фрагментам «Оборудования»\n";
        cout << "0 - Назад\n";
        cout << "Введите команду: ";
        cin >> command;
        command_int = command - '0';
        switch (command_int)
        {
        case 1: // Добавление нового гостиничного номера
            add_room();
            break;
        case 2: // Удаление сведений о гостиничном номере
            del_room(reg_list_root_ptr);
            break;
        case 3: // Просмотр всех имеющихся гостиничных номеров
            bst.preorder(root);
            break;
        case 4:// Очистка всех данных о гостиничных номерах
            bst.makeempty(root);
            cout << "Все данные о номерах успешно удалены!\n";
            break;
        case 5: // Поиск гостиничного номера по «номеру»
            find_room_nom();
            break;
        case 6: // Поиск гостиничного номера по фрагментам «Оборудования»
            cout << "Введите фрагмент обарудования по которому хотите искать номера: ";
            fak_getline();
            getline(cin, need_equip, '\n');
            bst.search_equip(root, need_equip, bptr);
            if (!b)
                cout << "\nПо вашему запросу ничего не найдено!\n";
        case 0:
            break;
        default:
            break;
        }
    } while (command_int != 0);
}

void reg_data()
{
    system("cls");
    char command;
    int command_int, hesh_address;
    vector <list> list_find;
    reg_form reg;
    list p;

    cout << "Гостиница\n";
    do
    {
        cout << "\nРабота с регистрацией постояльцев\n";
        cout << "Меню:\n";
        cout << "1 - Заселение\n";
        cout << "2 - Выселение всего номера\n";
        cout << "3 - Выселение конкретного постояльца\n";
        cout << "4 - Просмотр всех заселённых номеров\n";
        cout << "0 - Назад\n";
        cout << "Введите команду: ";
        cin >> command;
        command_int = command - '0';
        switch (command_int)
        {
        case 1: // Заселение
            check_in();
            break;
        case 2: // Выселение всей комнаты
            departure_room();
            break;
        case 3: // Выселение конкретного постояльца
            departure_guest(reg_list_root_ptr);
            break;
        case 4:
            listprint(reg_list_root_ptr);
            break;
        case 0:
            break;
        default:
            break;
        }
    } while (command_int != 0);
}

int main()
{
	setlocale(LC_ALL, "Rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(time(0));
    char command;
    int command_int, hesh_address;
    root = NULL;
    root1 = NULL;

    for (int i = 0; i < size_arr_guests; i++) //заполнение массива спец символом
    {
        guests_hesh[i].nom_pass[0] = '!';
    }
    /*
    for (int i = 0; i < size_arr_guests; i++) //заполнение массива коллизий нулями
    {
        arr_collision[i] = 0;
    }
    */
    filling_guests(); // заполнение базы данных о клиентах
    filling_tree_room(); //заполнение базы данных о номерах
    filling_registration_list(guests_hesh, reg_list_root_ptr);
    reg_list_root_ptr = shekerSort(reg_list_root_ptr);
    do 
    {
        system("cls");
        cout << "Гостиница\n";
        cout << "\nГлавное Меню:\n";
        cout << "1 - Работа с данными постояльцев\n";
        cout << "2 - Работа с данными о номерах\n";
        cout << "3 - Регистрация\n";
        cout << "0 - Выход\n";
        cout << "Введите команду: ";
        cin >> command;
        command_int = command - '0';

        switch (command_int)
        {
        case 1: //Работа с данными постояльцев
            guest_data();
            break;
        case 2: // Работа с данными о номерах
            room_data();
            break;
        case 3: // Работа с регистрацией постояльцев
            reg_data();
            break;
        case 0:
            break;
        default:
            continue;
        }
    } while (command_int != 0);
}