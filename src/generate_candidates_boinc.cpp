#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <string.h>

#include "boinc_api.h"
#include "filesys.h"
#include "util.h"

int k = 1;
double cpu_time = 20, comp_result;

typedef std::vector<std::string> Items;
typedef std::set<std::set<std::string>> Candidates;

int generate_candidates(char *in, char *out)
{
    Items items;
    char input_path[1024], output_path[1024];
    char buf[256];

    // Resolve input filename
    boinc_resolve_filename(in, input_path, sizeof(input_path));
    FILE *infile = boinc_fopen(input_path, "r");
    if (!infile)
    {
        fprintf(stderr,
                "%s Couldn't find input file, resolved name %s.\n",
                boinc_msg_prefix(buf, sizeof(buf)), input_path);
        return -1;
    }

    // Resolve output filename
    boinc_resolve_filename(out, output_path, sizeof(output_path));
    FILE *outfile = boinc_fopen(output_path, "w");
    if (!outfile)
    {
        fprintf(stderr,
                "%s Couldn't find output file, resolved name %s.\n",
                boinc_msg_prefix(buf, sizeof(buf)), output_path);
        fclose(infile);
        return -1;
    }

    char line[256];
    // Add each line to items
    while (fgets(line, sizeof(line), infile))
    {
        line[strcspn(line, "\n")] = 0;
        items.push_back(line);
    }

    // Sort items (required for next_permutation)
    std::sort(items.begin(), items.end());

    // Use std::next_permutation to generate all combinations (n choose k)
    Candidates candidates;
    do
    {
        std::set<std::string> candidate;
        for (int i = 0; i < k; i++)
        {
            candidate.insert(items[i]);
        }
        candidates.insert(candidate);
    } while (std::next_permutation(items.begin(), items.end()));

    // Save candidates to output file
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

    // Close input and output files
    fclose(infile);
    fclose(outfile);
    return 0;
}

int main(int argc, char **argv)
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

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-k"))
        {
            k = atoi(argv[++i]);
        }
        else
        {
            retval = generate_candidates(argv[i], argv[i + 1]);
            if (retval)
                exit(-1);
            i++;
        }
    }
    // Call BOINC finish
    boinc_finish(0);
}
