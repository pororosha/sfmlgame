#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

using namespace sf;

int main(){
    SoundBuffer buffer;//крч в ближайших нескольких строках обитает подключение звука
    if (!buffer.loadFromFile("phonk.mp3")) return -1;
    Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(10);
    Vector2u razm;//размеры окна по х и у
    razm.x = 1000;
    razm.y = 1000; 
    Vector2u razm_old = razm; //размеры окна до последнего изменения
    RenderWindow window(sf::VideoMode(razm.x, razm.y), L"Иллюзия", Style::Default);//создаём окно
    View view(FloatRect(0, 0, razm.x, razm.y));//создаём камеру, размером с окно
    window.setVerticalSyncEnabled(true);//изначально окно вертикально, чтоб без рандомных углов
    int rad = 50;// радиус круга-модельки персонажа
    Sprite hero;
    Texture SF;
    if (not SF.loadFromFile("zxc.png")) return -1;
    hero.setOrigin(rad, rad);
    hero.setTexture(SF);
    Sprite map;
    Texture minimap;
    if (not minimap.loadFromFile("map.png")) return -1;
    map.setTexture(minimap);
    map.setPosition(0, 0);
    double vx = 0; //скорость персонажа
    double vy = 0; 
    double x = 500; //координаты персонажа 
    //(не вектор, потому что они были первые и обошлось просто х и у)
    double y = 500;
    double len = 1;//задача переменной, которая впоследствие будет нужна для рассчёта скорости персонажа
    double movespeed = 7.0; //скорость героя
    Vector2i mouse_position; //текущие координаты курсора
    Vector2i mouse_old; //координаты курсора кадра назад
    Vector2i viewcenter; //центр камеры (вид)
    viewcenter.x = razm.x / 2; //изначально в центре 
    viewcenter.y = razm.y / 2;
    hero.setPosition(viewcenter.x, viewcenter.y);//начальная позиция героя
    bool islkm = false;//проверка, нажата ли левая кнопка мыши
    bool islkm_old = false; //была ли нажата ли левая кнопка мыши в прошлом кадре
    int dx = 0;//перемещение камеры по х
    int dy = 0;//по у
    bool is_mouse_in_window;//в окне ли курсор?
    bool is_floor;//находится ли курсор в допустимой для перемещения области
    const int num_obj = 4;

    int borders[num_obj][4]{//массив стенок
        {0,0,50,800},
        {0,0,800,50},
        {750,0,800,800},
        {0,750,800,800}
    }; // весёлые задавушки)))

    while (window.isOpen()){
        Event event;
        
        is_floor = true;
        mouse_position = Mouse::getPosition(window); //смотрим, где курсор
        is_mouse_in_window = mouse_position.x >= 0 and mouse_position.x <= razm.x and mouse_position.y >= 0 and mouse_position.y <= razm.y;
        
        ////////////////////////////////////////////СОБЫТИЯ В ОКНЕ/////////////////////////////////////////////////////////////////////////////////////
        while (window.pollEvent(event)){
            if (event.type == Event::Closed) window.close();
            else if (event.type == Event::Resized) {
                razm = window.getSize();
                view.setSize(razm.x, razm.y);
                dx -= (razm.x - razm_old.x)/2;//меняем восприятие мыши при изменении размера окна
                dy -= (razm.y - razm_old.y)/2;//не знаю, как это работает, но работает => не трогаем
                razm_old = razm;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //std::cout << mouse_position.x << " " << mouse_position.y << "   " << x << " " << y << "\n";

        if (sound.getStatus() == 0) sound.play();//не перестаём радовать сф-а смачными битами даже если они закончились

        //////////////////////////////////////////////ПЕРЕДВИЖЕНИЕ/////////////////////////////////////////////////////////////////////////////////////
        //len = sqrt(pow((mouse_position.x - x + dx), 2) + pow((mouse_position.y - y + dy), 2));

        if (Mouse::isButtonPressed(Mouse::Right) and is_mouse_in_window) {
            len = sqrt(pow((mouse_position.x - x + dx), 2) + pow((mouse_position.y - y + dy), 2));
            vx = (mouse_position.x - x + dx) / len;
            vy = movespeed * (mouse_position.y - y + dy) / len;
            if (vy<=0) hero.setRotation(asin(vx) * 57.3);
            else hero.setRotation(180-asin(vx) * 57.3);
            vx *= movespeed;
        }//по теореме пифагора и подобиям задаём скорости по х и у, найдя синусы косинусы там...

        for (int i = 0; i < num_obj; i++) {
            if (borders[i][0] <= (x + vx) and borders[i][2] >= (x + vx) and borders[i][1] <= (y + vy) and borders[i][3] >= (y + vy)) {
                is_floor = false;
                break;
            }
        }

        if (is_floor and len > rad) { x += vx; y += vy; len -= movespeed; }//двигаем, если не стенка

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        islkm_old = islkm;
        if (Mouse::isButtonPressed(Mouse::Middle) and is_mouse_in_window) islkm = true; //нажата ли левая кнопка мыши
        else islkm = false;

        if (islkm and islkm_old) {
            viewcenter.x = viewcenter.x - mouse_position.x + mouse_old.x;
            viewcenter.y = viewcenter.y - mouse_position.y + mouse_old.y;
            dx += (mouse_old.x - mouse_position.x);
            dy += (mouse_old.y - mouse_position.y);
            view.setCenter(viewcenter.x, viewcenter.y);
        } //если левая кнопка мыши удерживается - двигаем камеру за курсором

        mouse_old = mouse_position;
        
        hero.setPosition(x, y); //возможно возникает вопрос: а зачем задавать координаты, если можно 
        //использовать функцию .move, а я отвечу - да по фану, мне просто координаты нужны для 
        //расчётов выше, поэтому они есть.

        window.setView(view); //переводимся на вид с камеры
        window.clear(Color::Blue); //чистим, чтобы не накладывались рисунки
        window.draw(map);
        window.draw(hero); //рисуем собсна героя
        window.display();//хз, так надо
    }
    return 0;
}