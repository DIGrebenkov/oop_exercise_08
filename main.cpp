/*
    Гребенков Д.И.
    Группа М8О-207Б-19
    Вариант: 12 (трапеция, ромб, пятиугольник).

Создать приложение, которое будет считывать из стандартного ввода данные фигур, согласно варианту задания, выводить их характеристики на экран и записывать в файл.
Программа должна:
Осуществлять ввод из стандартного ввода данных фигур, согласно варианту задания;
Программа должна создавать классы, соответствующие введенным данных фигур;
Программа должна содержать внутренний буфер, в который помещаются фигуры. Для создания буфера допускается использовать стандартные контейнеры STL. Размер буфера задается параметром командной строки.
При накоплении буфера они должны запускаться на асинхронную обработку, после чего буфер должен очищаться;
Обработка должна производиться в отдельном потоке;
Реализовать два обработчика, которые должны обрабатывать данные буфера:
Вывод информации о фигурах в буфере на экран;
Вывод информации о фигурах в буфере в файл. Для каждого буфера должен создаваться файл с уникальным именем.
Оба обработчика должны обрабатывать каждый введенный буфер. Т.е. после каждого заполнения буфера его содержимое должно выводиться как на экран, так и в файл.
Обработчики должны быть реализованы в виде лямбда-функций и должны хранится в специальном массиве обработчиков. Откуда и должны последовательно вызываться в потоке – обработчике.
В программе должно быть ровно два потока (thread). Один основной (main) и второй для обработчиков;
В программе должен явно прослеживаться шаблон Publish-Subscribe. Каждый обработчик должен быть реализован как отдельный подписчик.
Реализовать в основном потоке (main) ожидание обработки буфера в потоке-обработчике. Т.е. после отправки буфера на обработку основной поток должен ждать, пока поток обработчик выведет данные на экран и запишет в файл.

*/
#include <iostream>
#include <cmath>
#include <memory>
#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <functional>
#include <vector>

const double pi = 3.14159;

std::ofstream wf;

template <typename T>
class Figure {
    public:
        virtual int type () = 0;
        virtual void vertices () = 0;
        virtual void writeInFile () = 0;
        virtual ~Figure () {};
};

template <typename T>
class Trapezoid: public Figure <T> {
    public:
        std::pair <T, T> left_top, left_bottom, right_bottom;
        Trapezoid () {}
        Trapezoid (T ltx, T lty, T lbx, T lby, T rbx, T rby) {
            left_top.first = ltx;
            left_top.second = lty;
            left_bottom.first = lbx;
            left_bottom.second = lby;
            right_bottom.first = rbx;
            right_bottom.second = rby;
        }
        virtual int type () {
            return 1;
        }
        virtual void vertices () {
            std::cout << "Trapezoid: (" << left_top.first << ", " << left_top.second << ")"
            << "(" << left_bottom.first << ", " << left_bottom.second << ")"
            << "(" << right_bottom.first << ", " << right_bottom.second << ")"
            << "(" << right_bottom.first - left_top.first + left_bottom.first 
            << ", " << left_top.second << ")" << std::endl;
        }
        virtual void writeInFile () {
            wf << "Trapezoid: (" << left_top.first << ", " << left_top.second << ")"
            << "(" << left_bottom.first << ", " << left_bottom.second << ")"
            << "(" << right_bottom.first << ", " << right_bottom.second << ")"
            << "(" << right_bottom.first - left_top.first + left_bottom.first 
            << ", " << left_top.second << ")" << std::endl;
        }
};

template <typename T>
class Rhombus: public Figure <T> {
    public:
        std::pair <T, T> left, bottom;
        Rhombus () {}
        Rhombus (T lx, T ly, T bx, T by) {
            left.first = lx;
            left.second = ly;
            bottom.first = bx;
            bottom.second = by;
        }
        virtual int type () {
            return 2;
        }
        virtual void vertices () {
            std::cout << "Rhombus: (" << left.first << ", " << left.second << ")";
            std::cout << "(" << bottom.first << ", " << bottom.second << ")";
            std::cout << "(" << 2 * bottom.first - left.first << ", " << left.second << ")";
            std::cout << "(" << bottom.first << ", " << 2 * left.second - bottom.second << ")" 
            << std::endl;
        }
        virtual void writeInFile () {
            wf << "Rhombus: (" << left.first << ", " << left.second << ")"
            << "(" << bottom.first << ", " << bottom.second << ")"
            << "(" << 2 * bottom.first - left.first << ", " << left.second << ")"
            << "(" << bottom.first << ", " << 2 * left.second - bottom.second << ")"
            << std::endl;
        }
};

template <typename T>
class Pentagon: public Figure <T> {
    public:
        std::pair <T, T> center;
        T radius;
        Pentagon () {}
        Pentagon (T cx, T cy, T r) {
            center.first = cx;
            center.second = cy;
            radius = r;
        }
        virtual int type () {
            return 3;
        }
        virtual void vertices () {
            std::cout << "Pentagon: (" << center.first - radius * sin (72 * pi / 180) << ", "
            << center.second + radius * cos (72 * pi / 180) << ")";
            std::cout << "(" << center.first << ", " << center.second + radius << ")";
            std::cout << "(" << center.first + radius * sin (72 * pi / 180) << ", " <<
            center.second + radius * cos (72 * pi / 180) << ")";
            std::cout << "(" << center.first + radius * sin (36 * pi / 180) << ", " <<
            center.second - radius * cos (36 * pi / 180) << ")";
            std::cout << "(" << center.first - radius * sin (36 * pi / 180) << ", " <<
            center.second - radius * cos (36 * pi / 180) << ")" << std::endl;
        }
        virtual void writeInFile () {
            wf << "Pentagon: (" << center.first - radius * sin (72 * pi / 180) << ", "
            << center.second + radius * cos (72 * pi / 180) << ")"
            << "(" << center.first << ", " << center.second + radius << ")"
            << "(" << center.first + radius * sin (72 * pi / 180) << ", "
            << center.second + radius * cos (72 * pi / 180) << ")"
            << "(" << center.first + radius * sin (36 * pi / 180) << ", "
            << center.second - radius * cos (36 * pi / 180) << ")"
            << "(" << center.first - radius * sin (36 * pi / 180) << ", " 
            << center.second - radius * cos (36 * pi / 180) << ")" << std::endl;
        }
};

template <typename T>
class Factory {
    public:
        static std::shared_ptr <Figure <T> > create (int type) {
            if ((type > 0) && (type < 4)) {
                if (type == 1) {
                    T ltx, lty, lbx, lby, rbx, rby;
                    std::cout << "Enter coordinates of left-top, left-bottom and right-bottom vertices: "
                    << std::endl;
                    std::cin >> ltx >> lty >> lbx >> lby >> rbx >> rby;
                    Trapezoid <T> * temp (new Trapezoid <T> (ltx, lty, lbx, lby, rbx, rby));
                    return std::shared_ptr <Figure <T> > (temp);
                }
                else if (type == 2) {
                    T lx, ly, bx, by;
                    std::cout << "Enter coordinates of left and bottom vertices: " << 
                    std::endl;
                    std::cin >> lx >> ly >> bx >> by;
                    Rhombus <T> * temp (new Rhombus <T> (lx, ly, bx, by));
                    return std::shared_ptr <Figure <T> > (temp);
                }
                else if (type == 3) {
                    T r, cx, cy;
                    std::cout << "Enter coordinates of center and radius: " << 
                    std::endl;
                    std::cin >> cx >> cy >> r;
                    Pentagon <T> * temp (new Pentagon <T> (cx, cy, r));
                    return std::shared_ptr <Figure <T> > (temp);
                }
            }
            std::shared_ptr <Figure <T> > temp;
            return (temp);
        }
};

int gen = 0;
std::queue <std::shared_ptr <Figure <int> > > mes;

template <typename T>
class Publisher {
    public:
        std::queue <std::shared_ptr <Figure <T> > > que;
        std::mutex mtx;
        void add (int type) {
            std::shared_ptr <Figure <T> > temp = Factory <T> :: create (type);
            que.push (temp);
        }
        void publish () {
            mtx.lock ();
            while (!que.empty ()) {
                mes.push (que.front ());
                que.pop ();
            }
            mtx.unlock ();
        }
};

template <typename T>
void threadF () {
    std::string name = std::to_string (gen);
    name.append (".txt");
    wf.open (name.c_str ());
    std::function <void (std::shared_ptr <Figure <T> > figure)> print = 
    [] (std::shared_ptr <Figure <T> > figure) {
            figure->vertices ();
    };
    std::function <void (std::shared_ptr <Figure <T> > figure)> write = 
    [] (std::shared_ptr <Figure <T> > figure) {
            figure->writeInFile ();
    };
    std::vector <std::function <void (std::shared_ptr <Figure <T> >)> > arr = {print, write};
    std::mutex mtx;
    std::shared_ptr <Figure <T> > ptr;
    if (!mes.empty ()) {
        mtx.lock ();
        while (!mes.empty ()) {
            ptr = mes.front ();
            for (auto func: arr) {
                func (ptr);
            }
            mes.pop ();
        }
        mtx.unlock ();
        gen ++;
    }
    wf.close ();
}

void showMenu () {
    std::cout << "0. Show menu." << std::endl;
    std::cout << "1. Enter trapezoid." << std::endl;
    std::cout << "2. Enter rhombus." << std::endl;
    std::cout << "3. Enter pentagon." << std::endl;
    std::cout << "4. Quit the program without publishing." << std::endl;
}

int main (int argc, char* argv[]) {
    unsigned long long int bufferSize = std::stoi (std::string (argv [1]));
    Publisher <int> pub;
    int option = 0;
    showMenu ();
    while (option != 4) {
        std::cout << "Enter option: ";
        std::cin >> option;
        switch (option) {
            case 0: {
                showMenu ();
                break;
            }
            case 1: {
                pub.add (1);
                if (pub.que.size () >= bufferSize) {
                    pub.publish ();
                    std::thread thr (threadF <int>);
                    thr.join ();
                }
                break;
            }
            case 2: {
                pub.add (2);
                if (pub.que.size () >= bufferSize) {
                    pub.publish ();
                    std::thread thr (threadF <int>);
                    thr.join ();
                }
                break;
            }
            case 3: {
                pub.add (3);
                if (pub.que.size () >= bufferSize) {
                    pub.publish ();
                    std::thread thr (threadF <int>);
                    thr.join ();
                }
                break;
            }
            case 4: {
                break;
            }
            default: {
                std::cerr << "ERROR: incorrect option." << std::endl;
                break;
            }
        }
    }
    return 0;
}