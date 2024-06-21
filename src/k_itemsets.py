from itertools import combinations

def generate_candidates(itemsets, length):
    """
    Generate candidate itemsets of a given length from the current itemsets.
    
    Args:
        itemsets (list): The list of current itemsets.
        length (int): The length of the candidate itemsets to generate.
    
    Returns:
        set: A set of candidate itemsets.
    """
    candidates = set()
    itemsets_list = list(itemsets)
    for i in range(len(itemsets_list)):
        for j in range(i+1, len(itemsets_list)):
            candidate = itemsets_list[i].union(itemsets_list[j])
            if len(candidate) == length:
                candidates.add(candidate)
    return candidates

transactions = []
with open("../data/transactions.dat", 'r') as f:
    lines = f.read().splitlines() 
    for line in lines:
        # Split the string into key and values part
        key, values = line.split(';')
        # Split the values part into a list
        values_list = values.split(',')
        transactions.append(values_list)

print(transactions[0:5])