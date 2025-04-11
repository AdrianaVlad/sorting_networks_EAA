import unittest
from src.environment.simulation import SortingNetworkEnv


class TestEnvironment(unittest.TestCase):
    def test_reset(self):
        env = SortingNetworkEnv(n=4)
        state = env.reset()
        self.assertEqual(state, [])

    def test_step_add(self):
        env = SortingNetworkEnv(n=4, max_steps=10)
        env.reset()
        action = ('add', (0, 1))
        state, reward, done, info = env.step(action)
        self.assertEqual(state, [(0, 1)])

    def test_evaluation(self):
        env = SortingNetworkEnv(n=3)
        env.reset()
        actions = [('add', (0, 1)), ('add', (1, 2))]
        for action in actions:
            env.step(action)
        state, reward, done, info = env.step(('add', (0, 2)))
        self.assertTrue(isinstance(done, bool))


if __name__ == '__main__':
    unittest.main()
