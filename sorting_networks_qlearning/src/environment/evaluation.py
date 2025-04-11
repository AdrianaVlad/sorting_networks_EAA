import itertools

def apply_network(input_list, comparators): # lista de intrari + secv de comparatoare
    data = input_list.copy()
    for comp in comparators:
        i, j = comp
        if data[i] > data[j]:
            data[i], data[j] = data[j], data[i]
    # print("15. environment evaluation apply_network")
    return data

def evaluate_network(network, n):
    """
    Check whether the network (the list of comparators) correctly sorts all binary inputs.
    All input combinations of length n are generated (based on the 0-1 principle).
    """

    # generarea tuturor comb posibile de 0 si 1 de lungime n
    for inp in itertools.product([0, 1], repeat=n):
        output = apply_network(list(inp), network.comparators) # apl ret de sort la intrarea curenta
        if output != sorted(inp):
            return False
    # print("16. environment evaluation evaluate_network")
    return True
