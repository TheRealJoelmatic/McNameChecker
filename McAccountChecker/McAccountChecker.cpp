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
#include <direct.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <winuser.h>

// starting/menu
void menu(std::string message);
void startCheck(std::string mode);

// Main functions
void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads);
void CheckProxys(const std::vector<std::string>& proxyslines, int numThreads);
void scapeProxys();

// Api Calls
void sendAPI(const std::vector<std::string>& proxyslines, std::string username);
void SendProxyRequest(const std::vector<std::string>& proxyslines);

// Quick functions
void createFolder(const std::string& filePath);
bool createFile(const std::string& filePath);
bool fileExists(const std::string& filepath);

// Pretty functions
void logo();
void printCenter(const std::string& text);
void clearLine(int line);
void updateLine(const std::string& line, int lineNumber);
void remove_scrollbar();
void setConsoleBackground(int r, int g, int b, int textR, int textG, int textB);

// What to do with the api resonance
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
static size_t DiscardResponseCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::vector<std::string> getProxiesFromURL(const std::string& url);

// Text files
std::string accounts = "data/accounts/accounts.txt";
std::string Taccounts = "data/accounts/Taccounts.txt";
std::string Faccounts = "data/accounts/Faccounts.txt";
std::string proxys = "data/proxy/proxys.txt";
std::string Wproxys = "data/proxy/Wproxys.txt";
std::string Sproxys = "data/proxy/scapProxys.txt";

// Combined dir
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
bool firstTime;

//Threads things
std::mutex mtx;
std::atomic<bool> stopProcessing(false);

//----------------------------
//
// starting/menu
//
//----------------------------
int wmain()
{
    // Whether to play the start animation
    firstTime = true;

    int width = 950;   // Desired width
    int height = 500;  // Desired height
    const char* title = ""; // app title

    std::wstring wideTitle(title, title + strlen(title)); // format the title

    HWND console = GetConsoleWindow();
    RECT R; // react

    GetWindowRect(console, &R);

    LONG style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_THICKFRAME; // the thing where you can drag the bottom and sides
    style &= ~WS_MAXIMIZEBOX; // the box

    SetWindowLong(console, GWL_STYLE, style); // remove resizing (It causes random memory crashes otherwise)
    MoveWindow(console, R.left, R.top, width, height, TRUE); // set the size
    SetConsoleTitleW(wideTitle.c_str()); // change title

    setConsoleBackground(39, 24, 109, 0, 0, 0); // Set the background color to blue
    remove_scrollbar();

    system("cls");

    // Any starting logic
    startCheck("start");
    menu("Thanks for Downloading");

    return 0;
}

void menu(std::string message) {

    //Rest a bunch of values
    proxyErrors = 0;
    proxySucesses = 0;
    takenNames = 0;
    freeNames = 0;
    totalLoop = 0;
    proxyTimeouts = 0;
    proxySuccesses = 0;
    numConsole = 0;
    std::string menuOption;

    system("cls");

    //The cool animation
    if (firstTime) {
        std::cout << "\033[8;0H";
        printCenter(R"DELIMITER(_______________________________________________________________)DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[6;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(\_|  |_/\__,_|\__|_|\___|  \____/_| |_|\___|\___|_|\_\___|_|   )DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[5;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(| |  | | (_| | |_| | (__  | \__/\ | | |  __/ (__|   <  __/ |   )DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[4;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(| |\/| |/ _` | __| |/ __| | |   | '_ \ / _ \/ __| |/ / _ \ '__|)DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[3;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(| .  . | __ _| |_ _  ___  | /  \/ |__   ___  ___| | _____ _ __ )DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[2;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(|  \/  |     | | (_)      /  __ \ |             | |            )DELIMITER");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[1;0H";
        std::cout << "\n";
        printCenter(R"DELIMITER(___  ___      _   _        _____ _               _             )DELIMITER");
        std::cout << "\033[9;0H";
        firstTime = false;
    }
    else
    {
        logo();
    }

    //The rest of the ui
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

    //What tool is chosen 
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

    // Make a the folders that are needed
    createFolder("/data");
    createFolder("/data/accounts");
    createFolder("/data/proxy");

    //Check all the files if we need to make some
    logo();
    printCenter(R"DELIMITER(Checking if ACCOUNT file exists)DELIMITER");
    fileExists(accounts);
    system("cls");

    printCenter(R"DELIMITER(Checking if PROXY file exists)DELIMITER");
    fileExists(proxys);
    system("cls");

    printCenter(R"DELIMITER(Checking if PROXY file exists)DELIMITER");
    fileExists(Wproxys);
    system("cls");

    printCenter(R"DELIMITER(Checking if FreeAccounts file exists)DELIMITER");
    fileExists(Faccounts);
    system("cls");

    printCenter(R"DELIMITER(Checking if PROXYSCAPER file exists)DELIMITER");
    fileExists(Sproxys);
    system("cls");

    // check what mode is chosen
    if (mode == "WSTP") {
        scapeProxys();
        return;
    }
    else if (mode == "start")
    {
        return;
    }

    logo();

    //Questions about threads and buffer
    int numThreads;
    std::string numThreadsStr;
    if (mode == "CMN" || mode == "CTPU") {
        while (true) {
            numThreadsStr = "";

            if (mode == "CMN")printCenter("Enter the number of threads to use (Recommend 500 - 1000): ");
            else printCenter("Enter the number of threads to use (Recommend 500): ");
            printCenter(R"DELIMITER(_______________________________________________________________)DELIMITER");

            std::cin >> numThreadsStr;

            bool isNumber = true;
            for (char c : numThreadsStr) {
                if (!std::isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber && !numThreadsStr.empty()) {
                int numThreads = std::stoi(numThreadsStr);
                if (numThreads <= 0) {
                    system("cls");
                    logo();
                    printCenter("Invalid input. You must have more than 0 threads");
                }
                else {
                    break;
                }
            }
            else {
                system("cls");
                logo();
                printCenter("Invalid input. Only numbers are allowed.");
            }
        }
        numThreads = std::stoi(numThreadsStr);
    }
    system("cls");
    logo();

    std::string numBufferStr;
    if (mode == "CMN" || mode == "CTPU") {
        while (true) {
            numConsole = 0;
            std::string numBufferStr;

            printCenter("The more threads, the higher this value should be.");
            printCenter("Here are the recommended values:");
            printCenter("1 - 50 threads (10 buffer)");
            printCenter("100 - 500 threads (100 buffer)");
            if (mode == "CMN")
                printCenter("1000 - 2000 threads (200 buffer)");
            printCenter("Enter CONSOLE BUFFER");
            printCenter(R"DELIMITER(_______________________________________________________________)DELIMITER");

            std::cin >> numBufferStr;

            bool isNumber = true;
            for (char c : numBufferStr) {
                if (!std::isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber && !numBufferStr.empty()) {
                try {
                    numConsole = std::stoi(numBufferStr);
                    if (numConsole <= 0) {
                        system("cls");
                        logo();
                        printCenter("Please write a valid input!");
                    }
                    else {
                        break;
                    }
                }
                catch (const std::invalid_argument& e) {
                    system("cls");
                    logo();
                    printCenter("Invalid input. Only numbers are allowed.");
                }
            }
            else {
                system("cls");
                logo();
                printCenter("Invalid input. Only numbers are allowed.");
            }
        }
    }

    // Read the proxy and account files
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


    system("cls");

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

//----------------------------
//
// request API Calls
//
//----------------------------

void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads)
{
    printCenter("Starting");
    system("cls");
    logo();
    std::string currentProxy;
    int updateConsles = 0;

    auto start = std::chrono::steady_clock::now();

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

                auto end = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();

                std::cout << "Running it up for " << accountsline << std::endl; //8
                std::cout << "On them proxy's " << std::endl;
                std::cout << "-----------------------------" << std::endl;
                std::cout << "Account Names: " << accountslines.size() << std::endl; //11
                std::cout << "Proxy List: " << proxyslines.size() << std::endl; //12
                std::cout << "Total Loops: " << totalLoop << std::endl; //13
                std::cout << "Time Elapsed: " << duration << " Mins" << std::endl; //13
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

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();

    menu("Fished Checking accounts in " + std::to_string(duration) + " Mins");
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
void scapeProxys() {
    std::wstring executableDirectory = getExecutableDirectory();
    std::wstring Sproxys = L"\\data\\proxy\\scapProxys.txt";
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

//----------------------------
//
// SEND API REQUESTS
//
//----------------------------

void sendAPI(const std::vector<std::string>& proxyslines, std::string username)
{
    std::wstring executableDirectory = getExecutableDirectory();
    std::wstring wproxys = L"\\data\\accounts\\Faccounts.txt";
    wproxys = executableDirectory + wproxys;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8Wproxys = converter.to_bytes(wproxys);

    std::ofstream file(wproxys, std::ios::app);
    if (!file.is_open()) {
        menu("Error opening the file.");
        return;
    }


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
                    file << username << '\n';
                    file.close();
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
void SendProxyRequest(const std::vector<std::string>& proxyslines) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, proxyslines.size() - 1);

    int randomIndex = dis(gen);

    std::wstring executableDirectory = getExecutableDirectory();
    std::wstring wproxys = L"\\data\\proxy\\Wproxys.txt";
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

//----------------------------
//
// Quick functions
//
//----------------------------

bool createFile(const std::string& filePath)
{
    std::ofstream file(filePath.c_str());
    return file.good();
}
void createFolder(const std::string& filePath) {
    std::wstring executableDirectory = getExecutableDirectory();
    std::string CheckDir = std::string(executableDirectory.begin(), executableDirectory.end());

    const char* folderPath = filePath.c_str();
    int result = _mkdir(folderPath);
    std::string fullPath = CheckDir + folderPath;

    result = _mkdir(fullPath.c_str());

    printCenter(fullPath.c_str());
    if (result == 0) {
        std::cout << "Folder created successfully." << std::endl;
    }
    else {
        char errorMessage[100];
        strerror_s(errorMessage, sizeof(errorMessage), errno);
        std::cerr << "Failed to create folder. Error message: " << errorMessage << std::endl;
    }
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
std::wstring getExecutableDirectory()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
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

//----------------------------
//
// What to do with the api resonance
//
//----------------------------

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

//----------------------------
//
// Pretty functions
//
//----------------------------

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
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void remove_scrollbar()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(handle, &info);
    COORD new_size =
    {
        info.srWindow.Right - info.srWindow.Left + 1,
        info.srWindow.Bottom - info.srWindow.Top + 1
    };
    SetConsoleScreenBufferSize(handle, new_size);
}
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
void setConsoleBackground(int r, int g, int b, int textR, int textG, int textB) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(console, &csbi);

    csbi.ColorTable[0] = RGB(r, g, b); // Set the first color in the table to the specified RGB value

    SetConsoleScreenBufferInfoEx(console, &csbi);

    // Set the background color attribute without affecting the text color
    csbi.wAttributes &= 0xFFF0; // Preserve the existing text color
    csbi.wAttributes |= 0x0001; // Set the background color to the new value

    // Set the text color
    csbi.wAttributes &= 0xFF00; // Preserve the existing background color
    csbi.wAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Set the text color to white

    SetConsoleTextAttribute(console, csbi.wAttributes);
}
