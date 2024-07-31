#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <string.h>
#include <argparse/argparse.hpp>

#include "boinc_api.h"
#include "filesys.h"
#include "util.h"

int threshold = 5;

// Type definitions
typedef std::vector<std::vector<std::string>> Candidates;
typedef std::map<std::string, std::vector<std::string>> Transactions;

// Global variable declarations
// transactions: map of transactions
Transactions transactions;
Candidates candidates;

void printTransactions(Transactions t)
{
    if (!t.empty())
    {
        for (const auto &transaction : t)
        {
            std::cout << "ID: " << transaction.first << "\tItems: {";
            bool first = true;
            for (const std::string &transactionItem : transaction.second)
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

void printCandidates(Candidates c)
{
    if (!c.empty())
    {
        int candidateCount = 0;
        for (const auto &candidate : c)
        {
            std::cout << "Candidate #" << candidateCount << ": {";
            bool first = true;
            for (const auto &candidateItem : candidate)
            {
                if (!first)
                    std::cout << ", ";
                std::cout << candidateItem;
                first = false;
            }
            std::cout << "}" << std::endl;
            candidateCount++;
        }
    }
}

void printItems(std::set<std::string> s)
{
    if (!s.empty())
    {
        std::cout << "Items: {";
        bool first = true;
        for (const auto &item : s)
        {
            if (!first)
                std::cout << ", ";
            std::cout << item;
            first = false;
        }
    }
    std::cout << "}" << std::endl;
}

int calculate_frequency(std::string transactions_file, std::string in, std::string out)
{
    char transactions_path[1024], input_path[1024], output_path[1024];
    char buf[256];
    char line[10240];

    // Resolve transactions filename
    boinc_resolve_filename(transactions_file.c_str(), transactions_path, sizeof(transactions_path));
    FILE *transfile = boinc_fopen(transactions_path, "r");
    if (!transfile)
    {
        fprintf(stderr,
                "%s Couldn't find input file, resolved name %s.\n",
                boinc_msg_prefix(buf, sizeof(buf)), input_path);
        return -1;
    }

    // Resolve input filename
    boinc_resolve_filename(in.c_str(), input_path, sizeof(input_path));
    FILE *infile = boinc_fopen(input_path, "r");
    if (!infile)
    {
        fprintf(stderr,
                "%s Couldn't find input file, resolved name %s.\n",
                boinc_msg_prefix(buf, sizeof(buf)), input_path);
        return -1;
    }

    // Resolve output filename
    boinc_resolve_filename(out.c_str(), output_path, sizeof(output_path));
    FILE *outfile = boinc_fopen(output_path, "w");
    if (!outfile)
    {
        fprintf(stderr,
                "%s Couldn't find output file, resolved name %s.\n",
                boinc_msg_prefix(buf, sizeof(buf)), output_path);
        fclose(infile);
        return -1;
    }

    std::vector<std::string> transaction_lines;
    // Add each transaction line to vector
    while (fgets(line, sizeof(line), transfile))
    {
        line[strcspn(line, "\n")] = 0;
        transaction_lines.push_back(line);
    }
    // Parse transactions vector
    for (const auto &lline : transaction_lines)
    {
        std::string strline = lline;
        std::string transactionID;
        // Split line by ; delimiter
        int delimiterPos = strline.find(';');
        transactionID = strline.substr(0, delimiterPos);
        strline.erase(0, delimiterPos + 1);
        // Split rest of line by , delimiter
        while (strline.find(',') != std::string::npos)
        {
            delimiterPos = strline.find(',');
            std::string item = strline.substr(0, delimiterPos);
            // Add item to transactions map
            transactions[transactionID].push_back(item);
            // Erase item from line
            strline.erase(0, delimiterPos + 1);
        }
        if ((strline.find(',') == std::string::npos) && strline.length() != 0)
        {
            transactions[transactionID].push_back(strline);
        }
        std::sort(transactions[transactionID].begin(), transactions[transactionID].end());
    }

    std::vector<std::string> candidate_lines;
    while (fgets(line, sizeof(line), infile))
    {
        line[strcspn(line, "\n")] = 0;
        candidate_lines.push_back(line);
    }
    // Process candidate lines
    for (const auto &lline : candidate_lines)
    {
        std::string strline = lline;
        std::vector<std::string> candidateItems;

        // Split line by , delimiter
        while (strline.find(',') != std::string::npos)
        {
            int delimiterPos = strline.find(',');
            std::string item = strline.substr(0, delimiterPos);
            // Add item to transactions map
            candidateItems.push_back(item);
            // Erase item from line
            strline.erase(0, delimiterPos + 1);
        }
        if ((strline.find(',') == std::string::npos) && strline.length() != 0)
        {
            candidateItems.push_back(strline);
        }

        // Sort canddiateItems list
        std::sort(candidateItems.begin(), candidateItems.end());
        // Push the candidateItems list to the candidates vector
        candidates.push_back(candidateItems);
    }

    // printTransactions(transactions);
    // printCandidates(candidates);

    // Calculate support for each candidate
    std::vector<int> support(candidates.size(), 0);
    for (int i = 0; i <= (int)(candidates.size() - 1); i++)
    {
        for (auto const &transaction : transactions)
        {
            // Every time that the candidate vector is a subset of the transaction vector,
            // we count it as a support for that candidate.
            if (std::includes(transaction.second.begin(), transaction.second.end(), candidates[i].begin(), candidates[i].end()))
            {
                support[i]++;
            }
        }
    }

    for (int i = 0; i <= (int)(candidates.size() - 1); i++)
    {
        bool first = true;
        std::cout << "Candidate #" << i << ": {";
        for (const auto &candidateItem : candidates[i])
        {
            if (!first)
                std::cout << ", ";
            std::cout << candidateItem;
            first = false;
        }
        std::cout << "}\t Support: " << support[i] << std::endl;

        // If the support for this candidate is higher than the relavancy threshold,
        // save this candidate to the output file.
        first = true;
        if (support[i] >= threshold)
        {
            for (const auto &candidateItem : candidates[i])
            {
                if (!first)
                    fprintf(outfile, ",");
                fprintf(outfile, "%s", candidateItem.c_str());
                first = false;
            }
            fprintf(outfile, "\n");
        }
    }

    fclose(transfile);
    fclose(infile);
    fclose(outfile);

    return 0;
}

int main(int argc, char *argv[])
{
    char buf[256];
    // Init. BOINC API
    BOINC_OPTIONS options;
    boinc_options_defaults(options);
    int retval = boinc_init_options(&options);
    if (retval)
    {
        fprintf(stderr, "%s boinc_init returned %d\n",
                boinc_msg_prefix(buf, sizeof(buf)), retval);
        exit(retval);
    }

    argparse::ArgumentParser program("calculate_frequency", "v1.0.0");
    program.add_argument("-t", "--threshold")
        .help("Set the threshold of supporting transactions for the Apriori algorithm.")
        .default_value(5) // TODO: research default threshold
        .nargs(1)
        .required()
        .scan<'i', int>();
    program.add_argument("-tr", "--transactions")
        .help("File path containing a list of transactions.")
        .default_value("transactions.dat")
        .required();
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

    auto transactionsPath = program.get<std::string>("transactions");
    auto programInputPath = program.get<std::string>("input");
    auto programOutputPath = program.get<std::string>("output");

    // transactions, in, out
    retval = calculate_frequency(transactionsPath, programInputPath, programOutputPath);
    if (retval)
        exit(-1);

    // Call BOINC finish
    boinc_finish(0);
}