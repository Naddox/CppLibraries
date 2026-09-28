#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <limits>

enum class MenuType {
	Main,   ///< Option 0 is displayed as Exit
	Sub     ///< Option 0 is displayed as Back
};

class Menu
{
public:
    /**
     *@brief Creates a new menu.
     * 
     * @param title The text displayed at the top of the menu.
     * @param type The menu type. Use MenuType::Main for the root menu
     *             or MenuType::Sub for a submenu.
     */
    Menu(const std::string& title, MenuType type)
        : title(title), type(type)
    {
    }

    /**
     * @brief Adds a selectable option to the menu.
     * 
     * @param option Text displayed for the option.
     * @param action Function executed when the option is selected.
     */
	void addOption(const std::string& option, std::function<void()> action) {
		MenuOption menuOption{ 
            option,
            action,
            MenuOptionType::Action 
        };
		options.push_back(menuOption);
	}

    /**
     * @brief Adds a selectable subMenu option to the menu.
     *
     * @param option Text displayed for the subMenu option.
     * @param subMenu The menu object opened when this option is selected.
     */
    void addSubMenu(const std::string& option, const Menu& subMenu) {
        MenuOption menuOption{ 
            option,
            [&subMenu]() {subMenu.run(); },
            MenuOptionType::SubMenu 
        };
        options.push_back(menuOption);
    }

    /**
     * @brief Displays the menu and processes selections until the user exits.
     * 
     * Main menus display "Exit" for option 0.
     * Submenus display "Back" for option 0.
     */
    void run() const {
        if (options.empty()) {
            return;
        }
        while (true) {
            display();
            int choice = getSelection();

            if (choice == 0) {
                break;
            }
            
            const MenuOption& selectedOption = options[choice - 1];
            selectedOption.action();

            if (selectedOption.type == MenuOptionType::Action) {
                waitForContinue();
            }
        }
    }

private:
	std::string title;
	MenuType type;

    enum class MenuError {
        InvalidInput,
        ExtraCharacters,
        InvalidSelection
    };

    enum class MenuOptionType {
        Action,
        SubMenu
    };

	struct MenuOption {
		std::string option;
		std::function<void()> action;
        MenuOptionType type;
	};

	std::vector<MenuOption> options;

	void clearScreen() const {
		std::cout << "\033[2J\033[1;1H";
	}

    void waitForContinue() const {
        std::cout << "\nPress ENTER to return to the menu...";
        std::cin.get();
    }

    void display() const {
        clearScreen();

        std::size_t menuWidth = 32;
        if (title.length() + 4 > menuWidth) {
            menuWidth = title.length() + 4;
        }

        std::string divider(menuWidth, '=');
        std::size_t padding = (menuWidth - title.length()) / 2;
        std::cout << divider << '\n';
        std::cout << std::string(padding, ' ') << title << '\n';
        std::cout << divider << "\n\n";

        for (std::size_t i = 0; i < options.size(); ++i) {
            const MenuOption& currentOption = options[i];
            if (currentOption.type == MenuOptionType::SubMenu) {
                std::cout << "    " << (i + 1) << ". " 
                          << currentOption.option << " ->\n";
            }
            else {
                std::cout << "    " << (i + 1) << ". " 
                          << currentOption.option << std::endl;
            }
        }
        if (MenuType::Sub == type) {
            std::cout << "    0. Back\n";
        }
        else {
            std::cout << "    0. Exit\n";
        }
        std::cout << "\nSelect an option: ";
    }

    int getSelection() const {
        int selection;
        std::string inputBuffer;
        while (true) {
            try {
                // Verify input starts with an int
                if (!(std::cin >> selection)) {
                    throw MenuError::InvalidInput;
                }
                // Check for remaining characters in buffer
                std::getline(std::cin, inputBuffer);
                if (!inputBuffer.empty()) {
                    for (std::size_t i = 0; i < inputBuffer.length(); ++i) {
                        if (inputBuffer[i] != ' ') {
                            throw MenuError::ExtraCharacters;
                        }
                    }
                }
                // Validate the selection is within range of total options
                if (selection < 0) {
                    throw MenuError::InvalidSelection;
                }
                if (static_cast<std::size_t>(selection) > options.size()) {
                    throw MenuError::InvalidSelection;
                }
                break;
            }
            catch (MenuError error) {
                switch (error) {
                case MenuError::InvalidInput:
                    std::cerr << "Invalid input. Please enter a number: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;

                case MenuError::ExtraCharacters:
                    std::cerr << "Invalid input. Please enter a number with no extra characters: ";
                    break;
                case MenuError::InvalidSelection:
                    std::cerr << "Invalid selection. Please choose an option from the menu: ";
                    break;
                default:
                    std::cerr << "Unknown error. How did you even get here? Retry selection: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
            }
        }
        return selection;
	}
};

