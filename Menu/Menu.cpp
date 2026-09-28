#include <iostream>
#include "Menu.h"

void simpleFunction() {
    std::cout << "You did it!\n";
}

int main()
{
    Menu mainMenu("Main Menu", MenuType::Main);
    Menu subMenu("Sub Menu", MenuType::Sub);

    mainMenu.addSubMenu("Sub menu", subMenu);
    mainMenu.addOption("Do it", simpleFunction);

    subMenu.addOption("Do this", simpleFunction);

    mainMenu.run();
}