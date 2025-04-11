class ComparatorNetwork:
    def __init__(self, n):
        # n = nr de fire
        self.n = n
        self.comparators = []  # lista de comparatoare - (i, j)

    def add_comparator(self, i, j):
        """Add comparator for wires i and j."""
        if i >= self.n or j >= self.n or i < 0 or j < 0: # i si j interv 0, n-1?
            raise ValueError("The index is out of range.")
        self.comparators.append((i, j))
        # print("17. environment network add_comparator")

    def remove_comparator(self, index):
        """Remove comparator for wires index."""
        if index < 0 or index >= len(self.comparators): # index valid => intre 0 si nr comp -1
            raise IndexError("The comparator index is out of range.")
        del self.comparators[index]
        # print("18. environment network remove_comparator")

    def reset(self):
        """Reset the network."""
        self.comparators = []
        # print("19. environment network reset")

    def __str__(self):
        """Textual representation of the network."""
        # print("20. environment network str")
        return f"ComparatorNetwork(n={self.n}, comparators={self.comparators})"
