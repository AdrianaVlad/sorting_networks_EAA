import unittest
from src.environment.network import ComparatorNetwork
from src.environment.evaluation import evaluate_network

class TestEvaluation(unittest.TestCase):
    def test_apply_network(self):
        network = ComparatorNetwork(n=3)
        network.add_comparator(0, 1)
        network.add_comparator(1, 2)
        is_valid = evaluate_network(network, 3)
        self.assertFalse(is_valid)

if __name__ == '__main__':
    unittest.main()
