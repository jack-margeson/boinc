#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <string.h>

typedef std::vector<std::string> Items;
typedef std::set<std::set<std::string>> Candidates;

int generate_candidates(char *in, char *out, char *k)
{
    Items items;
    FILE *infile = fopen(in, "r");
    if (!infile)
    {
        fprintf(stderr, "Couldn't find input file %s.\n", in);
        return -1;
    }
    else
    {
        char line[256];
        // Add each line to items
        while (fgets(line, sizeof(line), infile))
        {
            line[strcspn(line, "\n")] = 0;
            items.push_back(line);
        }
    }
    fclose(infile);
    // Sort items (required for next_permutation)
    std::sort(items.begin(), items.end());

    // Use std::next_permutation to generate all combinations (n choose k)
    Candidates candidates;
    do
    {
        std::set<std::string> candidate;
        for (int i = 0; i < std::stoi(k); i++)
        {
            candidate.insert(items[i]);
        }
        candidates.insert(candidate);
    } while (std::next_permutation(items.begin(), items.end()));

    // Print all candidates to outfile
    FILE *outfile = fopen(out, "w");
    bool first_candidate = true;
    for (auto &candidate : candidates)
    {
        if (!first_candidate)
            fprintf(outfile, "\n");
        first_candidate = false;
        bool first_item = true;
        for (auto &item : candidate)
        {
            if (!first_item)
                fprintf(outfile, ",");
            first_item = false;
            fprintf(outfile, "%s", item.c_str());
        }
    }
    fclose(outfile);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = generate_candidates(argv[1], argv[2], argv[3]);
    return 0;
}
