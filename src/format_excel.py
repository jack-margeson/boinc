import pandas as pd
import argparse

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Process an Excel file.')
    parser.add_argument('-i', '--input', type=str, help='Path to the input Excel file',
                        default="../data/online_retail.xlsx")
    args = parser.parse_args()

    format_excel(args.input)

def format_excel(filepath: str): 
    # Create dictionary for storing key value pairs
    transactions = {}
    unique_items = []
    try:
        # Read the Excel file
        df = pd.read_excel(filepath)
        for index, row in df.iterrows():
            if row['StockCode'] not in unique_items:
                unique_items.append(row['StockCode'])
            if row['InvoiceNo'] in transactions:
                transactions[row['InvoiceNo']].append(row['StockCode'])
            else:
                transactions[row['InvoiceNo']] = [row['StockCode']]
    except FileNotFoundError:
        print(f"Error: The file '{filepath}' was not found.")
        return
    except Exception as e:
        print(f"An error occurred while reading the Excel file: {e}")
        return
    
    # Open file for writing 
    with open('../data/transactions.dat', 'w') as f:
        for (key, value) in transactions.items():
            line = f"{str(key)};{','.join(map(str, value))}\n"
            f.write(line)

    # Print number of unique value entries 
    print(f'Unique stock codes: {str(len(unique_items))}')

if __name__ == "__main__":
    main()
