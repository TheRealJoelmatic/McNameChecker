#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include <algorithm>
#include <mutex>
#include <random>

void menu(std::string message);
void startCheck();
void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads);
void sendAPI(const std::vector<std::string>& proxyslines, std::string username);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
static size_t DiscardResponseCallback(void* contents, size_t size, size_t nmemb, void* userp);
bool fileExists(const std::string& filepath);
bool createFile(const std::string& filePath);

std::string accounts = "Data/Accounts/accounts.txt";
std::string Taccounts = "Data/Accounts/Taccounts.txt";
std::string Faccounts = "Data/Accounts/Faccounts.txt";
std::string proxys = "Data/Proxys/proxys.txt";
std::string accountsCheckDir;
std::string proxysCheckDir;
std::wstring getExecutableDirectory();


// Shared variables
int proxyErrors = 0;
int proxySucesses = 0;
int takenNames = 0;
int freeNames = 0;
int totalLoop = 0;
int proxyTimeouts = 0;
int proxySuccesses = 0;
bool proxyWorking;
bool accountFound;

std::mutex mtx;
std::atomic<bool> stopProcessing(false);

int wmain()
{
    // Any starting logic
    menu("");
    return NULL;
}

void menu(std::string message) {
    system("cls");
    proxyErrors = 0;
    proxySucesses = 0;
    takenNames = 0;
    freeNames = 0;
    totalLoop = 0;
    proxyTimeouts = 0;
    proxySuccesses = 0;

    std::string menuOption;

    std::cout << message;
    std::cout << "Matic MC Account Name Checker\n";
    std::cout << "\n 1 - Check Names";
    std::cout << "\nWhat would you like to run? : ";
    std::cin >> menuOption;
    if (menuOption == "1" || menuOption == "check names")
    {
        startCheck();
    }
    else
    {
        menu("Invaded option!");
    }
}

void startCheck() {
    system("cls");

    std::cout << "Checking if ACCOUNT file exists \n";
    fileExists(accounts);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    system("cls");

    std::cout << "Checking if PROXY file exists \n";
    fileExists(proxys);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    system("cls");

    std::cout << "Checking if TakenAccounts file exists \n";
    fileExists(Taccounts);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    system("cls");

    std::cout << "Checking if FreeAccounts file exists \n";
    fileExists(Faccounts);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    system("cls");

  

    system("cls");

    std::wstring executableDirectory = getExecutableDirectory();
    std::string accountsCheckDir = std::string(executableDirectory.begin(), executableDirectory.end()) + "\\" + accounts;
    std::cout << accountsCheckDir + "\n";

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
        std::cout << "\nUnable to open the file. (Make sure you have some names in there)" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        exit(0); // Exit the program if the file cannot be opened
    }
    for (const std::string& accountsline : accountslines) {
        std::cout << accountsline << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    system("cls");


    std::string proxysCheckDir = std::string(executableDirectory.begin(), executableDirectory.end()) + "\\" + proxys;
    std::cout << proxysCheckDir + "\n";

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
        std::cout << "\nUnable to open the file. (Make sure you have some names in there)" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        exit(0); // Exit the program if the file cannot be opened
    }
    for (const std::string& proxysline : proxyslines) {
        std::cout << proxysline << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    for (const std::string& accountsline : accountslines) {
        std::cout << accountsline << std::endl;
    }
    system("cls");

    std::cout << "Enter the number of threads to use (Recommend 500 - 1000): ";
    int numThreads;
    std::cin >> numThreads;

    processAccounts(accountslines, proxyslines, numThreads);
}

void processAccounts(const std::vector<std::string>& accountslines, const std::vector<std::string>& proxyslines, int numThreads)
{
    std::cout << "Starting" << std::endl;
    std::string currentProxy;

    // Create a vector to hold the threads
    std::vector<std::thread> threads;
    accountFound = false;  // Flag to track if account is found

    for (const std::string& accountsline : accountslines) {
        accountFound = false;  // Reset the flag for each account

        for (const std::string& proxysline : proxyslines) {
            system("cls");
            totalLoop++;
            std::cout << "\nRunning it up for " << accountsline << std::endl;
            std::cout << "On them proxy's " << std::endl;
            std::cout << "-----------------------------" << std::endl;
            std::cout << "Account Names: " << accountslines.size() << std::endl;
            std::cout << "Proxy List: " << proxyslines.size() << std::endl;
            std::cout << "Total Loops: " << totalLoop << std::endl;
            std::cout << "-----------------------------" << std::endl;
            std::cout << "Progress: " << (proxySuccesses + proxyErrors + proxyTimeouts) << "/" << proxyslines.size() * accountslines.size() << std::endl;
            std::cout << "Taken Names: " << takenNames << std::endl;
            std::cout << "Free Names: " << freeNames << std::endl;
            std::cout << "Proxy Errors: " << proxyErrors << std::endl;
            std::cout << "Proxy Timeouts: " << proxyTimeouts << std::endl;
            std::cout << "-----------------------------" << std::endl;
            std::cout << "\n";

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

    std::cout << "Finished" << std::endl;
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
        std::cout << "proxy: " << proxy<< std::endl;
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
        std::cout << "\nUnable to open the file. (Make sure you have some proxies in there)" << std::endl;
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
        std::cout << "\nUnable to open the file. (Make sure you have some names in there)" << std::endl;
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
        std::cout << "\n\nFOUND at " + CheckDir + "\n";
        return file.good();
    }
    else
    {
        std::cout << "\n\nCould not find! creating one now at " + CheckDir + "\n";
        if (createFile(CheckDir)) {
            std::cout << "\n\nFile created successfully" << CheckDir << std::endl;
        }
        else {
            std::cout << "\n\nFailed to create file: " << CheckDir << std::endl;
            menu("Could not make a file in" + CheckDir);
        }
    }
    return file.good();
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
