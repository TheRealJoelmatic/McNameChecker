#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <random>
#include <codecvt>

#include <Windows.h>

#include <curl/curl.h>
#include "rapidjson/document.h"

void menu(std::string message);
void startCheck(std::string mode);
void logo();
void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads);
void sendAPI(const std::vector<std::string>& proxyslines, std::string username);
void printCenter(const std::string& text);
void clearLine(int line);
void CheckProxys(const std::vector<std::string>& proxyslines, int numThreads);
void SendProxyRequest(const std::vector<std::string>& proxyslines);
void updateLine(const std::string& line, int lineNumber);
void scapeProxys();
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
std::vector<std::string> getProxiesFromURL(const std::string& url);
static size_t DiscardResponseCallback(void* contents, size_t size, size_t nmemb, void* userp);
bool fileExists(const std::string& filepath);
bool createFile(const std::string& filePath);

std::string accounts = "accounts.txt";
std::string Taccounts = "Taccounts.txt";
std::string Faccounts = "Faccounts.txt";
std::string proxys = "proxys.txt";
std::string Wproxys = "Wproxys.txt";
std::string Sproxys = "scapProxys.txt";
std::string accountsCheckDir;
std::string proxysCheckDir;
std::string currentProxy;
std::wstring getExecutableDirectory();


// Shared variables
int proxyErrors = 0;
int proxySucesses = 0;
int takenNames = 0;
int freeNames = 0;
int totalLoop = 0;
int proxyTimeouts = 0;
int proxySuccesses = 0;
int numConsole;
bool proxyWorking;
bool accountFound;

std::mutex mtx;
std::atomic<bool> stopProcessing(false);

int wmain()
{
    HWND console = GetConsoleWindow();

    RECT r;
    GetWindowRect(console, &r);
    LONG style = GetWindowLong(console, GWL_STYLE);

    style &= ~WS_THICKFRAME;
    style &= ~WS_MAXIMIZEBOX;

    SetWindowLong(console, GWL_STYLE, style);

    int width = 950;   // Desired width
    int height = 500;  // Desired height
    MoveWindow(console, r.left, r.top, width, height, TRUE);
    // Any starting logic
    startCheck("start");
    menu("Thanks for Downloading");
    return NULL;
}

void menu(std::string message) {
    system("cls");

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hConsole, &csbi); // Get current console screen buffer information

    csbi.ColorTable[0] = RGB(0, 0, 128); // Set the first color in the table to blue (RGB value)

    // Update the console screen buffer with the new color table
    SetConsoleScreenBufferInfoEx(hConsole, &csbi);

    proxyErrors = 0;
    proxySucesses = 0;
    takenNames = 0;
    freeNames = 0;
    totalLoop = 0;
    proxyTimeouts = 0;
    proxySuccesses = 0;
    numConsole = 0;

    std::string menuOption;

    logo();

    printCenter(R"DELIMITER(_______________________________)DELIMITER");
    printCenter(R"DELIMITER(/                               \)DELIMITER");
    printCenter(message);
    printCenter(R"DELIMITER(\_______________________________/)DELIMITER");
    printCenter(R"DELIMITER(_____________________________________________________|________________________________________________________)DELIMITER");
    printCenter(R"DELIMITER(               |                                     |                                      |                 )DELIMITER");

    printCenter(R"DELIMITER( _____________[1]_______________       _____________[2]_______________        _____________[3]_______________)DELIMITER");
    printCenter(R"DELIMITER( /                               \     /                               \      /                               \)DELIMITER");
    printCenter(R"DELIMITER( |      Check Minecraft Names    |     |      Check Them Proxies Up    |      |     Web Scape  Them Proxies   |)DELIMITER");
    printCenter(R"DELIMITER( \_______________________________/     \_______________________________/      \_______________________________/)DELIMITER");

    std::cout << "\n";
    printCenter(R"DELIMITER(Choose An Option)DELIMITER");
    printCenter(R"DELIMITER()DELIMITER");
    std::cin >> menuOption;
    if (menuOption == "1")
    {
        startCheck("CMN");
    }
    else if (menuOption == "2")
    {
        startCheck("CTPU");
    }
    else if (menuOption == "3")
    {
        startCheck("WSTP");
    }
    else
    {
        menu("Invaded option!");
    }
}

void startCheck(std::string mode) {
    system("cls");

    logo();
    printCenter(R"DELIMITER(Checking if ACCOUNT file exists)DELIMITER");
    fileExists(accounts);
    system("cls");
    logo();

    printCenter(R"DELIMITER(Checking if PROXY file exists)DELIMITER");
    fileExists(proxys);
    system("cls");
    logo();

    printCenter(R"DELIMITER(Checking if PROXY file exists)DELIMITER");
    fileExists(Wproxys);
    system("cls");
    logo();

    printCenter(R"DELIMITER(Checking if FreeAccounts file exists)DELIMITER");
    fileExists(Faccounts);
    system("cls");
    logo();

    printCenter(R"DELIMITER(Checking if PROXYSCAPER file exists)DELIMITER");
    fileExists(Sproxys);
    system("cls");
    logo();
  

    system("cls");
    logo();

    std::wstring executableDirectory = getExecutableDirectory();
    std::string accountsCheckDir = std::string(executableDirectory.begin(), executableDirectory.end()) + "\\" + accounts;
    printCenter(accountsCheckDir);

    std::ifstream file(accountsCheckDir);
    std::vector<std::string> accountslines; // Vector to store the lines of the file

    if (file.is_open()) {
        std::string accountsline;
        while (std::getline(file, accountsline)) {
            accountslines.push_back(accountsline); // Add each line to the vector
        }
        file.close();
    }
    else {
        printCenter(R"DELIMITER(Unable to open the file.)DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        menu("Unable to open the file."); 
    }
    //for (const std::string& accountsline : accountslines) {
    //    printCenter(accountsline);
    //}

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    system("cls");
    logo();

    std::string proxysCheckDir = std::string(executableDirectory.begin(), executableDirectory.end()) + "\\" + proxys;
    printCenter(proxysCheckDir);

    std::ifstream proxysfile(proxysCheckDir);
    std::vector<std::string> proxyslines; // Vector to store the lines of the file

    if (proxysfile.is_open()) {
        std::string proxysline;
        while (std::getline(proxysfile, proxysline)) {
            proxyslines.push_back(proxysline); // Add each line to the vector
        }
        proxysfile.close();
    }
    else {
        printCenter(R"DELIMITER(Unable to open the file.)DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        menu("Unable to open the file."); // Exit the program if the file cannot be opened
    }
    //for (const std::string& proxysline : proxyslines) {
    //    printCenter(proxysline);
    //}

    //for (const std::string& accountsline : accountslines) {
    //    printCenter(accountsline);
    //}
    system("cls");
    if (mode == "WSTP") {
        scapeProxys();
        return;
    }
    else if (mode == "start")
    {
        return;
    }
    logo();

    int numThreads;
    if (mode == "CMN" || mode == "CTPU") {
        printCenter("Enter the number of threads to use (Recommend 500 - 1000): ");
        std::cin >> numThreads;

        system("cls");
        logo();
    }

    system("cls");
    logo();

    printCenter("Enter CONSOLE BUFFER");
    if (mode == "CMN" || mode == "CTPU") {
        printCenter("the more threads the higher this should be.");
        printCenter("Here are the recommend values");
        printCenter("1 - 50 threads (10 buffer)");
        printCenter("100 - 500 threads (100 buffer)");
        printCenter("1000+ threads (200 buffer)");
        printCenter("2000+ threads (400 buffer)");
    }
    else
    {
        printCenter("(Recommend for this is 1)");
    }
    std::cin >> numConsole;

    if (mode == "CMN") {
        processAccounts(accountslines, proxyslines, numThreads);
    }
    else if (mode == "CTPU") {
        CheckProxys(proxyslines, numThreads);
    }
    else {
        menu("An Error Has Happened");
    }
}

void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads)
{
    printCenter("Starting");
    system("cls");
    logo();

    std::string currentProxy;
    int updateConsles = 0;

    // Create a vector to hold the threads
    std::vector<std::thread> threads;
    accountFound = false;  // Flag to track if account is found
    for (const std::string& accountsline : accountslines) {
        accountFound = false;  // Reset the flag for each account
        int numLinesToUpdate = 9;
        for (const std::string& proxysline : proxyslines) {
            
            if (updateConsles == 0) {
                totalLoop++;
                system("cls");
                logo();
                std::cout << "Running it up for " << accountsline << std::endl; //8
                std::cout << "On them proxy's " << std::endl;
                std::cout << "-----------------------------" << std::endl;
                std::cout << "Account Names: " << accountslines.size() << std::endl; //11
                std::cout << "Proxy List: " << proxyslines.size() << std::endl; //12
                std::cout << "Total Loops: " << totalLoop << std::endl; //13
                std::cout << "-----------------------------" << std::endl;
                std::cout << "Taken Names: " << takenNames << std::endl; //15
                std::cout << "Free Names: " << freeNames << std::endl; //16
                std::cout << "Proxy Errors: " << proxyErrors << std::endl; //17
                std::cout << "Proxy Timeouts: " << proxyTimeouts << std::endl; //18
                std::cout << "-----------------------------" << std::endl;
                updateConsles++; currentProxy;
            }
            else
            {
                if (updateConsles > numConsole) {
                    updateConsles = 0;
                }
                else
                {
                    updateConsles++;
                }
            }

            if (!proxyWorking || proxysline != currentProxy) {
                currentProxy = proxysline;
                proxyWorking = true;
            }

            if (proxyWorking) {
                // Create a new thread for each API request
                threads.emplace_back([&]() {
                    sendAPI(proxyslines, accountsline);
                    });

                if (threads.size() >= numThreads) {
                    // Wait for the threads to finish
                    for (auto& thread : threads) {
                        thread.join();
                    }

                    // Clear the vector to reuse it for the next batch of threads
                    threads.clear();
                }

                if (accountFound) {
                    // Account found, break out of the inner loop
                    break;
                }

                if (proxyErrors > 0 || proxyTimeouts > 0) {
                    proxyWorking = false;
                }
            }
            if (accountFound) {
                // Account found, break out of the outer loop
                break;
            }
        }
    }

    // Wait for the remaining threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    threads.clear();

    menu("Fished");
}



void sendAPI(const std::vector<std::string>& proxyslines, std::string username)
{
    CURL* curl = curl_easy_init();

    if (curl)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, proxyslines.size() - 1);
        int randomIndex = dis(gen);
        std::string proxy = proxyslines[randomIndex];

        std::string response;
        std::string url = "https://api.mojang.com/users/profiles/minecraft/" + username;


        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
        currentProxy = proxy;
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        rapidjson::Document document;
        if (!response.empty())
        {
            rapidjson::Document document;
            document.Parse(response.c_str());
            stopProcessing = true;
            accountFound = true;

            // Check if the document is valid JSON
            if (!document.HasParseError())
            {
                std::lock_guard<std::mutex> lock(mtx);
                // Check if "id" field exists
                if (document.HasMember("id") && document["id"].IsString())
                {
                    std::string id = document["id"].GetString();
                    takenNames++;
                }
                else
                {
                    freeNames++;
                }
            }
        }

        if (res == CURLE_OPERATION_TIMEDOUT)
        {
            proxyTimeouts++;
        }
        else
        {
            proxyErrors++;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        // Failed to initialize Curl handle
        std::cout << "Failed to initialize Curl handle" << std::endl;
        proxyErrors++;
    }
}

void CheckProxys(const std::vector<std::string>& proxyslines, int numThreads) {

    system("cls");
    logo();
    int updateConsles = 0;
    std::vector<std::thread> threads;
    proxyTimeouts = 1;

    CURL* curl = curl_easy_init();
    for (const std::string& proxysline : proxyslines) {
        if (updateConsles == 0) {
            totalLoop++;
            system("cls");
            logo();

            std::cout << "Running it up for " << proxysline << std::endl;
            std::cout << "_____________________________" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Progress: " << proxySuccesses + proxyTimeouts << "/" << proxyslines.size() << std::endl;
            std::cout << "Proxy Working: " << proxySuccesses << std::endl;
            std::cout << "Proxy Timeouts: " << proxyTimeouts << std::endl;
            std::cout << "_____________________________" << std::endl;
            updateConsles++; currentProxy;
        }
        else {
            if (updateConsles > numConsole) {
                updateConsles = 0;
            }
            else {
                updateConsles++;
            }
        }
        //threads and 

        threads.emplace_back([&]() {
            SendProxyRequest(proxyslines);
            });

        if (threads.size() >= numThreads) {
            // Wait for the threads to finish
            for (auto& thread : threads) {
                thread.join();
            }

            // Clear the vector to reuse it for the next batch of threads
            threads.clear();
        }
     
    }

    curl_easy_cleanup(curl);
    
    menu("Complete Saved all the working proxys in Wproxys.txt");
}

void SendProxyRequest(const std::vector<std::string>& proxyslines) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, proxyslines.size() - 1);

    int randomIndex = dis(gen);

    std::wstring executableDirectory = getExecutableDirectory();
    std::wstring wproxys = L"\\Wproxys.txt";
    wproxys = executableDirectory + wproxys;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8Wproxys = converter.to_bytes(wproxys);

    std::ofstream file(wproxys, std::ios::app);
    if (!file.is_open()) {
        menu("Error opening the file.");
        return;
    }

    CURL* curl = curl_easy_init();
    if (curl) {
        std::string proxy = proxyslines[randomIndex];
        std::string response;
        std::string url = "https://api.mojang.com/users/profiles/minecraft/";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OPERATION_TIMEDOUT) {
            proxyTimeouts++;
        }
        else {
            proxySuccesses++;
            file << proxy << '\n';
            file.close();
        }

        curl_easy_cleanup(curl);
    }
    else {
        // Failed to initialize Curl handle
        std::cout << "Failed to initialize Curl handle" << std::endl;
        proxyErrors++;
    }
}
std::vector<std::string> loadProxyLines()
{
    std::vector<std::string> proxyslines;

    // Code to load proxy lines from a file
    std::string proxysCheckDir = "./proxys.txt";
    std::ifstream proxysfile(proxysCheckDir);

    if (proxysfile.is_open()) {
        std::string proxysline;
        while (std::getline(proxysfile, proxysline)) {
            proxyslines.push_back(proxysline);
        }
        proxysfile.close();
    }
    else {
        printCenter("\nUnable to open the file. (Make sure you have some proxies in there)");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        exit(0);
    }

    return proxyslines;
}
std::vector<std::string> loadAccountLines()
{
    std::vector<std::string> accountslines;

    // Code to load account lines from a file
    std::string accountsCheckDir = "./accounts.txt";
    std::ifstream file(accountsCheckDir);

    if (file.is_open()) {
        std::string accountsline;
        while (std::getline(file, accountsline)) {
            accountslines.push_back(accountsline);
        }
        file.close();
    }
    else {
        printCenter("Unable to open the file. (Make sure you have some names in there)");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        exit(0);
    }

    return accountslines;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

static size_t DiscardResponseCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    return size * nmemb;
}

bool fileExists(const std::string& filepath)
{

    std::wstring executableDirectory = getExecutableDirectory();
    std::replace(executableDirectory.begin(), executableDirectory.end(), L'\\', L'/');

    std::string CheckDir = std::string(executableDirectory.begin(), executableDirectory.end()) + "/" + filepath;
    std::ifstream file(CheckDir);
    if (file.good()) {
        printCenter("\nFOUND at " + CheckDir);
        return file.good();
    }
    else
    {


        // make the file
        printCenter("\n\nCould not find! creating one now at " + CheckDir);
        if (createFile(CheckDir)) {
            printCenter("\n\nFile created successfully" + CheckDir);
        }
        else {
            printCenter("\n\nFailed to create file:" + CheckDir);
            menu("Could not make a file in" + CheckDir);
        }
    }
    return file.good();
}

void scapeProxys() {
    std::wstring executableDirectory = getExecutableDirectory();
    std::wstring Sproxys = L"\\scapProxys.txt";
    Sproxys = executableDirectory + Sproxys;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8Wproxys = converter.to_bytes(Sproxys);

    std::ofstream file(Sproxys, std::ios::app);
    if (!file.is_open()) {
        menu("Error opening the file.");
        return;
    }
    std::vector<std::string> proxies = getProxiesFromURL("https://gist.githubusercontent.com/TheRealJoelmatic/d0c908aad0eeb3723f933e7532d89cac/raw/f95087fb9b5ec3bf886b7a8733c1d45b4447fdeb/gistfile1.txt");
    for (const auto& proxy : proxies) {
        std::cout << proxy << std::endl;
        file << proxy << '\n';
    }
    file.close();
    menu("Done: " + std::to_string(proxies.size()) + " Proxys scaped!");
}

std::vector<std::string> getProxiesFromURL(const std::string& url) {
    std::vector<std::string> proxies;
    std::string response;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to fetch URL: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl" << std::endl;
    }

    // Process the response and extract proxies
    size_t startPos = 0;
    size_t endPos = response.find('\n');

    while (endPos != std::string::npos) {
        std::string proxy = response.substr(startPos, endPos - startPos);
        proxies.push_back(proxy);

        startPos = endPos + 1;
        endPos = response.find('\n', startPos);
    }

    return proxies;
}


bool createFile(const std::string& filePath)
{
    std::ofstream file(filePath.c_str());
    return file.good();
}

std::wstring getExecutableDirectory()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}


//----------------------------
//
//Make it look cool
//
//----------------------------

void printCenter(const std::string& text) {
    // Get the console window size
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    // Calculate the padding on each side
    int textLength = static_cast<int>(text.length());
    int padding = (consoleWidth - textLength) / 2;

    // Build the centered string with padding
    std::string centeredText;
    centeredText.append(padding, ' ');
    centeredText.append(text);

    // Print the centered text

    std::cout << centeredText << std::endl;
}

void logo() {
    printCenter(R"DELIMITER(___  ___      _   _        _____ _               _             )DELIMITER");
    printCenter(R"DELIMITER(|  \/  |     | | (_)      /  __ \ |             | |            )DELIMITER");
    printCenter(R"DELIMITER(| .  . | __ _| |_ _  ___  | /  \/ |__   ___  ___| | _____ _ __ )DELIMITER");
    printCenter(R"DELIMITER(| |\/| |/ _` | __| |/ __| | |   | '_ \ / _ \/ __| |/ / _ \ '__|)DELIMITER");
    printCenter(R"DELIMITER(| |  | | (_| | |_| | (__  | \__/\ | | |  __/ (__|   <  __/ |   )DELIMITER");
    printCenter(R"DELIMITER(\_|  |_/\__,_|\__|_|\___|  \____/_| |_|\___|\___|_|\_\___|_|   )DELIMITER");
    printCenter(R"DELIMITER(_______________________________________________________________)DELIMITER");
    std::cout << "\n";
}

// Function to set the cursor position
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Function to clear a specific line
void clearLine(int line) {
    gotoxy(0, line);
    std::cout << std::string(80, ' ');
    gotoxy(0, line);
}
void updateLine(const std::string& line, int lineNumber) {
    // Move the cursor to the beginning of the line
    std::cout << "\033[" << lineNumber << ";1H";
    std::cout << line << std::endl;
}