// Work generation for the apriori@home project
// Jack Margeson, 07/22/2024

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <chrono>
#include <filesystem>
#include <argparse/argparse.hpp>

#include "boinc_api.h"
#include "filesys.h"
#include "util.h"

// Type definitions
typedef std::set<std::string> Items;
typedef std::map<std::string, std::vector<std::string>> Transactions;

// Global variable declarations
// transactions: map of transactions
Transactions transactions;
// items: set of unique items existing in all transactions
Items items;

void printTransactions(Transactions t)
{
    if (!t.empty())
    {
        for (const auto &transaction : t)
        {
            std::cout << "ID: " << transaction.first << "\tItems: {";
            bool first = true;
            for (const auto &transactionItem : transaction.second)
            {
                if (!first)
                    std::cout << ", ";
                std::cout << transactionItem;
                first = false;
            }
            std::cout << "}" << std::endl;
        }
    }
}

int saveItemList(Items i, std::string filename)
{
    if (!i.empty())
    {
        std::ofstream outputFile;

        try
        {
            outputFile.open(filename);
        }
        catch (const std::exception &err)
        {
            std::cerr << err.what() << std::endl;
            return 1;
        }
        if (outputFile.is_open())
        {
            bool first = true;
            for (auto item : items)
            {
                if (!first)
                    outputFile << "\n";
                outputFile << item;
                first = false;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    // Parse command line arguments.
    // --help: ignore all other arguments, print help menu
    // --dry-run: boolean, does everything except actually creating work units.
    // --threshold c: int, set a threshold of supporting transactions for the Apriori algorithm
    // --verbose: boolean, sets verbose mode
    // in: input file path for a list of transactions
    // out: output file path (for n/a)

    argparse::ArgumentParser program("work_generator", "v1.0.0");
    program.add_argument("-d", "--dry-run")
        .help("Execute all steps of the work generator, except for actually creating the work units through the BOINC API.")
        .flag();
    program.add_argument("-v", "--verbose")
        .help("Increase the verbosity of logging.")
        .flag();
    program.add_argument("-c", "--chunk-size")
        .help("Set the chunk size of items to partition for each work unit for candidate generation.")
        .default_value(100) // TODO: research default chunk size
        .nargs(1)
        .required()
        .scan<'i', int>();
    program.add_argument("-t", "--threshold")
        .help("Set the threshold of supporting transactions for the Apriori algorithm.")
        .default_value(0) // TODO: research default threshold
        .nargs(1)
        .required()
        .scan<'i', int>();
    program.add_argument("input")
        .help("Input file path containing a list of transactions.");
    program.add_argument("output")
        .help("Output file path containing [n/a].");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // Get command line inputs for program.
    // bool dryRun = program.get<bool>("dry-run");
    bool verbose = program.get<bool>("verbose");
    int chunkSize = program.get<int>("chunk-size");
    // int transactionsThreshold = program.get<int>("threshold");
    auto transactionsPath = program.get<std::string>("input");
    // auto programOutputPath = program.get<std::string>("output");

    // 1. From a list of transactions, create a list of all items.
    if (verbose)
        std::cout << "work_generator.cpp | Parsing transactions file..." << std::endl;
    if (!transactionsPath.empty())
    {
        std::ifstream transactionsStream;
        try
        {
            transactionsStream.open(transactionsPath.c_str());

            if (transactionsStream.is_open())
            {
                // 2. Partition the list of items into sizable chunks, defined as an argument.
                std::string line;

                while (std::getline(transactionsStream, line))
                {
                    std::string transactionID;
                    // Split line by ; delimiter
                    int delimiterPos = line.find(';');
                    transactionID = line.substr(0, delimiterPos);
                    line.erase(0, delimiterPos + 1);
                    // Split rest of line by , delimiter
                    while (line.find(',') != std::string::npos)
                    {
                        delimiterPos = line.find(',');
                        std::string item = line.substr(0, delimiterPos);
                        // Add item to transactions map
                        transactions[transactionID].push_back(item);
                        // Add item to unique items set
                        items.insert(item);
                        // Erase item from line
                        line.erase(0, delimiterPos + 1);
                    }
                }

                // printTransactions(transactions);
            }
        }
        catch (const std::exception &err)
        {
            std::cerr << err.what() << std::endl;
            std::cerr << program;
            return 1;
        }
    }

    // 3. Save the item chunk lists to individual, uniquely named files.
    // Get epoch timestamp
    if (verbose)
        std::cout << "work_generator.cpp | Getting epoch timestamp..." << std::endl;
    const auto currTime = std::chrono::system_clock::now();
    const auto secSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(currTime.time_since_epoch()).count();
    if (verbose)
        std::cout << "work_generator.cpp | Epoch timestamp: " << secSinceEpoch << std::endl;
    // Create directory for chunk files
    try
    {
        std::filesystem::create_directory("./item_chunks_" + std::to_string(secSinceEpoch));
        if (verbose)
            std::cout << "work_generator.cpp | Created directory ./item_chunks_" << secSinceEpoch << std::endl;
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }
    // For each item, write into seperate files according to the chunk size
    int itemRemainder = static_cast<int>(items.size()) % chunkSize;
    int fileIndex = 1;
    Items::iterator itemsIterator = items.begin();

    if (verbose)
        std::cout << "work_generator.cpp | Writing unique items into file chunks (size " << chunkSize << ")..." << std::endl;
    while (fileIndex <= (static_cast<int>(items.size()) / chunkSize) + 1)
    {
        std::ofstream outputFile;
        try
        {
            outputFile.open("./item_chunks_" + std::to_string(secSinceEpoch) + "/item_data_" + std::to_string(fileIndex) + ".apr");
        }
        catch (const std::exception &err)
        {
            std::cerr << err.what() << std::endl;
            std::cerr << program;
            return 1;
        }
        if (outputFile.is_open())
        {
            bool first = true;
            int stopIterator = -1;

            // If the file index is larger than the number of items divided by the chunk size,
            // this is the remainter of the items that should go in their own file. This will
            // end up being a smaller chunk than the other files. Otherwise, the stop iterator
            // should just be the chunk size.
            if (fileIndex > static_cast<int>(items.size()) / chunkSize)
                stopIterator = itemRemainder;
            else
                stopIterator = chunkSize;

            for (int i = 0; i <= stopIterator - 1; i++)
            {
                if (!first)
                    outputFile << "\n";
                outputFile << *itemsIterator;
                itemsIterator = std::next(itemsIterator);
                first = false;
            }
        }
        fileIndex++;
    }

    // 4. Use the BOINC API to schedule tasks.
    // 5. Wait for the tasks to finish.
    // 6. Consolitate answers from tasks, combine same sets.
    // 7. Filter sets that do not meet threshold from the list of transactions.
    // 8. Repeat steps 1-6 until we've run out of sets that are above the relevancy threshold or we've hit the number of max sets.
    // 9. Algorithm is finished. Create file reporting findings.

    // Save item list for testing
    if (verbose)
        std::cout << "work_generator.cpp | Saving full item list as items_" << secSinceEpoch << ".apr..." << std::endl;
    saveItemList(items, "items_" + std::to_string(secSinceEpoch) + ".apr");

    return 0;
}
